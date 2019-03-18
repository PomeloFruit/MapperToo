#include "drawRoads.h"

#include "globals.h"
#include "latLonToXY.h"
#include "StreetsDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"


/* setRoadColurSize
 *  - sets the roads colour and size based on the parameters below
 *  - equations found in the function were determined by finding points that looked good and then using curve fitting
 * 
 * @param type <int> - The type of street as classified in fillStructs 
 * @param highlight <bool> - determines if the road should be highlighted or not
 * @param g <ezgl::renderer> - The EZGL renderer
 * @param startArea <double> - The initial area of the canvas
 * @param currentArea <double> - the current area of the canvas
 * 
 * @return void
 */

void roadDrawing::setRoadColourSize(infoStrucs &info, int type, bool highlight, ezgl::renderer &g, double startArea, double currentArea){
    const int HIGHLIGHTFACT = 1;
    int width = SECONDARYWIDTH;
    g.set_color(255,255,255,255); //Default white colour
    double inputAdjust=1/(currentArea/startArea);
    double adjustingAdd=(280930.9-(2586289/1221110)) + (2.117982 - 280930.9)/(1 + (pow(inputAdjust/17546410000, 0.7331144)));
    if(info.initiateSicko == 1){
        switch(type){
            case HIGHWAY: 
            case HIGHWAYRAMP:// yellowish
                width =HIGHWAYWIDTH + adjustingAdd;
                g.set_color(255, 101, 0, 170);
                break;
            case TRUNK:
                width = PRIMWIDTH + adjustingAdd;
                g.set_color(255, 101, 0, 255);
                break;
            case PRIMARY: // white and thick
                width = PRIMWIDTH + adjustingAdd;
                g.set_color(255, 101, 0, 255);
                break;
            case SECONDARY:
                width = SECONDARYWIDTH + adjustingAdd;
                g.set_color(255, 101, 0, 255);
                break;
            case RESIDENTIAL:
                width = adjustingAdd;
                g.set_color(255, 101, 0, 255);
                break;
            case SERVICE:
                width = adjustingAdd;
                g.set_color(255, 101, 0, 255);
                break;
            default:
                break;
        }
    }else{
        switch(type){
            case HIGHWAY: 
            case HIGHWAYRAMP:// yellowish
                width =HIGHWAYWIDTH + adjustingAdd;
                g.set_color(250,215,56,200);
                break;
            case TRUNK:
                width = PRIMWIDTH + adjustingAdd;
                g.set_color(255,255,255,255);
                break;
            case PRIMARY: // white and thick
                width = PRIMWIDTH + adjustingAdd;
                g.set_color(255,255,255,255);
                break;
            case SECONDARY:
                width = SECONDARYWIDTH + adjustingAdd;
                g.set_color(255,255,255,255);
                break;
            case RESIDENTIAL:
                width = adjustingAdd;
                g.set_color(255,255,255,255);
                break;
            case SERVICE:
                width = adjustingAdd;
                g.set_color(244,244,244,255);
                break;
            default:
                break;
        }
    }
      
    if(highlight){
        width = width + HIGHLIGHTFACT;
        g.set_color(255,102,255,175);
    }
    
    g.set_line_width(width);
    g.set_line_cap (ezgl::line_cap::round);
}


