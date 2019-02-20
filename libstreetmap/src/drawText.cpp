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

//changing angles
//showing 1way
std::vector<double> angles;
double initialArea;
void drawText::initilize(int numStreetSegs, ezgl::rectangle& startRectangle, mapBoundary &xy, infoStrucs &info){
    //ezgl::rectangle startRectangle=g.get_visible_world();
    initialArea=abs((startRectangle.right()-startRectangle.left())*(startRectangle.top()-startRectangle.bottom()));
    angles.resize(numStreetSegs);
    for(int i=0;i<numStreetSegs;i++){
        LatLon initialPosition=info.IntersectionInfo[info.StreetSegInfo[i].fromIntersection].position;
        LatLon finalPosition=info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;
        //double slope=(xy.yFromLat(finalPosition.lat())-xy.yFromLat(initialPosition.lat()))/(xy.xFromLon(finalPosition.lon())-xy.xFromLon(initialPosition.lon()));
//        double xPlace=xy.xFromLon(initialPosition.lon())+find_street_segment_length(i)/2;
        //double yPlace=xy.yFromLat(initialPosition.lat())+slope*(find_street_segment_length(i))/2;//this is not gucchi
        //math time
        angles[i]=findAngle(initialPosition, finalPosition, xy);
        //std::cout<<i<<'\n';
    }
}


