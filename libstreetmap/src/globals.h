
#pragma once

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "OSMID.h"
#include "ezgl/point.hpp"

#include <string>
#include <iostream>
#include <map>

#define ROADWIDTH 2
#define PRIMWIDTH 4
#define HIGHWAYWIDTH 5

#define HIGHWAY 0
#define PRIMARY 1
#define SECONDARY 2
#define RESIDENTIAL 3
#define SERVICE 4

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
    
    OSMID id;
    const OSMWay* wayPtr;
    int type;
    bool clicked;
    unsigned streetID;
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
    bool clicked;
    
    OSMID id;
};

struct subwayRouteData {
    std::string name;
    std::vector< LatLon > point;
    std::vector< OSMID > nodePoints;
    bool clicked;
    
    std::vector< const OSMWay* > wayPtr; 
};

struct subwayData {
    std::string name;
    LatLon point;
    bool clicked;
    OSMID id;
    subwayRouteData* srd;
    
    const OSMNode* nodePtr; 
};

class infoStrucs {
public:
    std::map<OSMID, const OSMWay*> WayMap;

    std::vector<subwayData> SubwayInfo;
    
    std::vector<subwayRouteData> SubwayRouteInfo;

    std::vector<intersectionData> IntersectionInfo;

    std::vector<streetSegData> StreetSegInfo;

    std::vector<featureData> FeatureInfo;

    std::vector< std::vector<ezgl::point2d> > FeaturePointVec;

    std::vector<POIData> POIInfo;
    
    std::vector<unsigned> lastIntersection, lastPOI, lastSeg, lastSubway, lastFeature;
    
    std::string textInput1, textInput2;
    
    std::string corInput1, corInput2;
    
    bool showSubway;
};

