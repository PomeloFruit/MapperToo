
#include "drawText.h"

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"
#include <math.h>
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include <string>

/* initialize
 *  - Gives drawText access to the current xy object
 * 
 * @param void 
 * @return void
 */

void drawText::initilize(){
    xy.initialize(); 
}


/* createText
 *  - Determines which text needs to be drawn and where it should be drawn and if it needs to be drawn
 * 
 * @param numStreetSegs <int> - The number of street segments present in the map
 * @param numStreets <int> - The number of streets present in the map
 * @param info <infoStrucs> - An object that contains various data structures filled with info relevant to the map
 * @param g <ezgl::renderer> - The EZGL renderer
 * @return void
 */

void drawText::createText(int numStreetSegs, int numStreets, infoStrucs &info, ezgl::renderer &g){
        //initialization of all the variables that need in the function
        ezgl::rectangle currentRectangle=g.get_visible_world();
        //alreadyDrawnStreets ensures that streets will not be drawn more than once
        std::vector<int> alreadyDrawnStreets;
        alreadyDrawnStreets.resize(numStreets);
        alreadyDrawnStreets.clear();
        //maxCount is the initial maximum number of streets that will be drawn 
        int maxCount=7;
        //current area is the area that is currently being shown by the program
        //initial area is the area that was shown at the start of the program (or on load of a new map)
        double currentArea=abs((currentRectangle.right()-currentRectangle.left())*(currentRectangle.top()-currentRectangle.bottom()));
        ezgl::rectangle startRectangle({xy.xMin,xy.yMin},{xy.xMax,xy.yMax});
        initialArea=abs((startRectangle.right()-startRectangle.left())*(startRectangle.top()-startRectangle.bottom()));
        
        //using the ratio between the currentArea and initialArea to know at what
        //zoom we are at, then using that to determine which roads to draw
        bool drawHighway=(true);
        bool drawPrimary=((currentArea/initialArea)<.20);
        bool drawSecondary=((currentArea/initialArea)<.05);
        bool drawResidential=((currentArea/initialArea)<.005);
        bool drawService=((currentArea/initialArea)<.0009);
        bool charCapOff=((currentArea/initialArea)<.0005);
        //space is 'jump' between the checked street seg numbers
        int space; 
        //numRoadsDrawn is the number of roads that have already been drawn
        int numRoadsDrawn=0;
        //roadTypes is the number of roads we want to draw (there are 7 kinds of roads)
        int roadTypes=6;
        //the charLimit prevents very large names from being drawn when the roads are very small
        unsigned charLimit=17;
        int sizeAdjustmentFactor=4000;
        g.format_font("sans serif", ezgl::font_slant::normal, ezgl::font_weight::normal, 11);
       
        //determines what space and maxCounter should be
        spaceMaxCountDeterminer(space, maxCount, drawPrimary, drawSecondary, drawResidential);
        
        //Goes through all the streets determining how/if the names should be drawn for each
        //names are drawn in order of the street priority (more important roads are drawn first) and then by their seg ID
        for(int p=0;p<roadTypes;p++){
            for(int i=0;i<numStreetSegs;i=i+space){
                int roadType=info.StreetSegInfo[i].type;
                std::string roadName=info.StreetSegInfo[i].name;
                alreadyDrawnStreets.clear();
                //getting intial and final positions 
                LatLon initialPosition=info.IntersectionInfo[info.StreetSegInfo[i].fromIntersection].position;
                LatLon finalPosition=info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;
                
                //the road will only draw if it is during its "priority" time to draw
                bool timeToDraw=((drawHighway&&(roadType==HIGHWAY)&&p==HIGHWAY)
                            ||(drawPrimary&&(roadType==PRIMARY)&&p==PRIMARY)
                            ||(drawSecondary&&(roadType==SECONDARY)&&p==SECONDARY)
                            ||(drawResidential&&(roadType==RESIDENTIAL)&&p==RESIDENTIAL)
                            ||(drawService&&(roadType==SERVICE)&&p==SERVICE)
                            ||(drawService&&(roadType==TRUNK)&&p==TRUNK));
                
                
                //finding the angle and determining if the direction needs to be flipped if one way
                std::pair<double, bool> angleToUse=findAngle(initialPosition, finalPosition);
                
                //if the street segment contains curvepoints the initial and final positions must change for the calcuation of the angle
                //this block determines if the initial and final positions should change, and what they should be if they do change
                
                if(info.StreetSegInfo[i].numCurvePoints>0){
                    //returns the new latlon positions as a pair
                    std::pair<LatLon, LatLon> initFinalPosition=positionDeterminer(info, i, currentRectangle);
                    initialPosition=initFinalPosition.first;
                    finalPosition=initFinalPosition.second;
                    //re-gets the angle if needed with the new positions
                    angleToUse=findAngle(initialPosition, finalPosition);
                }
                
                
                //setting the colour and determining if either the initial position or the final position is in bounds 
                g.set_color(0, 0, 0, 255);
                bool inBoundsInitial=inBounds(currentRectangle, initialPosition);
                bool inBoundsFinal=inBounds(currentRectangle, finalPosition);
                                
                //determines if the road should be drawn based on a large variety of parameters
                //the number of roads drawn must be less than the maximum, names will only be drawn once on each street
                //either the first or last position must be in bounds for the street to be drawn
                //the segment must be sufficiently large
                //the segment must have either less than a certain amount of characters or we must be zoomed in far enough
                bool validToDraw=((numRoadsDrawn<maxCount)
                    &&(alreadyDrawnStreets[info.StreetSegInfo[i].streetID]<1)
                    &&((inBoundsInitial)||(inBoundsFinal))
                    &&(((((currentArea/initialArea)*sizeAdjustmentFactor<find_distance_between_two_points(initialPosition, finalPosition))
                    ||(roadType==PRIMARY))
                    &&(info.StreetSegInfo[i].numCurvePoints==HIGHWAY))
                    ||(((currentArea/initialArea)*sizeAdjustmentFactor<find_distance_between_two_points(initialPosition, finalPosition))
                    &&(info.StreetSegInfo[i].numCurvePoints>0)))
                    &&((((roadName).length())<charLimit)
                    ||(charCapOff))
                    &&(roadName.compare("<unknown>")));

                if(validToDraw){
                    
                    if(timeToDraw){
                        std::string stringToDraw=info.StreetSegInfo[i].name;
                        //determining which way to place the one way markers if the road is one way
                        if(getInfoStreetSegment(i).oneWay){
                            if(angleToUse.second){
                                stringToDraw="> "+roadName+" >";
                            }
                            else{
                                stringToDraw="< "+roadName+" <";
                            }
                        }
                        
                        //get the x and y values, set the rotation format the text and draw it
                        double xPlace=xy.xFromLon(initialPosition.lon())+((xy.xFromLon((finalPosition.lon()))-xy.xFromLon(initialPosition.lon()))/2);
                        double yPlace=xy.yFromLat(initialPosition.lat()+((finalPosition.lat()-initialPosition.lat())/2));
                        g.set_text_rotation(angleToUse.first);
                        g.draw_text({xPlace, yPlace}, stringToDraw);
                        alreadyDrawnStreets[info.StreetSegInfo[i].streetID]++;
                        numRoadsDrawn++;
                }
            }
        }
    }
}