void drawText::createText(int numStreetSegs, int numStreets, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    //for(int i=0;i<numStreets;i++){
        ezgl::rectangle currentRectangle=g.get_visible_world();
        std::vector<int> alreadyDrawnStreets;
        alreadyDrawnStreets.resize(numStreets);
        alreadyDrawnStreets.clear();
        int maxCount=6;
        double currentArea=abs((currentRectangle.right()-currentRectangle.left())*(currentRectangle.top()-currentRectangle.bottom()));
        bool drawHighway=(true);//but make it a bool so I can change it later
        bool drawPrimary=((currentArea/initialArea)<.20);
        bool drawSecondary=((currentArea/initialArea)<.007);
        bool drawResidential=((currentArea/initialArea)<.005);
        bool drawService=((currentArea/initialArea)<.0009);
        bool charCapOff=((currentArea/initialArea)<.00009);
//        bool drawPrimary=true;
//        bool drawSecondary=true;
//        bool drawResidential=true;
//        bool drawService=true;
        //bool drawOther??????=;
        //std::cout<<drawHighway<<" "<<drawPrimary<<" "<<drawSecondary<<" "<<drawResidential<<" "<<drawService<<'\n';
        //std::cout<<currentArea/initialArea<<'\n';
        //1.0293->0.370549->.133398->0.0480231->0.0172883->0.0062238->0.00224057->0.000806604
        //it also seems like no roads are listed as primary
        int numRoadsDrawn=0;
        int roadTypes=5;
        if(drawPrimary){
            maxCount=maxCount+5;
        }
        if(drawSecondary){
            maxCount=maxCount+3;
        }
        if(drawResidential){
            maxCount=maxCount+4000;
        }
        for(int p=0;p<roadTypes;p++){
            for(int i=0;i<numStreetSegs;i++){//->this should be for numTypes
                alreadyDrawnStreets.clear();

                LatLon initialPosition=info.IntersectionInfo[info.StreetSegInfo[i].fromIntersection].position;
                LatLon finalPosition=info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;
                double angleToUse=angles[i];
                if(info.StreetSegInfo[i].numCurvePoints>0){
                    int bestCurvePoint=indexOfLargestGoodCurvepoint(i, currentRectangle, xy, info);
                    if(bestCurvePoint==0){
                        initialPosition=info.IntersectionInfo[info.StreetSegInfo[i].fromIntersection].position;
                        finalPosition=getStreetSegmentCurvePoint(0, i);//so rn if the first curvepoint is oob I simply do not show the street at all
                    }
                    else if(info.StreetSegInfo[i].numCurvePoints==bestCurvePoint){
                        initialPosition=getStreetSegmentCurvePoint(bestCurvePoint, i);
                        finalPosition=info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;
                    }
                    else{
                        initialPosition=getStreetSegmentCurvePoint(bestCurvePoint-1, i);
                        finalPosition=getStreetSegmentCurvePoint(bestCurvePoint, i);
                    }
                    //and I really did try to only use these info boys but I needed the api for this one doc
                    //for curvepoints I gotta actually calculate the angle
                    angleToUse=findAngle(initialPosition, finalPosition, xy);
                }
                //^^I'm going to have to try all the curvepoints->try to place it on the longest curvepoint, then in decreasing order
                //double xPlace=xy.xFromLon((cos(DEG_TO_RAD*angle)*find_street_segment_length(i)/2)/((xy.maxLon-xy.minLon)+initialPosition.lon()));
                //double yPlace=xy.yFromLat(initialPosition.lat()+(slope*(sin(DEG_TO_RAD*angle)*find_street_segment_length(i)/2)/((xy.maxLat-xy.minLat))));
                g.set_color(0, 0, 0, 255);//for now but I think if I don't wnt to draw things/draw things on different levels I can either: make the size 0 or make it transparent
    //            bool inBoundsInitial=(xy.yFromLat(initialPosition.lat())<currentRectangle.top())&&(xy.yFromLat(initialPosition.lat())>currentRectangle.bottom())&&(xy.xFromLon(initialPosition.lon())>currentRectangle.left())&&(xy.xFromLon(initialPosition.lon())<currentRectangle.right());//->make this into function later
    //            bool inBoundsFinal=(xy.yFromLat(finalPosition.lat())<currentRectangle.top())&&(xy.yFromLat(finalPosition.lat())>currentRectangle.bottom())&&(xy.xFromLon(finalPosition.lon())>currentRectangle.left())&&(xy.xFromLon(finalPosition.lon())<currentRectangle.right());//->make this into function later
                bool inBoundsInitial=inBounds(currentRectangle, initialPosition, xy);
                bool inBoundsFinal=inBounds(currentRectangle, finalPosition, xy);
                //std::cout<<inBoundsFinal<<" "<<inBoundsInitial<<'\n';
                if((numRoadsDrawn<maxCount)&&(alreadyDrawnStreets[info.StreetSegInfo[i].streetID]<1)&&(inBoundsInitial)&&(inBoundsFinal)
                        &&(((((currentArea/initialArea)*4000<find_distance_between_two_points(initialPosition, finalPosition))||(info.StreetSegInfo[i].type==1))&&(info.StreetSegInfo[i].numCurvePoints==0))
                        ||(((currentArea/initialArea)*4000<find_distance_between_two_points(initialPosition, finalPosition))&&(info.StreetSegInfo[i].numCurvePoints>0)))
                        &&((((info.StreetSegInfo[i].name).length())<17)||(charCapOff))){//and potentially in bounds->check if it's already been drawn this drawing sesh, if yes don't draw again (pending change) if no (this might shit on highways))
                    //I'm also not going to chose to draw on a thing if the street length is too small
                    //maybe I give some bias to streets with more intersections//longer length?
                    if((drawHighway&&(info.StreetSegInfo[i].type==0)&&p==0)||(drawPrimary&&(info.StreetSegInfo[i].type==1)&&p==1)
                            ||(drawSecondary&&(info.StreetSegInfo[i].type==2)&&p==2)||(drawResidential&&(info.StreetSegInfo[i].type==3)&&p==3)||
                            (drawService&&(info.StreetSegInfo[i].type==4)&&p==4)){
                        double xPlace=xy.xFromLon(initialPosition.lon())+((xy.xFromLon((finalPosition.lon()))-xy.xFromLon(initialPosition.lon()))/2);
                        double yPlace=xy.yFromLat(initialPosition.lat()+((finalPosition.lat()-initialPosition.lat())/2));
                        //g.set_font_size(16);
                        g.format_font("serif", ezgl::font_slant::italic, ezgl::font_weight::bold, 6);//I'm just gonna use serif for now but I know it's supposed to be sans serif was 4->I gotta make the thing bigger
                        g.set_text_rotation(angleToUse);
                        //g.draw_text({xPlace, yPlace}, info.StreetSegInfo[i].name, xy.xFromLon(finalPosition.lon()-initialPosition.lon()), xy.yFromLat(finalPosition.lat()-initialPosition.lat()));
                        g.draw_text({xPlace, yPlace}, info.StreetSegInfo[i].name);
                        //std::cout<<xPlace<<"!!!!!!!"<<yPlace<<" "<<((finalPosition.lat()-initialPosition.lat())/2)<<initialPosition.lat()<<'\n';
                        alreadyDrawnStreets[info.StreetSegInfo[i].streetID]++;
                        numRoadsDrawn++;
                }
            }
        }
    }
}


