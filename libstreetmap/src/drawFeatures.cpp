#include "drawFeatures.h"

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include "ezgl/graphics.hpp"

#include <algorithm>

std::vector<std::string> tourist {"aquarium", "artwork", "attraction", "gallery", "information", "museum", "theme_park", "zoo", "college", "university", "brothel", "arts_centre"
    , "casino", "cinema", "fountain", "gambling", "music_venue", "nightclub", "planetarium", "theatre", "marketplace", "townhall", "stadium"};
    
std::vector<std::string> foodDrink {"alcohol", "bakery", "beverages", "coffee", "confectionery", "convenience", "ice_cream","pastry", "seafood", "bar", "bbq", "cafe", 
    "food_court", "pub", "restaurant"}; 
    //note: i don't have "fast_food" in here because there are too many foodDrink POIs
std::vector<std::string> shops {"department_store", "general", "kiosk", "mall", "supermarket", "wholesale", "bag", "boutique", "clothes" , "jewelry", "leather"
    , "shoes", "watches", "variety", "second_hand", "beauty", "cosmetics", "erotic", "hairdresser", "herbalist", "massage", "medical_supply", "perfumery"
    , "tattoo", "electrical", "florist", "antiques", "candles", "interior_decoration", "computer", "robot", "electronics", "mobile_phone", "radiotechnics" 
    , "fishing", "fuel", "outdoor", "scuba_diving", "ski", "sports", "swimming_pool", "art", "collector", "craft", "games", "model", "music", "musical_instrument"
    , "photo", "camera", "video", "video_games", "anime", "books", "gift", "stationery", "ticket", "cannabis", "e-cigarette", "tobacco", "toys", "travel_agency"};
        
void featureDrawing::setFeatureColour(int type, ezgl::renderer &g, bool special){
    if(special){
        g.set_color(0,0,0,150);
    } else {
        switch(type){
            case 0: // unknown = dark gray
                g.set_color(152,151,150,255);
                break;
            case 1: // park = darkish green
                g.set_color(85, 209, 49, 79);
                break;
            case 2: // beach = peach
                g.set_color(235,210,111,255);
                break;
            case 3: // lake = blue 
                g.set_color(60, 182, 255, 163);
                //g.set_color(16,184,225,255);
                break;
            case 4: // river = dark blue 
                g.set_color(80, 179, 255, 128);
                break;
            case 5: // island = dark green
                g.set_color(100,209, 0, 99);
                break;
            case 6: // building = darkish gray
                g.set_color(22, 23, 56, 20);
                break;
            case 7: // green space = light green 
                g.set_color(82, 204, 42, 56);
                //g.set_color(95,218,24,255);
                break;
            case 8: // golf course = putting green
                g.set_color(148,235,87,155);
                break;
            case 9: // stream = light blue
                g.set_color(80, 179, 255, 128);
                break;
            default:
                g.set_color(152,151,150,255);
                break;
        }
    }
}
 

int featureDrawing::classifyPOI(std::string type){
    int poiType = 0; 
    
    if(std::find(tourist.begin(), tourist.end(), type) != tourist.end()){
        poiType = 1; 
    } else if(std::find(foodDrink.begin(), foodDrink.end(), type) != foodDrink.end()){
        poiType = 2;
    } else if(std::find(shops.begin(), shops.end(), type) != shops.end()){
        poiType = 3; 
    } else {
        poiType = 0; 
    }
    
    return poiType; 
}

void featureDrawing::setPOIColour(int type, ezgl::renderer& g){
    if(type == 1){
        g.set_color(255,71,113,191);
    }else if (type == 2){
        g.set_color(255, 165, 54, 191);
    }else if (type == 3){
        g.set_color(0, 112, 255, 191);
    }
    else{
        g.set_color(0,0,0,0);
    }
}


void featureDrawing::drawFeatures(int numFeatures, infoStrucs &info, ezgl::renderer &g){
    for(int s = 1; s <= 4; s++){        //drawing by priority number where 1 is the highest and 4 is the lowest (all open features are 4 automatically)
        for(int i = 0; i < numFeatures; i++){
            if(info.FeatureInfo[i].priorityNum == s){
                setFeatureColour(info.FeatureInfo[i].featureType, g, info.FeatureInfo[i].clicked);
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
    }
}


// draw all
void featureDrawing::drawPOI(int numPOI, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea){
    bool sufficentlyZoomed=(adjustmentFactor)<0.01;
    if(sufficentlyZoomed){
        for(int i=0 ; i<numPOI ; i++){   
            drawOnePOI(i, xy, info, g, adjustmentFactor, currentArea);
        }
        drawClickedPOI(xy, info, g, adjustmentFactor, currentArea);
    }
}

// make sure last clicked is on top of others
void featureDrawing::drawClickedPOI(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea){
    for(unsigned i=0 ; i<info.lastPOI.size() ; i++){
        drawOnePOI(info.lastPOI[i], xy, info, g, adjustmentFactor, currentArea);
    }
}

void featureDrawing::drawOnePOI(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea){
    const double HIGHLIGHTRAD = 0.0005;
    const double NORMALRAD = 0.00015;
    double areaForcer=sqrt(0.000073593*currentArea/M_PI)/NORMALRAD;//that first number is the average of two proportions of areas that were deemed to be desireable
    double areaForcerClick=sqrt(0.000432814*currentArea/M_PI)/HIGHLIGHTRAD;
    double forcedRadius=areaForcer*NORMALRAD;
    double forcedRadiusClick=areaForcerClick*HIGHLIGHTRAD;
    
    LatLon newPoint;
    double xNew, yNew, radius; 
    
    newPoint = getPointOfInterestPosition(i);       
    xNew = xy.xFromLon(newPoint.lon());
    yNew = xy.yFromLat(newPoint.lat());
    
    if (info.POIInfo[i].clicked) {
        //highlight circle (pink)
        radius = forcedRadiusClick;
        g.set_color(255,77,190,125);
        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
        
        //inner circle (dark purple)
        radius = forcedRadius;
        g.set_color(79,0,79,255);
        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);

    } else {
        //regular POI (dark red)
        radius = forcedRadius;
        setPOIColour(classifyPOI(info.POIInfo[i].type), g); 
        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
    }
}

void featureDrawing::drawSubways(bool draw, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    if(draw){
        for(unsigned i=0 ; i<info.SubwayInfo.size() ; i++){
            drawOneSubway(i, xy, info, g);
        }
    }
}

void featureDrawing::drawOneSubway(unsigned i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    const double SUBWAYRAD = 0.0005;
    const double HIGHLIGHTRAD = 0.0015;
    
    double xNew, yNew;
    LatLon drawPoint;
    
    drawPoint = info.SubwayInfo[i].point;
    g.set_color(79,0,79,255);
    
    xNew = xy.xFromLon(drawPoint.lon());
    yNew = xy.yFromLat(drawPoint.lat());
    
    if(info.SubwayInfo[i].clicked){
        g.set_color(64,255,194,255);
        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),SUBWAYRAD,SUBWAYRAD,0,360);
        g.set_color(255,155,36,150);
        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),HIGHLIGHTRAD,HIGHLIGHTRAD,0,360);
        return;
    }
    
    g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),SUBWAYRAD,SUBWAYRAD,0,360);
}