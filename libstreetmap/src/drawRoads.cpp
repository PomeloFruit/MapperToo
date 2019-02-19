#include "drawRoads.h"

#include "globals.h"
#include "latLonToXY.h"
#include "StreetsDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"

 void roadDrawing::setRoadColourSize(int type, ezgl::renderer &g){
    g.set_line_width(ROADWIDTH);
    switch(type){
        case HIGHWAY: // yellowish
            g.set_line_width(HIGHWAYWIDTH);
            g.set_color(247,247,62,255);
            break;
        case PRIMARY: // white and thick
            g.set_line_width(PRIMWIDTH);
            g.set_color(255,255,255,255);
            break;
        case SECONDARY: //
            g.set_color(255,255,255,255);
            break;
        case RESIDENTIAL: //
            g.set_color(255,255,255,255);
            break;
        case SERVICE: // light gray
            g.set_color(244,244,244,255);
            break;
        default:
            g.set_color(255,255,255,255);
            break;
    }
}

 void roadDrawing::drawStreetRoads(int numSegs, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    LatLon from, to;
    
    for(int i=0 ; i<numSegs ; i++){
        
        from = info.IntersectionInfo[info.StreetSegInfo[i].fromIntersection].position;

        for(int c=0;c<info.StreetSegInfo[i].numCurvePoints;c++){
            to = getStreetSegmentCurvePoint(c,i);
            drawStraightStreet(from, to, xy, g);
            from = to;
        }
        to = info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;
        
        setRoadColourSize(info.StreetSegInfo[i].type, g);
        drawStraightStreet(from, to, xy, g);
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
        float x = xy.xFromLon(info.IntersectionInfo[i].position.lon());
        float y = xy.yFromLat(info.IntersectionInfo[i].position.lat());
        
        float width=0.00003; ///no magic numbers!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        float height=width;
        g.fill_rectangle({x-width, y-height},{x+width, y+height});
    }
}