double drawText::findAngle(LatLon &initialPosition, LatLon &finalPosition, mapBoundary &xy){
    double angle=atan2(xy.yFromLat(finalPosition.lat())-xy.yFromLat(initialPosition.lat()), (xy.xFromLon(finalPosition.lon())-xy.xFromLon(initialPosition.lon())));//this is in lat lon atm
    //std::cout<<(xy.yFromLat(finalPosition.lat())-xy.yFromLat(initialPosition.lat()))<<" "<<(xy.xFromLon(finalPosition.lon())-xy.xFromLon(initialPosition.lon()))<<" "<<angle<<'\n';
    if(angle<(-M_PI/2)){
        angle=angle+M_PI;
    }
    else if(angle>(M_PI/2)){
        angle=angle+M_PI;
    }
    return 180*angle/M_PI;
}

int drawText::indexOfLargestGoodCurvepoint(int streetSegment, ezgl::rectangle& curBounds, mapBoundary& xy, infoStrucs &info){
    int bestCurvePoint=0;
    double distance=0;
    LatLon prevLocation=info.IntersectionInfo[info.StreetSegInfo[streetSegment].fromIntersection].position;
    for(int x=0;x<info.StreetSegInfo[streetSegment].numCurvePoints;x++){
        LatLon curLocation=getStreetSegmentCurvePoint(x, streetSegment);
        if(inBounds(curBounds, prevLocation, xy)&&inBounds(curBounds, curLocation, xy)&&(find_distance_between_two_points(curLocation, prevLocation)>distance))
            bestCurvePoint=x;//->meaning from curvepoint i-1 to curvepoint i
        prevLocation=curLocation;
    }
    return bestCurvePoint;
}
//3 cases->i=0 (best curve is from start to i), i!=0&&i!=end(best curve is from i-1 to i), i=end(best curve is from i to end))

bool drawText::inBounds(ezgl::rectangle& curBounds, LatLon& position, mapBoundary& xy){
    return (xy.yFromLat(position.lat())<curBounds.top())&&(xy.yFromLat(position.lat())>curBounds.bottom())&&(xy.xFromLon(position.lon())>curBounds.left())&&(xy.xFromLon(position.lon())<curBounds.right());
}
//so I want to draw the text a little up the road
//if (street_length>blah_amount)&&(currentArea/initialArea)->street gets (some allowance of placements of name))
//I'm pretty sure it checks if the shit is out of bounds automatically, if not I gotta do that
//I gotta do that <-I did that
//I gotta do curvepoints<-sorta done
//if a thing has a curvepoint just draw before the first one NO DON'T DO THAT
//part of me wants to say just like oh if it says ramp just don't put that shit on ezklap
//I want the distance from initial to final to be whatever not the distance of the whole seg (because if I do it this way I can discriinaate against curvepoints)
//so some of the intersections work as planned, others don't<-this problem persists and idk why

//I think this gave me cancer
//also it still doesn't really work that well
//maybe I just stop showing stuff once I plot all the important stuff
//so that the shit is kept to a minimum

//if there are multiple street segments belonging to the same street present I should place it on the one that is geographically in the middle (but if it's a curvepoint thing just don't bother)
//^^I think doing this would be pretty computationally intensive to the point where I really don't think it's a good idea
//putting the names down is really just an endless black hole, I could keep doing this forever

//TODO: DRAW ARROWS ON ONEWAYS
//RESIZE ROADS
//EXCLUDE DRAWING EVERYTHING AT A DISTANCE
//IF I REALLY HAVE TIME I'D HAVE TO GO BACK AND MAKE DRAWING THE ROAD NAMES EVEN BETTER