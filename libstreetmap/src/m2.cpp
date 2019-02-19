/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m1.h"
#include "m2.h"
#include "drawRoads.h"
#include "drawFeatures.h"
#include "latLonToXY.h"
//#include "globals.h" //I think this can be removed
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "OSMID.h"
#include <math.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <map>

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"

const int ROADWIDTH = 3;
const int PRIMWIDTH = 4;
const int HIGHWAYWIDTH = 5;

const int HIGHWAY = 0;
const int PRIMARY = 1;
const int SECONDARY = 2;
const int RESIDENTIAL = 3;
const int SERVICE = 4;

///////////================================================================================

double averageLatInRad;
double maxLat, maxLon, minLat, minLon;

// finds the average latitude within the map bounds and stores in averageLatInRad
void averageLat();

//returns the y coordinate corresponding the latitude
float yFromLat(float lat);

//returns the x coordinate corresponding the longitude
float xFromLon(float lon);

///////////================================================================================

void averageLat(){
    
    maxLat = getIntersectionPosition(0).lat();
    minLat = maxLat;
    maxLon = getIntersectionPosition(0).lon();
    minLon = maxLon;
    
    int numOfIntersections = getNumIntersections();
    
    //find min and max lat/lon points
    for(int i=0;i<numOfIntersections;i++){
        if(getIntersectionPosition(i).lat()>maxLat){
            maxLat = getIntersectionPosition(i).lat();
        }
        if(getIntersectionPosition(i).lat()<minLat){
            minLat = getIntersectionPosition(i).lat();
        }
        if(getIntersectionPosition(i).lon()>maxLon){
            maxLon = getIntersectionPosition(i).lon();
        }
        if(getIntersectionPosition(i).lon()<minLon){
            minLon = getIntersectionPosition(i).lon();
        }
    }
    
    averageLatInRad = DEG_TO_RAD*(maxLat+minLat)/2;
}


float xFromLon(float lon){
    float projectionFactor,x;
    
    projectionFactor = cos(averageLatInRad);
    x = lon*projectionFactor;
    
    return x;
}


float yFromLat(float lat){
    return lat;
}

///////////================================================================================


struct intersectionData {
    LatLon position;
    std::string name;
};

struct streetSegData {
    unsigned fromIntersection;
    unsigned toIntersection;
    int numCurvePoints;
    std::string name;
    
    OSMID id;
    const OSMWay* wayPtr;
    int type;
};

struct featureData {
    int featureType;
    std::string name;
};

struct POIData {
    std::string name;
    std::string type;
    
    OSMID id;
};

std::map<OSMID, const OSMWay*> WayMap;

std::vector<intersectionData> IntersectionInfo;

std::vector<streetSegData> StreetSegInfo;

std::vector<featureData> FeatureInfo;
std::vector<std::vector<ezgl::point2d>> FeaturePointVec;

std::vector<POIData> POIInfo;

void populateOSMWayInfo();
int getRoadType(const OSMWay* wayPtr);
void populateStreetSegInfo();
void populateIntersectionInfo();
void populateFeatureInfo();
void populatePOIInfo();

/////////////================================================================================

void populateOSMWayInfo(){
    WayMap.clear();
    const OSMWay* currentWayPtr;
    OSMID currentID;
    
    for(unsigned i=0 ; i<getNumberOfWays() ; i++){
        currentWayPtr = getWayByIndex(i);
        currentID = currentWayPtr->id();
        WayMap.insert({currentID,currentWayPtr});
    }
}

int getRoadType(const OSMWay* wayPtr){
    for(unsigned i=0 ; i<getTagCount(wayPtr) ; i++){
        std::string key,value;
        std::tie(key,value) = getTagPair(wayPtr,i);
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        
        if(key == "highway"){
            if(value == "motorway" || value == "motorway_link"){
                return HIGHWAY;
            } else if (value == "trunk" || value == "trunk_link" || value == "primary"){
                return PRIMARY;
            } else if (value == "secondary" || value == "tertiary"){
                return SECONDARY;
            } else if (value == "residential"){
                return RESIDENTIAL;
            } else {
                return SERVICE;
            }
        }
    }
}

void populateStreetSegInfo(){
    int numStreetSegments = getNumStreetSegments();
    StreetSegInfo.resize(numStreetSegments);
    
    for(int i=0;i<numStreetSegments;i++){
        StreetSegInfo[i].fromIntersection = getInfoStreetSegment(i).from;
        StreetSegInfo[i].toIntersection = getInfoStreetSegment(i).to;
        StreetSegInfo[i].numCurvePoints = getInfoStreetSegment(i).curvePointCount;
        StreetSegInfo[i].id = getInfoStreetSegment(i).wayOSMID;
        StreetSegInfo[i].wayPtr = WayMap[StreetSegInfo[i].id];
        StreetSegInfo[i].type = getRoadType(StreetSegInfo[i].wayPtr);
    }
}

void populateIntersectionInfo(){
    int numOfIntersections = getNumIntersections();
    IntersectionInfo.resize(numOfIntersections);
    
    for(int i=0;i<numOfIntersections;++i){
        IntersectionInfo[i].position = getIntersectionPosition(i);
        IntersectionInfo[i].name = getIntersectionName(i);
    }
}