/* drawStreetRoads
 *  - Determines which street segments should be drawn and calls another function to draw them
 * 
 * @param numSegs <int> - The number of street segments on the map
 * @param xy <mapBoundary> - An object that contains the bounds of the original map and functions to convert to and from LatLon/XY
 * @param info <infoStrucs> - An object that contains various data structures filled with info relevant to the map
 * @param g <ezgl::renderer> - The EZGL renderer
 * @param startArea <double> - The initial area of the canvas
 * @param currentArea <double> - the current area of the canvas
 * @param currentRectangle <ezgl::rectangle> - the rectangle representing the current bounds of the map
 * 
 * @return void
 */

 void roadDrawing::drawStreetRoads(int numSegs, mapBoundary &xy, infoStrucs &info, 
                    ezgl::renderer &g, double startArea, double currentArea, ezgl::rectangle currentRectangle){
    LatLon from, to;
    bool sufficentlyZoomed=false;
    bool sufficentlyBig=false;
    bool sufficentlyZoomedLevel1=false;
    bool sufficentlyZoomedLevel2=false;
    bool draw=false;
    
    const double RATIO1 = 0.1;
    const double RATIO2 = 0.25;
    const double RATIO3 = 0.05;
    
    const int TRUNKLIM1 = 1000;
    const int TRUNKLIM2 = 100;
    
    for(int i=0 ; i<numSegs ; i++){
        
        setRoadColourSize(info, info.StreetSegInfo[i].type, info.StreetSegInfo[i].clicked, g, startArea, currentArea);
        
        from = info.IntersectionInfo[info.StreetSegInfo[i].fromIntersection].position;
        //============================== DEFINE THE NUMBERS ===================================================
        int &type = info.StreetSegInfo[i].type;
        
        
        if((100<info.numStreetType[TRUNK]) && (info.numStreetType[TRUNK] < TRUNKLIM1)){
            
            sufficentlyBig = (type == HIGHWAY || type == TRUNK || type == PRIMARY);
            sufficentlyZoomedLevel1 = (type == SECONDARY) && ((currentArea/startArea) < RATIO2);
            sufficentlyZoomed = (currentArea/startArea) < RATIO1;
            
        }
        else if(info.numStreetType[TRUNK] < TRUNKLIM2){
            
            sufficentlyBig=(type == HIGHWAY || type == TRUNK || type == PRIMARY || type == SECONDARY);
            sufficentlyZoomed=(currentArea/startArea) < RATIO2;
            
        }
        else{
            
            sufficentlyBig=(type == HIGHWAY || type == TRUNK);
            sufficentlyZoomedLevel1 = (type == PRIMARY) && ((currentArea/startArea) < RATIO2);
            sufficentlyZoomed = (currentArea/startArea) < RATIO3;
            sufficentlyZoomedLevel2 = (type == SECONDARY) && (currentArea/startArea) < RATIO1; 
            
        }
         
        if(sufficentlyZoomed || sufficentlyBig || sufficentlyZoomedLevel1 || sufficentlyZoomedLevel2){
           
            for(int c=0;c<info.StreetSegInfo[i].numCurvePoints;c++){
                
                to = getStreetSegmentCurvePoint(c,i);
                draw=(inBounds(xy, currentRectangle, to))||(inBounds(xy, currentRectangle, from));
                
                if(draw){
                    drawStraightStreet(from, to, xy, g);
                }
                from = to;
            }
            
            to = info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;
            draw = (inBounds(xy, currentRectangle, to))||(inBounds(xy, currentRectangle, from));
            
            if(draw){
                    drawStraightStreet(from, to, xy, g);
            }
            
        }
    }
    for(int i=0;i<info.lastSeg.size();i++){
        setRoadColourSize(info, HIGHWAY, true, g, startArea, currentArea);//all roads that are highlighted on a path will be drawn at the same width as a highway and shit
       //^^all roads that are being in lastSeg should be highlighted
        //this is also the most scuffed way of doing this
        from = getIntersectionPosition(getInfoStreetSegment(info.lastSeg[i]).from);
        for(int j=0;j<getInfoStreetSegment(info.lastSeg[i]).curvePointCount;j++){
            to = getStreetSegmentCurvePoint(j,info.lastSeg[i]);
            draw=(inBounds(xy, currentRectangle, to))||(inBounds(xy, currentRectangle, from));

            if(draw){
                drawStraightStreet(from, to, xy, g);
            }
            from = to;
        }
        to=getIntersectionPosition(getInfoStreetSegment(info.lastSeg[i]).to);
        //to = info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;
        draw = (inBounds(xy, currentRectangle, to))||(inBounds(xy, currentRectangle, from));

        if(draw){
                drawStraightStreet(from, to, xy, g);
        }
    }
}

 
 /* drawStraightStreet
 *  - Draws a straight line on the map according to the specifications set out in setRoadColourSize. Called by drawStreetRoads
  * 
 * @param pt1 <LatLon> - The latitude and longitude of the first point
 * @param pt2 <LatLon> - The latitude and longitude of the second point
 * @param xy <mapBoundary> - An object that contains the bounds of the original map and functions to convert to and from LatLon/XY
 * @param g <ezgl::renderer> - The EZGL renderer
  * 
 * @return void
 */
 
void roadDrawing::drawStraightStreet(LatLon &pt1, LatLon &pt2, mapBoundary &xy, ezgl::renderer &g){
    float xInitial, yInitial, xFinal, yFinal;
    
    xInitial = xy.xFromLon(pt1.lon());
    yInitial = xy.yFromLat(pt1.lat());
    xFinal = xy.xFromLon(pt2.lon());
    yFinal = xy.yFromLat(pt2.lat());
   
    g.draw_line({xInitial, yInitial},{xFinal, yFinal});
}


