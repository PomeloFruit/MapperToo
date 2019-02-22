#include "drawRoads.h"

#include "globals.h"
#include "latLonToXY.h"
#include "StreetsDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"



void roadDrawing::setRoadColourSize(int type, bool highlight, ezgl::renderer &g, double startArea, double currentArea){
    const int HIGHLIGHTFACT = 5;
    int width = ROADWIDTH;
    g.set_color(255,255,255,255); //Ddefault white colour
    //1/(cur/start))
    double inputAdjust=1/(currentArea/startArea);
    double adjustingAdd=(280930.9-(2586289/1221110)) + (2.117982 - 280930.9)/(1 + (pow(inputAdjust/17546410000, 0.7331144)));
    switch(type){
        case HIGHWAY: // yellowish
            width =HIGHWAYWIDTH+adjustingAdd;
            g.set_color(255,238,41,200);
            break;
        case PRIMARY: // white and thick
            width = PRIMWIDTH+adjustingAdd;
            g.set_color(255,255,255,255);
            break;
        case SECONDARY: //===========================================================================
            width = ROADWIDTH+adjustingAdd;
            g.set_color(255,255,255,255);
            break;
        case RESIDENTIAL: //===========================================================================
            width = ROADWIDTH+adjustingAdd-2;
            g.set_color(255,255,255,255);
            break;
        case SERVICE: // light gray
            width = ROADWIDTH+adjustingAdd-2;
            g.set_color(244,244,244,255);
            break;
        default: //=====================================================================================
            break;
    }
    
    if(highlight){
        width = width + HIGHLIGHTFACT;
        g.set_color(255,102,255,175);
    }
    
    g.set_line_width(width);
    g.set_line_cap (ezgl::line_cap::round);
}
//y = (280930.9-(2586289/1221110)) + (2.117982 - 280930.9)/(1 + (x/17546410000)^0.7331144)
 void roadDrawing::drawStreetRoads(int numSegs, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double startArea, double currentArea){
    LatLon from, to;
    bool sufficentlyZoomed;
    bool sufficentlyBig;
    for(int i=0 ; i<numSegs ; i++){
        setRoadColourSize(info.StreetSegInfo[i].type, info.StreetSegInfo[i].clicked, g, startArea, currentArea);
        
        from = info.IntersectionInfo[info.StreetSegInfo[i].fromIntersection].position;
        
        sufficentlyZoomed=(currentArea/startArea)<0.25;
        sufficentlyBig=(info.StreetSegInfo[i].type==0||info.StreetSegInfo[i].type==1||info.StreetSegInfo[i].type==2);
        if(sufficentlyZoomed||sufficentlyBig){
            for(int c=0;c<info.StreetSegInfo[i].numCurvePoints;c++){
                to = getStreetSegmentCurvePoint(c,i);
                drawStraightStreet(from, to, xy, g);
                from = to;
            }
            to = info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;

            drawStraightStreet(from, to, xy, g);
        }
    }
}

void roadDrawing::drawStraightStreet(LatLon &pt1, LatLon &pt2, mapBoundary &xy, ezgl::renderer &g){
    float xInitial, yInitial, xFinal, yFinal;
    
    xInitial = xy.xFromLon(pt1.lon());
    yInitial = xy.yFromLat(pt1.lat());
    xFinal = xy.xFromLon(pt2.lon());
    yFinal = xy.yFromLat(pt2.lat());
   
    g.draw_line({xInitial, yInitial},{xFinal, yFinal});
}

void roadDrawing::drawIntersections(int numInter, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    for(int i = 0 ; i < numInter ; i++){
        drawOneIntersection(i, xy, info, g);
    }
    drawSpecialIntersections(xy, info, g);
}

void roadDrawing::drawSpecialIntersections(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    for(unsigned i = 0 ; i < info.lastIntersection.size() ; i++){
        drawOneIntersection(info.lastIntersection[i], xy, info, g);
    }
}

void roadDrawing::drawOneIntersection(int id, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    const double NORMALRAD = 0.00015;
    const float RADIUSNORM = 0.00003;
    float x, y;
       
    x = xy.xFromLon(info.IntersectionInfo[id].position.lon());
    y = xy.yFromLat(info.IntersectionInfo[id].position.lat());

    if(info.IntersectionInfo[id].clicked) {
        g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/intersection.png"), ezgl::point2d(x-2*RADIUSNORM, y+2*RADIUSNORM));
//        //outer circle (turquoise)
//        g.set_color(110,236,209,125);
//        g.fill_elliptic_arc(ezgl::point2d(x,y),RADIUSHIGH,RADIUSHIGH,0,360);
//        
//        //inner circle (dark navy)
//        g.set_color(0,119,119,255);
//        g.fill_elliptic_arc(ezgl::point2d(x,y),RADIUSHIGH/5,RADIUSHIGH/5,0,360);
        
    } else {
        //regular intersection (white)
        g.set_color(255,255,255,255);
        g.fill_elliptic_arc(ezgl::point2d(x,y),RADIUSNORM,RADIUSNORM,0,360);
    }
}