void populateFeatureInfo(){
    int numFeatures = getNumFeatures();
    int numPoints;
    LatLon newPoint;
    double xNew, yNew;
    FeatureInfo.resize(numFeatures);
    FeaturePointVec.resize(numFeatures);
    
    for(int i=0;i<numFeatures;i++){
        FeatureInfo[i].name = getFeatureName(i);
        FeatureInfo[i].featureType = getFeatureType(i);
        
        numPoints = getFeaturePointCount(i);
        FeaturePointVec[i].clear();
        
        for(int p=0 ; p<numPoints; p++){
            newPoint = getFeaturePoint(p, i);
            
            xNew = xFromLon(newPoint.lon());
            yNew = yFromLat(newPoint.lat());
            
            FeaturePointVec[i].push_back(ezgl::point2d(xNew,yNew));
        }
    }
}

void populatePOIInfo(){
    int numPOI = getNumPointsOfInterest();
   
    POIInfo.resize(numPOI);
        
    for(int i=0 ; i<numPOI ; i++){
        
        
        POIInfo[i].name = getPointOfInterestName(i);
        POIInfo[i].type = getPointOfInterestType(i);
        
        std::cout << "n - " << POIInfo[i].name << " t - " << POIInfo[i].type << std::endl;
    }
}

///////////================================================================================

void setFeatureColour(int type, ezgl::renderer &g);
void setRoadColour(int type, ezgl::renderer &g);
void drawFeatures(int numFeatures, ezgl::renderer &g);
void drawStreetRoads(int numSegs, ezgl::renderer &g);
void drawStraightStreet(LatLon pt1, LatLon pt2, ezgl::renderer &g);
void drawIntersections(int numInter, ezgl::renderer &g);
void drawPOI(int numPOI, ezgl::renderer &g);

/////////////================================================================================

void setFeatureColour(int type, ezgl::renderer &g){
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
        
void setRoadColour(int type, ezgl::renderer &g){
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

void drawFeatures(int numFeatures, ezgl::renderer &g){
    for(int i=0 ; i<numFeatures ; i++){
        setFeatureColour(FeatureInfo[i].featureType, g);
        
        if(FeaturePointVec[i].size()>1){
            g.fill_poly(FeaturePointVec[i]);
        } else {
            g.fill_rectangle(FeaturePointVec[i][0],0.0001,0.0001);
        }
    }
}

void drawStreetRoads(int numSegs, ezgl::renderer &g){
    LatLon from, to;
    
    for(int i=0 ; i<numSegs ; i++){
        
        from = IntersectionInfo[StreetSegInfo[i].fromIntersection].position;

        for(int c=0;c<StreetSegInfo[i].numCurvePoints;c++){
            to = getStreetSegmentCurvePoint(c,i);
            drawStraightStreet(from, to, g);
            from = to;
        }
        to = IntersectionInfo[StreetSegInfo[i].toIntersection].position;
        
        setRoadColour(StreetSegInfo[i].type, g);
        drawStraightStreet(from, to, g);
    }
}

void drawStraightStreet(LatLon pt1, LatLon pt2, ezgl::renderer &g){
    float xInitial, yInitial, xFinal, yFinal;
    
    xInitial=xFromLon(pt1.lon());
    yInitial=yFromLat(pt1.lat());
    xFinal=xFromLon(pt2.lon());
    yFinal=yFromLat(pt2.lat());
   
    g.draw_line({xInitial, yInitial},{xFinal, yFinal});
}

void drawIntersections(int numInter, ezgl::renderer &g){
     for(int i=0 ; i<numInter ; i++){
        float x = xFromLon(IntersectionInfo[i].position.lon());
        float y = yFromLat(IntersectionInfo[i].position.lat());
        
        float width=0.00003; ///no magic numbers!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //float width=10;
        float height=width;
        g.fill_rectangle({x-width, y-height},{x+width, y+height});
    }
}

void drawPOI(int numPOI, ezgl::renderer &g){
    LatLon newPoint;
    double xNew, yNew;
    
    double radius = 0.00005;
     for(int i=0 ; i<numPOI ; i++){
        newPoint = getPointOfInterestPosition(i);
            
        xNew = xFromLon(newPoint.lon());
        yNew = yFromLat(newPoint.lat());
        
        g.set_color(255,0,0,255);
        g.draw_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
    }
}

///////////================================================================================

void draw_main_canvas(ezgl::renderer &g);

void draw_map(){
    ezgl::application::settings settings;
    settings.main_ui_resource="libstreetmap/resources/main.ui";
    settings.window_identifier="MainWindow";
    settings.canvas_identifier="MainCanvas";
    ezgl::application application(settings);
    
    averageLat();//to make sure that we have the max stuff/min stuff so we can set bounds later
    populateOSMWayInfo();
    populateStreetSegInfo();
    populateIntersectionInfo();
    populateFeatureInfo();
    
    ezgl::rectangle initial_world({xFromLon(minLon),yFromLat(minLat)},{xFromLon(maxLon),yFromLat(maxLat)});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world);
    
    application.run(NULL,NULL,NULL,NULL);
    
}


//
////doesn't really need to exist but like hey maybe it'll be useful one day
//std::pair<double, double> Lat_Lon_To_X_Y(LatLon point){ //call average lat before using this 
//    double projectionFactor = cos(averageLatInRad);
//    double x = point.lon()*projectionFactor;
//    double y = point.lat();
//    return std::make_pair(x,y);
//}


void draw_main_canvas(ezgl::renderer &g){
    //g.draw_rectangle({xFromLon(minLon),yFromLat(minLat)},{xFromLon(maxLon),yFromLat(maxLat)});//questionable?
    g.set_color(219,219,219,255);
    g.fill_rectangle(g.get_visible_world());
    
    drawFeatures(getNumFeatures(),g);
    drawStreetRoads(getNumStreetSegments(), g);
    drawIntersections(getNumIntersections(), g);
    drawPOI(getNumPointsOfInterest(), g);
}
    