/* findAngle
 *  - Determines the angle at which the text should be placed and also loads in a bool that tells if one way directions should be flipped
 * 
 * @param initialPosition <LatLon> - The initial position in latitude and longitude
 * @param finalPosition <LatLon> - The final position in latitude and longitude
 * @return std::pair<double, bool>
 */

std::pair<double, bool> drawText::findAngle(LatLon &initialPosition, LatLon &finalPosition){
    double angle=atan2(xy.yFromLat(finalPosition.lat())-xy.yFromLat(initialPosition.lat()), (xy.xFromLon(finalPosition.lon())-xy.xFromLon(initialPosition.lon())));
    bool right=true;
    if(angle<(-M_PI/2)){
        angle=angle+M_PI;
        right=false;
    }
    else if(angle>(M_PI/2)){
        angle=angle+M_PI;
        right=false;
    }
    //converting into degrees
    return std::make_pair(180*angle/M_PI, right);
}


/* indexOfLargestGoodCurvepoint
 *  - For when the street contains curvepoints
 *  - This function determines on which segment of the street segment to draw the name
 * 
 * @param streetSegment <int> - The index of the street segment that is to be drawn on
 * @param info <infoStrucs> - An object that contains various data structures filled with info relevant to the map
 * @param currentRectangle <ezgl::rectangle> - the rectangle representing the current bounds of the map
 * @return void
 */

