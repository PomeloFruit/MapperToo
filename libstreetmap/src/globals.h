// prevents double includes
#pragma once

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "OSMID.h"
#include "ezgl/point.hpp"

#include <string>
#include <iostream>
#include <map>

//============================== Constants ===================================

#define TRUNKWIDTH 4
#define SECONDARYWIDTH 1
#define PRIMWIDTH 3
#define HIGHWAYWIDTH 5

#define HIGHWAY 0
#define PRIMARY 1
#define SECONDARY 2
#define RESIDENTIAL 3
#define SERVICE 4
#define TRUNK 5
#define HIGHWAYRAMP 6

//POI Type Codes
#define POITOURIST 1
#define POIFOOD 2
#define POISHOPS 3
#define POIUNDEF 0

//Drawing Codes for Train/Subway Routes
#define TRAINROUTE 3
#define HIGHTRAINROUTE 6
#define SUBWAYROUTE 5
#define HIGHSUBWAYROUTE 10

//============================== Structures ===================================

struct intersectionData {
    LatLon position;
    std::string name;
    
    bool clicked;
};

struct streetSegData {
    unsigned fromIntersection;
    unsigned toIntersection;
    int numCurvePoints;
    
    std::string name;
    int type;
    unsigned streetID;
    
    OSMID id;
    const OSMWay* wayPtr;
    
    bool clicked;
};

struct featureData {
    int featureType;
    std::string name;
    int priorityNum; 
    
    OSMID id;
    bool isOpen;
    
    bool clicked;
};

struct POIData {
    std::string name;
    std::string type; 
    int poiNum; 
    
    OSMID id;
    bool clicked;
};

struct subwayRouteData {
    std::string name;
    std::string operatorName;
    
    std::vector< std::vector< LatLon > > point;
    std::vector< std::vector< OSMID > > nodePoints;
    
    int type;
    bool clicked;
};

struct subwayData {
    std::string name;
    LatLon point; 
    std::vector< unsigned > routeNum;
    
    const OSMNode* nodePtr; 
    OSMID id;
    
    bool clicked;
};

//============================== Class ===================================

//infoStrucs holds its all, all the map features are held here
class infoStrucs {
public:
    // organized map to quickly match OSMID with way points
    std::map<OSMID, const OSMWay*> WayMap;

    // contains subway station data
    std::vector<subwayData> SubwayInfo;
    
    // contains route (subway and train) data)
    std::vector<subwayRouteData> SubwayRouteInfo;

    // contains intersection data
    std::vector<intersectionData> IntersectionInfo;

    // contains street segment data
    std::vector<streetSegData> StreetSegInfo;
    
    // contains features data
    std::vector<featureData> FeatureInfo;

    // contains features xy coordinate points
    std::vector< std::vector<ezgl::point2d> > FeaturePointVec;

    // contains poi data
    std::vector<POIData> POIInfo;
    
    // contains the "last" highlighted indices
    std::vector<unsigned> lastIntersection, lastPOI, lastSeg, lastFeature, lastSubway;
    
    // contains the input from the text fields
    std::string textInput1, textInput2, textInput3, textInput4;
    
    // contains the output to the text fields/ corrected names
    std::string corInput1, corInput2;
    
    // contains whether or not to show 0-none/1-subways/2-trains/3-both
    int showRoute;
    
    bool findDirections;
   
    // Holds the number of each street type in each map 
    // 0 for Motorway, 1 for Primary ... 4 for Service, 5 for Trunk
    int numStreetType [6];
    
    // stores each street in the map along with its streetType (ie Trunk, Motorway etc..) 
    std::vector<std::pair<unsigned, int>> streetType;
    
    // shows which button is on (0 means don't show, 1 means show)
    int poiButtonStatus [4];
    
    // initiates sicko mode
    int initiateSicko;
};

