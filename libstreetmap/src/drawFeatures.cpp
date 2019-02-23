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
    //food drink more like only show 50
    //note: i don't have "fast_food" in here because there are too many foodDrink POIs
std::vector<std::string> shops {"department_store", "general", "kiosk", "mall", "supermarket", "wholesale", "bag", "boutique", "clothes" , "jewelry", "leather"
    , "shoes", "watches", "variety", "second_hand", "beauty", "cosmetics", "erotic", "hairdresser", "herbalist", "massage", "medical_supply", "perfumery"
    , "tattoo", "electrical", "florist", "antiques", "candles", "interior_decoration", "computer", "robot", "electronics", "mobile_phone", "radiotechnics" 
    , "fishing", "fuel", "outdoor", "scuba_diving", "ski", "sports", "swimming_pool", "art", "collector", "craft", "games", "model", "music", "musical_instrument"
    , "photo", "camera", "video", "video_games", "anime", "books", "gift", "stationery", "ticket", "cannabis", "e-cigarette", "tobacco", "toys", "travel_agency"};

    
    
const int POITOURIST = 1;
const int POIFOOD = 2;
const int POISHOPS = 3;
const int POIUNDEF = 0;

const double SUBWAYRAD = 0.0005;
const double HIGHLIGHTRAD = 0.0015;   

const int TRAINROUTE = 3;
const int HIGHTRAINROUTE = 6;
const int SUBWAYROUTE = 5;
const int HIGHSUBWAYROUTE = 10;

int foodDrinkLimit;
int touristLimit; 
int shopsLimit;

int foodDrinkPOICounter;
int touristPOICounter;
int shopsPOICounter; 
    
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
            default: //dark gray
                g.set_color(152,151,150,255);
                break;
        }
    }
}


int featureDrawing::classifyPOI(std::string type){
    int poiType = POIUNDEF; 
    
    if(std::find(tourist.begin(), tourist.end(), type) != tourist.end()){
        poiType = POITOURIST; 
    } else if(std::find(foodDrink.begin(), foodDrink.end(), type) != foodDrink.end()){
        poiType = POIFOOD;
    } else if(std::find(shops.begin(), shops.end(), type) != shops.end()){
        poiType = POISHOPS; 
    } else {
        poiType = POIUNDEF; 
    }
    
    return poiType; 
}
//
//void featureDrawing::setPOIColour(int type, ezgl::renderer& g){
//    if(type == POITOURIST){
//        g.set_color(255,71,113,191);
//    }else if (type == POIFOOD){
//        g.set_color(255, 165, 54, 191);
//    }else if (type == POISHOPS){
//        g.set_color(0, 112, 255, 191);
//    } else { //POIUNDEF
//        g.set_color(0,0,0,0);
//    }
//}

