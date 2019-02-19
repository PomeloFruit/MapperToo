/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m1.h"
#include "m2.h"
//#include "drawRoads.h"
//#include "drawFeatures.h"
#include "latLonToXY.h"
//#include "globals.h"
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

//==================================================================================

const int ROADWIDTH = 3;
const int PRIMWIDTH = 4;
const int HIGHWAYWIDTH = 5;

const int HIGHWAY = 0;
const int PRIMARY = 1;
const int SECONDARY = 2;
const int RESIDENTIAL = 3;
const int SERVICE = 4;

mapBoundary coordinates;

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

struct subwayData {
    std::string name;
    LatLon point;
    
    const OSMNode* nodePtr; 
};

std::map<OSMID, const OSMWay*> WayMap;

std::vector<subwayData> SubwayInfo;

std::vector<intersectionData> IntersectionInfo;

std::vector<streetSegData> StreetSegInfo;

std::vector<featureData> FeatureInfo;
std::vector<std::vector<ezgl::point2d>> FeaturePointVec;

std::vector<POIData> POIInfo;

void populateOSMWayInfo();
void populateOSMSubwayInfo();
int getRoadType(const OSMWay* wayPtr);
std::string getOSMSubwayName(const OSMNode* currentPtr);
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

//void populateOSMSubwayInfo(){
//    SubwayInfo.clear();
//    subwayData newStop;
//    bool isSubway = false;
//    const OSMWay* currentPtr;
//
//    for(unsigned i=0 ; i< getNumberOfNodes(); i++){
//        currentPtr = getNodeByIndex(i);
//        isSubway = checkIfSubway(currentPtr);
//        
//        if(isSubway){
//            newStop.name = getOSMSubwayName(currentPtr);
//            SubwayInfo.insert(newStop);
//        }
//    }
//}

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
    return SERVICE;
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
            
            xNew = coordinates.xFromLon(newPoint.lon());
            yNew = coordinates.yFromLat(newPoint.lat());
            
            FeaturePointVec[i].push_back(ezgl::point2d(xNew,yNew));
        }
        std::cout << "n - " << FeatureInfo[i].name << " t - " << FeatureInfo[i].featureType << std::endl;
    }
}

void populatePOIInfo(){
    int numPOI = getNumPointsOfInterest();
   
    POIInfo.resize(numPOI);
        
    for(int i=0 ; i<numPOI ; i++){
        
        
        POIInfo[i].name = getPointOfInterestName(i);
        POIInfo[i].type = getPointOfInterestType(i);
        
      //  std::cout << "n - " << POIInfo[i].name << " t - " << POIInfo[i].type << std::endl;
    }
}

///////////================================================================================

void setFeatureColour(int type, ezgl::renderer &g);
void setRoadColourSize(int type, ezgl::renderer &g);
void drawFeatures(int numFeatures, ezgl::renderer &g);
void drawStreetRoads(int numSegs, ezgl::renderer &g);
void drawStraightStreet(LatLon pt1, LatLon pt2, ezgl::renderer &g);
void drawIntersections(int numInter, ezgl::renderer &g);
void drawPOI(int numPOI, ezgl::renderer &g);

/////////////================================================================================

void setFeatureColour(int type, ezgl::renderer &g){
    std::cout << "hi\n";
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
        
void setRoadColourSize(int type, ezgl::renderer &g){
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
          std::cout << "hi2\n";
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
        
        setRoadColourSize(StreetSegInfo[i].type, g);
        drawStraightStreet(from, to, g);
    }
}

void drawStraightStreet(LatLon pt1, LatLon pt2, ezgl::renderer &g){
    float xInitial, yInitial, xFinal, yFinal;
    
    xInitial = coordinates.xFromLon(pt1.lon());
    yInitial = coordinates.yFromLat(pt1.lat());
    xFinal = coordinates.xFromLon(pt2.lon());
    yFinal = coordinates.yFromLat(pt2.lat());
   
    g.draw_line({xInitial, yInitial},{xFinal, yFinal});
}

void drawIntersections(int numInter, ezgl::renderer &g){
     for(int i = 0 ; i < numInter ; i++){
        float x = coordinates.xFromLon(IntersectionInfo[i].position.lon());
        float y = coordinates.yFromLat(IntersectionInfo[i].position.lat());
        
        float width=0.00003; ///no magic numbers!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //float width=10;
        float height=width;
        g.fill_rectangle({x-width, y-height},{x+width, y+height});
    }
}

void drawPOI(int numPOI, ezgl::renderer &g){
    LatLon newPoint;
    double xNew, yNew;
    
    double radius = 0.00025;
     for(int i=0 ; i<numPOI ; i++){
        newPoint = getPointOfInterestPosition(i);
            
        xNew = coordinates.xFromLon(newPoint.lon());
        yNew = coordinates.yFromLat(newPoint.lat());
        
        g.set_color(255,0,0,255);
        g.fill_elliptic_arc(ezgl::point2d(xNew,yNew),radius,radius,0,360);
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
    
    populateOSMWayInfo();
    populateStreetSegInfo();
    populateIntersectionInfo();

    populatePOIInfo();
     
    coordinates.initialize();
    
    double xMax, xMin, yMax, yMin; 
    xMax = coordinates.xFromLon(coordinates.maxLon);
    xMin = coordinates.xFromLon(coordinates.minLon);
    yMax = coordinates.yFromLat(coordinates.maxLat);
    yMin = coordinates.yFromLat(coordinates.minLat); 
    
        populateFeatureInfo();
        
    ezgl::rectangle initial_world({xMin,yMin},{xMax,yMax});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world);
    
    application.run(NULL,NULL,NULL,NULL);
    
}

void draw_main_canvas(ezgl::renderer &g){
    g.set_color(219,219,219,255); //light gray for background
    g.fill_rectangle(g.get_visible_world());
    
    drawFeatures(getNumFeatures(),g);
    drawStreetRoads(getNumStreetSegments(), g);
    drawIntersections(getNumIntersections(), g);
    drawPOI(getNumPointsOfInterest(), g);
}
    



