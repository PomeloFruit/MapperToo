#include "drawFeatures.h"

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include "ezgl/graphics.hpp"

#include <algorithm>


/* setFeatureColour function
 * - sets the feature colours based on Feature Type
 * 
 * @param type <int> - feature type as defined by layer 2 api
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * @param special <bool> - true when feature is highlighted 
 * 
 * @return void
 */

void featureDrawing::setFeatureColour(infoStrucs &info, int type, ezgl::renderer &g, bool special){
    if(special){
        g.set_color(216, 0, 113, 150);
        //g.set_color(255,236,175,150);
        
    } else if (info.initiateSicko == 1) { 
        switch(type){
            case 0: // unknown = dark gray
                g.set_color(0,0,0,255);
                break;
            case 1: // park = darkish green
                g.set_color(0, 109, 0, 200);
                break;
            case 2: // beach = peach
                g.set_color(255, 255, 0, 200);
                break;
            case 3: // lake = blue 
                g.set_color(0, 0, 87, 255);
                break;
            case 4: // river = dark blue 
                g.set_color(255, 255, 0, 200);
                break;
            case 5: // island = dark green
                g.set_color(0, 109, 0, 200);
                break;
            case 6: // building = darkish gray
                g.set_color(0,0,0,180);
                break;
            case 7: // green space = light green 
                g.set_color(0, 109, 0, 200);
                break;
            case 8: // golf course = putting green
                g.set_color(0, 109, 0, 200);
                break;
            case 9: // stream = light blue
                g.set_color(255, 255, 0, 200);
                break;
            default: //dark gray
                g.set_color(0,0,0,255);
                break;
        }
    }else {
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
                g.set_color(82, 204, 42, 200);
                //g.set_color(100,209, 0, 179);
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


/* drawFeatures function
 * - draw features taking into account their size and current viewing window
 * 
 * @param numFeatures <int> - number of features in the map 
 * @param info <infoStrucs> - object that contains all the data structures created when map is loaded 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * @param currentArea <double> - the current viewing area of the map
 * @param startArea <double> - the initial area of the map 
 * 
 * @return void
 */

void featureDrawing::drawFeatures(int numFeatures, infoStrucs &info, ezgl::renderer &g, double currentArea, double startArea){
    foodDrinkPOICounter = 0;
    touristPOICounter = 0;
    shopsPOICounter = 0; 
    int endDraw = 1;
    
    // determines the types of features to draw based on view port
    if((currentArea/startArea)>0.7){
        endDraw = 2;
    }
    else if((0.8 >(currentArea/startArea))&&((currentArea/startArea)>0.2)){
        endDraw = 3;
    }
    else if((0.3 >(currentArea/startArea))&&((currentArea/startArea)>0.01)){
        endDraw = 4;
    }
    else{
        endDraw = 5;
    }
    
    //drawing by priority number where 1 is the highest and 4 is the lowest (all open features are 4 automatically)
    for(int s=1; s <= endDraw; s++){        
        for(int i = 0; i < numFeatures; i++){
            
            if(info.FeatureInfo[i].priorityNum == s){
                
                setFeatureColour(info, info.FeatureInfo[i].featureType, g, info.FeatureInfo[i].clicked);
                
                if(info.FeaturePointVec[i].size()>1){
                    
                    if(info.FeatureInfo[i].isOpen){
                        
                        for(int p=1; p< static_cast<int>(info.FeaturePointVec[i].size()); p++){   
                            g.set_line_width(3);
                            g.draw_line(info.FeaturePointVec[i][p-1], info.FeaturePointVec[i][p]);
                        }
                        
                    } else { //closed feature
                        
                        g.fill_poly(info.FeaturePointVec[i]);
                        
                    }
                    
                } else { // feature is node
                    
                    const double size = 0.0001;
                    g.fill_rectangle(info.FeaturePointVec[i][0],size,size);
                
                }
            }
        }
    }
}


/* drawPOI function
 * - sets the limit for the number of POIs shown at each zoom level and then calls 
 *   drawOnePOI to draw the POI 
 * 
 * @param numPOI <int> - the number of POIs in the map
 * @param xy <mapBoundary> - allows for conversion of units from lat lon to xy 
 * @param info <infoStrucs> - object that contains all the data structures created when map is loaded 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * @param screenRatio <double> - ratio of current viewing area to total map 
 * 
 * @return void
 */

void featureDrawing::drawPOI(int numPOI, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double screenRatio){
    int limit = 0;
    int spacing = 0;
    
    const double LV1ZOOM = 0.01;
    const double LV2ZOOM = 0.5;
    
    const int POILIMIT1 = 40;
    const int POILIMIT2 = 20;
    const int POILIMIT3 = 15;
    const int SPACING1 = 5;
    const int SPACING2 = 10;
    const int SPACING3 = 15;
    
    drawnPOIs.clear();
        
    if(screenRatio < LV1ZOOM){
        limit = POILIMIT1;
        spacing = SPACING1;
    } else if (screenRatio < LV2ZOOM) {
        limit = POILIMIT2;
        spacing = SPACING2;

    } else {
        limit = POILIMIT3;
        spacing = SPACING3;
    }
    
    foodDrinkLimit = limit; 
    touristLimit = limit;
    shopsLimit = limit;
    
    for(int i=0 ; i<numPOI ; i=i+spacing){   
        drawOnePOI(i, xy, info, g);
    }
    drawClickedPOI(xy, info, g);
}


/* drawClickedPOI function
 * - draws the highlighted POI 
 * 
 * @param xy <mapBoundary> - allows for conversion of units from lat lon to xy 
 * @param info <infoStrucs> - object that contains all the data structures created when map is loaded 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * 
 * @return void
 */

void featureDrawing::drawClickedPOI(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    for(unsigned i=0 ; i<info.lastPOI.size() ; i++){
        drawOnePOI(info.lastPOI[i], xy, info, g);
    }
}


/* drawOnePOI function
 * - sets the limit for the number of POIs shown at each zoom level and then calls 
 *   drawOnePOI to draw the POI 
 * 
 * @param numPOI <int> - the number of POIs in the map
 * @param xy <mapBoundary> - allows for conversion of units from lat lon to xy 
 * @param info <infoStrucs> - object that contains all the data structures created when map is loaded 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * 
 * @return void
 */

void featureDrawing::drawOnePOI(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    const double RADIUS = g.get_visible_world().width()*0.009;
    LatLon newPoint;
    double xNew, yNew;
    
    newPoint = getPointOfInterestPosition(i);
    ezgl::rectangle curBounds=g.get_visible_world();
    
    bool drawPOI = (xy.yFromLat(newPoint.lat())<curBounds.top())
                &&(xy.yFromLat(newPoint.lat())>curBounds.bottom())
                &&(xy.xFromLon(newPoint.lon())>curBounds.left())
                &&(xy.xFromLon(newPoint.lon())<curBounds.right());

    xNew = xy.xFromLon(newPoint.lon());
    yNew = xy.yFromLat(newPoint.lat());
    
    // if the POI is clicked, draw the Selected POI png
    if (info.POIInfo[i].clicked && drawPOI) {
        if(info.initiateSicko == 1){
            g.draw_surface(g.load_png("jesus.png"), ezgl::point2d(xNew-RADIUS, yNew+RADIUS));
        }else{
            g.draw_surface(g.load_png("POI_select.png"), ezgl::point2d(xNew-RADIUS, yNew+RADIUS));
        }
 
        if(info.POIInfo[i].poiNum == 1 && info.poiButtonStatus[0] == 1) {
            
            touristPOICounter++;
            drawnPOIs.push_back(std::make_pair(ezgl::point2d(xNew-RADIUS, yNew+RADIUS), i));
        
        } else if(info.POIInfo[i].poiNum == 2 && info.poiButtonStatus[1] == 1) {
            
            foodDrinkPOICounter++;
            drawnPOIs.push_back(std::make_pair(ezgl::point2d(xNew-RADIUS, yNew+RADIUS), i));
        
        } else if (info.POIInfo[i].poiNum == 3 && info.poiButtonStatus[2] == 1) {
            
            shopsPOICounter++;
            drawnPOIs.push_back(std::make_pair(ezgl::point2d(xNew-RADIUS, yNew+RADIUS), i));
        
        }
        
    // if the POI is not clicked, draw the POI using the icon 
    } else if(drawPOI) {
      
        if(info.POIInfo[i].poiNum == 1 && touristPOICounter < touristLimit && info.poiButtonStatus[0] == 1) {
            
            g.draw_surface(g.load_png("tourist.png"), ezgl::point2d(xNew-RADIUS, yNew+RADIUS));
            touristPOICounter++;
            drawnPOIs.push_back(std::make_pair(ezgl::point2d(xNew-RADIUS, yNew+RADIUS), i));
        
        } else if (info.POIInfo[i].poiNum == 2 && foodDrinkPOICounter < foodDrinkLimit && info.poiButtonStatus[1] == 1) {
            
            g.draw_surface(g.load_png("food.png"), ezgl::point2d(xNew-RADIUS, yNew+RADIUS));
            foodDrinkPOICounter++;
            drawnPOIs.push_back(std::make_pair(ezgl::point2d(xNew-RADIUS, yNew+RADIUS), i));
        
        } else if (info.POIInfo[i].poiNum == 3 && shopsPOICounter < shopsLimit && info.poiButtonStatus[2] == 1) {
            
            g.draw_surface(g.load_png("shop bag.png"), ezgl::point2d(xNew-RADIUS, yNew+RADIUS));
            shopsPOICounter++;
            drawnPOIs.push_back(std::make_pair(ezgl::point2d(xNew-RADIUS, yNew+RADIUS), i));
        
        }
    }
}


/* drawSubways function
 * - draws the subways onto the map if selected to do so (if draw > 0)
 * 
 * @param draw <int> - determines what routes to draw
 * @param xy <mapBoundary> - allows for conversion of units from lat lon to xy 
 * @param info <infoStrucs> - object that contains all the data structures created when map is loaded 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * 
 * @return void
 */

void featureDrawing::drawSubways(int draw, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    if(draw>0){
        drawSubwayRoute(draw, xy, info, g);
        
        for(unsigned i=0 ; i<info.SubwayInfo.size() ; i++){
            drawOneSubway(i, xy, info, g);
        }
    }
}


/* drawOneSubway function
 * - draws the subway station at index i
 * 
 * @param i <int> - index of the subway to draw
 * @param xy <mapBoundary> - allows for conversion of units from lat lon to xy 
 * @param info <infoStrucs> - object that contains all the data structures created when map is loaded 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * 
 * @return void
 */

void featureDrawing::drawOneSubway(unsigned i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    const double RADIUS = g.get_visible_world().width()*0.009;
    double xNew, yNew;
    LatLon drawPoint;
    
    drawPoint = info.SubwayInfo[i].point;
    
    xNew = xy.xFromLon(drawPoint.lon());
    yNew = xy.yFromLat(drawPoint.lat());
    
    if(info.SubwayInfo[i].clicked){
        g.draw_surface(g.load_png("train_click.png"), ezgl::point2d(xNew-RADIUS, yNew+RADIUS));
        return;
    }
    g.draw_surface(g.load_png("train.png"), ezgl::point2d(xNew-RADIUS, yNew+RADIUS));

}


/* drawSubwayRoute function
 * - draws the subway routes if selected to do so
 * 
 * @param draw <int> - coded information for what routes to draw
 * @param xy <mapBoundary> - allows for conversion of units from lat lon to xy 
 * @param info <infoStrucs> - object that contains all the data structures created when map is loaded 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * 
 * @return void
 */

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


/* drawOneSubwayRoute function
 * - draws one subway routes if selected to do so
 * 
 * @param unsigned <r> - index value of subway route to draw
 * @param xy <mapBoundary> - allows for conversion of units from lat lon to xy 
 * @param info <infoStrucs> - object that contains all the data structures created when map is loaded 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * @param t <int> - type of route (subway/train)
 * 
 * @return void
 */

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
 
 
 /* drawStraightSubwaySection function
 * - draws one subway routes if selected to do so
 * 
 * @param pt1 <LatLon> - start point in latlon coordinates
 * @param pt2 <LatLon> - end point in latlon coordinates
 * @param xy <mapBoundary> - allows for conversion of units from lat lon to xy 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * @param high <bool> - where or the route section is highlighted
 * @param t <int> - classification of route type 
 * 
 * @return void
 */

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

// function can be used later to label selective POI

void featureDrawing::drawTextOnPOI(ezgl::renderer &g, infoStrucs &info){
    int namesToBeDrawn=drawnPOIs.size();
    g.set_text_rotation(0);
    //probably set a character limit somewhere around here
    for(int i=0;i<namesToBeDrawn;i++){
        if((i%5)==0){
            g.draw_text(drawnPOIs[i].first, info.POIInfo[drawnPOIs[i].second].name);
        }
    }
    
}