void featureDrawing::drawFeatures(int numFeatures, infoStrucs &info, ezgl::renderer &g, double currentArea, double startArea){
    foodDrinkPOICounter=0;
    touristPOICounter=0;
    shopsPOICounter=0; 
    
    int endDraw=1;
    if((currentArea/startArea)>0.8){
        endDraw=2;
    }
    else if((0.8>(currentArea/startArea))&&((currentArea/startArea)>0.3)){
        endDraw=3;
    }
    else if((0.3>(currentArea/startArea))&&((currentArea/startArea)>0.1)){
        endDraw=4;
    }
    else{
        endDraw=5;
    }
    //std::cout<<endDraw<<'\n';
    for(int s=1; s <= endDraw; s++){        //drawing by priority number where 1 is the highest and 4 is the lowest (all open features are 4 automatically)
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
                        //info.F
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
    bool zoom1=(adjustmentFactor)<0.5;
    bool zoom2=(adjustmentFactor)<0.01;
    
    if(zoom2){
        foodDrinkLimit = 40; 
        touristLimit = 40;
        shopsLimit = 40;
    }else if (zoom1){
        foodDrinkLimit = 20;
        touristLimit = 20;
        shopsLimit = 20;
    }else{
        foodDrinkLimit = 10;
        touristLimit = 10;
        shopsLimit = 10;
    }
    
    for(int i=0 ; i<numPOI ; i++){   
        drawOnePOI(i, xy, info, g, adjustmentFactor, currentArea);
    }
    drawClickedPOI(xy, info, g, adjustmentFactor, currentArea);
}

// make sure last clicked is on top of others
void featureDrawing::drawClickedPOI(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea){
    for(unsigned i=0 ; i<info.lastPOI.size() ; i++){
        drawOnePOI(info.lastPOI[i], xy, info, g, adjustmentFactor, currentArea);
    }
}

void featureDrawing::drawOnePOI(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea){
    const double HIGHLIGHTRADIUS = 0.0005;
    const double radius = g.get_visible_world().width()*0.009;
    //double areaForcer=sqrt(0.000073593*currentArea/M_PI)/NORMALRAD;//that first number is the average of two proportions of areas that were deemed to be desireable
    //double areaForcerClick=sqrt(0.000432814*currentArea/M_PI)/HIGHLIGHTRADIUS;
    //double forcedRadius=areaForcer*NORMALRAD;
    //double forcedRadiusClick=areaForcerClick*HIGHLIGHTRADIUS;
    
    LatLon newPoint;
    double xNew, yNew;
    
    newPoint = getPointOfInterestPosition(i);
    
    ezgl::rectangle curBounds=g.get_visible_world();
    bool drawPOI=
            (xy.yFromLat(newPoint.lat())<curBounds.top())
            &&(xy.yFromLat(newPoint.lat())>curBounds.bottom())
            &&(xy.xFromLon(newPoint.lon())>curBounds.left())
            &&(xy.xFromLon(newPoint.lon())<curBounds.right());

    xNew = xy.xFromLon(newPoint.lon());
    yNew = xy.yFromLat(newPoint.lat());
    
    if (info.POIInfo[i].clicked&&drawPOI) {
//        //highlight circle (pink)
//        g.set_color(255,77,190,125);
//        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
//        
//        //inner circle (dark purple)
//        radius = forcedRadius;
//        g.set_color(79,0,79,255);
//        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
        
        g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/POI_select.png"), ezgl::point2d(xNew-radius, yNew+radius));
        if(info.POIInfo[i].poiNum == 1 && info.poiButtonStatus[0] == 1){
            touristPOICounter++;
        }else if(info.POIInfo[i].poiNum == 2 && info.poiButtonStatus[1] == 1){
            foodDrinkPOICounter++;
        }else if (info.POIInfo[i].poiNum == 3 && info.poiButtonStatus[2] == 1){
            shopsPOICounter++;
        }
    } else if(drawPOI){
        //regular POI (dark red)
        info.POIInfo[i].poiNum = classifyPOI(info.POIInfo[i].type);
//        setPOIColour(poiType, g);
        if(info.POIInfo[i].poiNum == 1 && touristPOICounter < touristLimit && info.poiButtonStatus[0] == 1){
            g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/tourist.png"), ezgl::point2d(xNew-radius, yNew+radius));
            touristPOICounter++;
        } else if (info.POIInfo[i].poiNum == 2 && foodDrinkPOICounter < foodDrinkLimit && info.poiButtonStatus[1] == 1){
            g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/food.png"), ezgl::point2d(xNew-radius, yNew+radius));
            foodDrinkPOICounter++;
        } else if (info.POIInfo[i].poiNum == 3 && shopsPOICounter < shopsLimit && info.poiButtonStatus[2] == 1){
            g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/shop bag.png"), ezgl::point2d(xNew-radius, yNew+radius));
            shopsPOICounter++;
        }
//        if((info.POIInfo[i].poiNum!=2)){
//            if(info.POIInfo[i].poiNum == 1){
//                g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/tourist.png"), ezgl::point2d(xNew-radius, yNew+radius));
//            }else if (info.POIInfo[i].poiNum == 3){
//                g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/shop bag.png"), ezgl::point2d(xNew-radius, yNew+radius));
//            }
//            //g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
//        }
//        else if(foodDrinkPOICounter<50){
//            g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/food.png"), ezgl::point2d(xNew-radius, yNew+radius));
////            g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
//            foodDrinkPOICounter++;
//        }
//            //g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/small_image.png"), ezgl::point2d(xNew, yNew));
//        //radius = NORMALRAD;
//        //setPOIColour(classifyPOI(info.POIInfo[i].type), g);  
//        //g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
    }
}

void featureDrawing::drawSubways(int draw, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    if(draw>0){
        drawSubwayRoute(draw, xy, info, g);
        
        for(unsigned i=0 ; i<info.SubwayInfo.size() ; i++){
            drawOneSubway(i, xy, info, g);
        }
    }
}

void featureDrawing::drawOneSubway(unsigned i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    const double radius = g.get_visible_world().width()*0.009;
    double xNew, yNew;
    LatLon drawPoint;
    
    drawPoint = info.SubwayInfo[i].point;
    g.set_color(79,0,79,255);
    
    xNew = xy.xFromLon(drawPoint.lon());
    yNew = xy.yFromLat(drawPoint.lat());
    
    if(info.SubwayInfo[i].clicked){
//        g.set_color(64,255,194,255);
//        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),SUBWAYRAD,SUBWAYRAD,0,360);
//        g.set_color(255,155,36,150);
//        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),HIGHLIGHTRAD,HIGHLIGHTRAD,0,360);
        g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/train_click.png"), ezgl::point2d(xNew-radius, yNew+radius));
        return;
    }
//    g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),SUBWAYRAD,SUBWAYRAD,0,360);
    g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/train.png"), ezgl::point2d(xNew-radius, yNew+radius));

}

void featureDrawing::drawSubwayRoute(int draw, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    for(unsigned i=0 ; i<info.SubwayRouteInfo.size() ; i++){
        
        int routeType = info.SubwayRouteInfo.at(i).type;
        
        if(routeType == draw){
            drawOneSubwayRoute(i, xy, info, g, draw);
        } else if (draw ==3) {
            drawOneSubwayRoute(i, xy, info, g, routeType);
            drawOneSubwayRoute(i, xy, info, g, routeType);
        }
    }
}

 void featureDrawing::drawOneSubwayRoute(unsigned r, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, int t){
    LatLon from, to;
    unsigned start = 0;
    subwayRouteData &temp = info.SubwayRouteInfo.at(r);
    
    for(unsigned i=0 ; i<temp.point.size() ; i++){

        from = temp.point[i][start];
            
        for(unsigned j=start ; j<temp.point[i].size() ; j++){
            to = temp.point[i][j];
            drawStraightSubwaySection(from, to, xy, g, temp.clicked, t);
            from = to;
        }
    }
}

void featureDrawing::drawStraightSubwaySection(LatLon &pt1, LatLon &pt2, mapBoundary &xy, ezgl::renderer &g, bool high, int t){
    float xInitial, yInitial, xFinal, yFinal;
    
    xInitial = xy.xFromLon(pt1.lon());
    yInitial = xy.yFromLat(pt1.lat());
    xFinal = xy.xFromLon(pt2.lon());
    yFinal = xy.yFromLat(pt2.lat());
        
    if(t==1){
        g.set_color(255,153,0,255);
        g.set_line_width(SUBWAYROUTE);
    } else if (t==2) {
        g.set_color(0,34,157,179);
        g.set_line_width(TRAINROUTE);
    }
    g.draw_line({xInitial, yInitial},{xFinal, yFinal});
    
    if(high){
        if (t==1) { 
            g.set_color(23,186,189,179);
            g.set_line_width(HIGHSUBWAYROUTE);
        } else if (t==2) { 
            g.set_color(23,186,189,179);
            g.set_line_width(HIGHTRAINROUTE);
        }
        g.draw_line({xInitial, yInitial},{xFinal, yFinal});
    }
}