int drawText::indexOfLargestGoodCurvepoint(int streetSegment, ezgl::rectangle& curBounds, infoStrucs &info){
    int bestCurvePoint=0;
    double distance=0;
    LatLon prevLocation=info.IntersectionInfo[info.StreetSegInfo[streetSegment].fromIntersection].position;
    for(int x=0;x<info.StreetSegInfo[streetSegment].numCurvePoints;x++){
        LatLon curLocation=getStreetSegmentCurvePoint(x, streetSegment);
        if(inBounds(curBounds, prevLocation)&&inBounds(curBounds, curLocation)&&(find_distance_between_two_points(curLocation, prevLocation)>distance))
            //This means that the best segment of the street segment is from curvepoint i-1 to curvepoint i
            bestCurvePoint=x;
        prevLocation=curLocation;
    }
    return bestCurvePoint;
}


/* inBounds
 *  - Determines if a given point is in bounds
 * 
 * @param xy <mapBoundary> - An object that contains the bounds of the original map and functions to convert to and from LatLon/XY
 * @param currentRectangle <ezgl::rectangle> - the rectangle representing the current bounds of the map
 * @param position <LatLon> - the position that is to be determined to be in bounds or not (in latitude and longitude)
 * @return bool
 */

bool drawText::inBounds(ezgl::rectangle& curBounds, LatLon& position){
    return (xy.yFromLat(position.lat())<curBounds.top())&&(xy.yFromLat(position.lat())>curBounds.bottom())&&(xy.xFromLon(position.lon())>curBounds.left())&&(xy.xFromLon(position.lon())<curBounds.right());
}

/* spaceMaxCountDeterminer
 *  - Determines space and maxCount
 * 
 * @param space <int> - space is 'jump' between the checked street seg numbers
 * @param maxCount <int> - the maximum number of streets that will be drawn 
 * @param drawPrimary <bool> - States if primary roads should be drawn
 * @param drawSecondary <bool> - States if secondary roads should be drawn
 * @param drawResidential <bool> - States if residential roads should be drawn
 * @return void
 */

void drawText::spaceMaxCountDeterminer(int& space, int& maxCount, bool drawPrimary, bool drawSecondary, bool drawResidential){
    if(drawPrimary){
        maxCount=maxCount+3;
        space = 5;
    }
    if(drawSecondary){
        maxCount=maxCount+3;
        space = 50;
    }
    if(drawResidential){
        maxCount=maxCount+0;
        space = 5;
    }
}

/* positionDeterminer
 *  - Determines the new initial and final positions
 * 
 * @param info <infoStrucs> - An object that contains various data structures filled with info relevant to the map
 * @param index <int> - the current street segment index
 * @param currentRectangle <ezgl::rectangle> - the rectangle representing the current bounds of the map
 * @return std::pair<LatLon, LatLon>
 */

std::pair<LatLon, LatLon> drawText::positionDeterminer(infoStrucs &info, int index, ezgl::rectangle currentRectangle){
    int bestCurvePoint=indexOfLargestGoodCurvepoint(index, currentRectangle, info);
    LatLon initialPosition;
    LatLon finalPosition;
    //There are 3 different cases
    //either the best spot is after the inital position but before the first curvepoint
    if(bestCurvePoint==0){
        initialPosition=info.IntersectionInfo[info.StreetSegInfo[index].fromIntersection].position;
        finalPosition=getStreetSegmentCurvePoint(0, index);
    }
    //the second case is that the best point is after the final curvepoint and before
    //the final point
    else if(info.StreetSegInfo[index].numCurvePoints==bestCurvePoint){
        initialPosition=getStreetSegmentCurvePoint(bestCurvePoint, index);
        finalPosition=info.IntersectionInfo[info.StreetSegInfo[index].toIntersection].position;
    }
    //the final case is when the best spot is between two curvepoints
    else{
        initialPosition=getStreetSegmentCurvePoint(bestCurvePoint-1, index);
        finalPosition=getStreetSegmentCurvePoint(bestCurvePoint, index);
    }
    return std::make_pair(initialPosition, finalPosition);
}