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
            if(info.FeatureInfo[i].isOpen){
                for(int p=1; p< static_cast<int>(info.FeaturePointVec[i].size()); p++){
                    g.set_line_width(3); ///////////////////////////////////////////////////////magic  number???????
                    g.draw_line(info.FeaturePointVec[i][p-1], info.FeaturePointVec[i][p]);
                }
            } else { //closed feature
                g.fill_poly(info.FeaturePointVec[i]);
            }
        } else { // feature is node
            g.fill_rectangle(info.FeaturePointVec[i][0],0.0001,0.0001);///////////////////////////////////////////////////////magic  number???????
        }
    }
}


// draw all
void featureDrawing::drawPOI(int numPOI, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    for(int i=0 ; i<numPOI ; i++){
        drawOnePOI(i, xy, info, g);
    }
    drawClickedPOI(xy, info, g);
}

// make sure last clicked is on top of others
void featureDrawing::drawClickedPOI(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    for(unsigned i=0 ; i<info.lastPOI.size() ; i++){
        drawOnePOI(info.lastPOI[i], xy, info, g);
    }
    
}

void featureDrawing::drawOnePOI(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    const double HIGHLIGHTRAD = 0.0005;
    const double NORMALRAD = 0.00015;
    
    LatLon newPoint;
    double xNew, yNew, radius; 
    
    newPoint = getPointOfInterestPosition(i);       
    xNew = xy.xFromLon(newPoint.lon());
    yNew = xy.yFromLat(newPoint.lat());
    
    if (info.POIInfo[i].clicked) {
        radius = NORMALRAD;
        g.set_color(0,255,174,255);
        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
        radius = HIGHLIGHTRAD;
        g.set_color(214,115,246,100);
        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
    } else {
        radius = NORMALRAD;
        g.set_color(255,0,0,255);
        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
    }
}