/* drawIntersections
 *  - Determines if an intersections is to be drawn and calls a function to draw it if it is
 * 
 * @param numInter <int> - The number of intersections on the map
 * @param xy <mapBoundary> - An object that contains the bounds of the original map and functions to convert to and from LatLon/XY
 * @param info <infoStrucs> - An object that contains various data structures filled with info relevant to the map
 * @param g <ezgl::renderer> - The EZGL renderer
 * 
 * @return void
 */

void roadDrawing::drawIntersections(int numInter, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    ezgl::rectangle currentRectangle=g.get_visible_world();
//    for(int i = 0 ; i < numInter ; i++){
//        if(inBounds(xy, currentRectangle, info.IntersectionInfo[i].position)){
//            drawOneIntersection(i, xy, info, g, 0);
//        }
//    }
    drawSpecialIntersections(xy, info, g);
}


/* drawSpecialIntersections
 *  - Draws special intersections
 * 
 * @param xy <mapBoundary> - An object that contains the bounds of the original map and functions to convert to and from LatLon/XY
 * @param info <infoStrucs> - An object that contains various data structures filled with info relevant to the map
 * @param g <ezgl::renderer> - The EZGL renderer
 * 
 * @return void
 */

void roadDrawing::drawSpecialIntersections(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    if(info.directionStart != -1){
        drawOneIntersection(info.directionStart, xy, info, g, 1);
    }
    if(info.directionEnd != -1){
        drawOneIntersection(info.directionEnd, xy, info, g, 2);
    }
    
    for(unsigned i = 0 ; i < info.lastIntersection.size() ; i++){
        drawOneIntersection(info.lastIntersection[i], xy, info, g, 0);
    }
    
}


/* drawStreetRoads
 *  - Determines if the intersection should be drawn as normal or loaded in with a png
 *  - Then draws the intersection as specified
 * 
 * @param id <int> - The index number of the intersection to be drawn
 * @param xy <mapBoundary> - An object that contains the bounds of the original map and functions to convert to and from LatLon/XY
 * @param info <infoStrucs> - An object that contains various data structures filled with info relevant to the map
 * @param g <ezgl::renderer> - The EZGL renderer
 * 
 * @return void
 */

void roadDrawing::drawOneIntersection(int id, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, int startStop){
    const float RADIUSNORM = 0.00003;
    const double RADIUS2 = g.get_visible_world().width()*0.009;
    const double RADIUS = g.get_visible_world().width()*0.018;
    float x, y;
       
    x = xy.xFromLon(info.IntersectionInfo[id].position.lon());
    y = xy.yFromLat(info.IntersectionInfo[id].position.lat());

    if(info.IntersectionInfo[id].clicked) {
        if(info.initiateSicko == 1){
            g.draw_surface(g.load_png("jesus.png"), ezgl::point2d(x-RADIUS, y+RADIUS));
        }else{
            g.draw_surface(g.load_png("intersection.png"), ezgl::point2d(x-RADIUS, y+RADIUS));
        }
    } else if (startStop == 1){ // draw start
        g.draw_surface(g.load_png("POI_select.png"), ezgl::point2d(x-RADIUS2, y+RADIUS2));
    } else if (startStop == 2){ // draw stop
        g.draw_surface(g.load_png("destination.png"), ezgl::point2d(x-RADIUS2, y+RADIUS2));
    } else {
        if(info.initiateSicko == 1){
            g.set_color(0,0,0,255);
        }else{
            g.set_color(255,255,255,255);
        }
        
        g.fill_elliptic_arc(ezgl::point2d(x,y),RADIUSNORM,RADIUSNORM,0,360);
    }
    
}


/* inBounds
 *  - Determines if a given point is in bounds
 * 
 * @param xy <mapBoundary> - An object that contains the bounds of the original map and functions to convert to and from LatLon/XY
 * @param currentRectangle <ezgl::rectangle> - the rectangle representing the current bounds of the map
 * @param position <LatLon> - the position that is to be determined to be in bounds or not (in latitude and longitude)
 * 
 * @return inBounds <bool> - if the draw within the bounds
 */

bool roadDrawing::inBounds(mapBoundary &xy, ezgl::rectangle& curBounds, LatLon& position){
    bool inBounds;
    
    inBounds = (xy.yFromLat(position.lat())<curBounds.top())
            && (xy.yFromLat(position.lat())>curBounds.bottom())
            && (xy.xFromLon(position.lon())>curBounds.left())
            && (xy.xFromLon(position.lon())<curBounds.right());

    return inBounds;
}
