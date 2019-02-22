/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   drawText.cpp
 * Author: sartori2
 * 
 * Created on February 19, 2019, 9:28 PM
 */

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


std::vector<std::pair<double, bool>> anglesWithWay;
double initialArea;

void drawText::initilize(){
    xy.initialize();
}


void drawText::createText(int numStreetSegs, int numStreets, infoStrucs &info, ezgl::renderer &g){
    //for(int i=0;i<numStreets;i++){
        ezgl::rectangle currentRectangle=g.get_visible_world();
        std::vector<int> alreadyDrawnStreets;
        alreadyDrawnStreets.resize(numStreets);
        alreadyDrawnStreets.clear();
        int maxCount=7;
        ezgl::rectangle initial_world({xy.xMin,xy.yMin},{xy.xMax,xy.yMax});
        initialArea=abs((initial_world.right()-initial_world.left())*(initial_world.top()-initial_world.bottom()));
        double currentArea=abs((currentRectangle.right()-currentRectangle.left())*(currentRectangle.top()-currentRectangle.bottom()));
        bool drawHighway=(true);//but make it a bool so I can change it later
        bool drawPrimary=((currentArea/initialArea)<.20);
        bool drawSecondary=((currentArea/initialArea)<.05);
        bool drawResidential=((currentArea/initialArea)<.005);
        bool drawService=((currentArea/initialArea)<.0009);
        bool charCapOff=((currentArea/initialArea)<.005);
        int space; 
        
        int numRoadsDrawn=0;
        int roadTypes=5;
        //add something for trunks
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
        for(int p=0;p<roadTypes;p++){
            for(int i=0;i<numStreetSegs;i=i+space){
                alreadyDrawnStreets.clear();
                //getting intial and final positions 
                LatLon initialPosition=info.IntersectionInfo[info.StreetSegInfo[i].fromIntersection].position;
                LatLon finalPosition=info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;
                
                std::pair<double, bool> angleToUse=findAngle(initialPosition, finalPosition);
                if(info.StreetSegInfo[i].numCurvePoints>0){
                    int bestCurvePoint=indexOfLargestGoodCurvepoint(i, currentRectangle, info);
                    if(bestCurvePoint==0){
                        initialPosition=info.IntersectionInfo[info.StreetSegInfo[i].fromIntersection].position;
                        finalPosition=getStreetSegmentCurvePoint(0, i);
                    }
                    else if(info.StreetSegInfo[i].numCurvePoints==bestCurvePoint){
                        initialPosition=getStreetSegmentCurvePoint(bestCurvePoint, i);
                        finalPosition=info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;
                    }
                    else{
                        initialPosition=getStreetSegmentCurvePoint(bestCurvePoint-1, i);
                        finalPosition=getStreetSegmentCurvePoint(bestCurvePoint, i);
                    }
                    angleToUse=findAngle(initialPosition, finalPosition);
                }

                
                
                g.set_color(0, 0, 0, 255);//for now but I think if I don't wnt to draw things/draw things on different levels I can either: make the size 0 or make it transparent
                bool inBoundsInitial=inBounds(currentRectangle, initialPosition);
                bool inBoundsFinal=inBounds(currentRectangle, finalPosition);
                if((numRoadsDrawn<maxCount)&&(alreadyDrawnStreets[info.StreetSegInfo[i].streetID]<1)&&(inBoundsInitial)&&(inBoundsFinal)
                        &&(((((currentArea/initialArea)*4000<find_distance_between_two_points(initialPosition, finalPosition))||(info.StreetSegInfo[i].type==1))&&(info.StreetSegInfo[i].numCurvePoints==0))
                        ||(((currentArea/initialArea)*4000<find_distance_between_two_points(initialPosition, finalPosition))&&(info.StreetSegInfo[i].numCurvePoints>0)))
                        &&((((info.StreetSegInfo[i].name).length())<17)||(charCapOff))&&(info.StreetSegInfo[i].name.compare("<unknown>"))){
                    //I'm also not going to chose to draw on a thing if the street length is too small
                    //maybe I give some bias to streets with more intersections//longer length?
                    if((drawHighway&&(info.StreetSegInfo[i].type==0)&&p==0)||(drawPrimary&&(info.StreetSegInfo[i].type==1)&&p==1)
                            ||(drawSecondary&&(info.StreetSegInfo[i].type==2)&&p==2)||(drawResidential&&(info.StreetSegInfo[i].type==3)&&p==3)||
                            (drawService&&(info.StreetSegInfo[i].type==4)&&p==4)){
                        std::string stringToDraw=info.StreetSegInfo[i].name;
                        if(getInfoStreetSegment(i).oneWay){
                            if(angleToUse.second){
                                stringToDraw=info.StreetSegInfo[i].name+"=>";
                            }
                            else{
                                stringToDraw="<="+stringToDraw;
                            }
                        }
                        double xPlace=xy.xFromLon(initialPosition.lon())+((xy.xFromLon((finalPosition.lon()))-xy.xFromLon(initialPosition.lon()))/2);
                        double yPlace=xy.yFromLat(initialPosition.lat()+((finalPosition.lat()-initialPosition.lat())/2));
                        g.format_font("sans serif", ezgl::font_slant::normal, ezgl::font_weight::normal, 11);
                        g.set_text_rotation(angleToUse.first);
                        //g.draw_text({xPlace, yPlace}, info.StreetSegInfo[i].name, xy.xFromLon(finalPosition.lon()-initialPosition.lon()), xy.yFromLat(finalPosition.lat()-initialPosition.lat()));
                        g.draw_text({xPlace, yPlace}, stringToDraw);
                        alreadyDrawnStreets[info.StreetSegInfo[i].streetID]++;
                        numRoadsDrawn++;
                }
            }
        }
    }
}


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
    return std::make_pair(180*angle/M_PI, right);
    //so basically if I had to add pi at any point the arrow should appear on the left side instead of the right 
}

int drawText::indexOfLargestGoodCurvepoint(int streetSegment, ezgl::rectangle& curBounds, infoStrucs &info){
    int bestCurvePoint=0;
    double distance=0;
    LatLon prevLocation=info.IntersectionInfo[info.StreetSegInfo[streetSegment].fromIntersection].position;
    for(int x=0;x<info.StreetSegInfo[streetSegment].numCurvePoints;x++){
        LatLon curLocation=getStreetSegmentCurvePoint(x, streetSegment);
        if(inBounds(curBounds, prevLocation)&&inBounds(curBounds, curLocation)&&(find_distance_between_two_points(curLocation, prevLocation)>distance))
            bestCurvePoint=x;//->meaning from curvepoint i-1 to curvepoint i
        prevLocation=curLocation;
    }
    return bestCurvePoint;
}
//3 cases->i=0 (best curve is from start to i), i!=0&&i!=end(best curve is from i-1 to i), i=end(best curve is from i to end))

bool drawText::inBounds(ezgl::rectangle& curBounds, LatLon& position){
    return (xy.yFromLat(position.lat())<curBounds.top())&&(xy.yFromLat(position.lat())>curBounds.bottom())&&(xy.xFromLon(position.lon())>curBounds.left())&&(xy.xFromLon(position.lon())<curBounds.right());
}

//so some of the intersections work as planned, others don't<-this problem persists and idk why

//if there are multiple street segments belonging to the same street present I should place it on the one that is geographically in the middle (but if it's a curvepoint thing just don't bother)
//^^I think doing this would be pretty computationally intensive to the point where I really don't think it's a good idea
//putting the names down is really just an endless black hole, I could keep doing this forever

//OPTIMIZING LOADMAP:
/*
 * 
 * 
 * 
 */
