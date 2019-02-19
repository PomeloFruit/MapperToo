#include "drawFeatures.h"

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include "ezgl/graphics.hpp"


void featureDrawing::setFeatureColour(int type, ezgl::renderer &g){
    switch(type){
        case 0: // unknown = dark gray
            g.set_color(152,151,150,255);
            break;
        case 1: // park = darkish green
            g.set_color(92,201,103,255);
            break;
        case 2: // beach = peach
            g.set_color(235,210,111,255);
            break;
        case 3: // lake = blue 
            g.set_color(16,184,225,255);
            break;
        case 4: // river = dark blue 
            g.set_color(0,120,149,255);
            break;
        case 5: // island = dark green
            g.set_color(53,92,17,255);
            break;
        case 6: // building = darkish gray
            g.set_color(127,129,125,255);
            break;
        case 7: // green space = light green 
            g.set_color(95,218,24,255);
            break;
        case 8: // golf course = putting green
            g.set_color(161,23,115,225);
            break;
        case 9: // stream = light blue
            g.set_color(115,226,234,255);
            break;
        default:
            g.set_color(152,151,150,255);
            break;
    }
}
        

void featureDrawing::drawFeatures(int numFeatures, infoStrucs &info, ezgl::renderer &g){
    for(int i=0 ; i<numFeatures ; i++){
        setFeatureColour(info.FeatureInfo[i].featureType, g);
        if(info.FeaturePointVec[i].size()>1){
            g.fill_poly(info.FeaturePointVec[i]);
        } else {
            g.fill_rectangle(info.FeaturePointVec[i][0],0.0001,0.0001);
        }
    }
}



void featureDrawing::drawPOI(int numPOI, mapBoundary &xy, ezgl::renderer &g){
    LatLon newPoint;
    double xNew, yNew;
    
    double radius = 0.00025; ///no magic numbers!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     for(int i=0 ; i<numPOI ; i++){
        newPoint = getPointOfInterestPosition(i);
            
        xNew = xy.xFromLon(newPoint.lon());
        yNew = xy.yFromLat(newPoint.lat());
        
        g.set_color(255,0,0,255);
        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
    }
}