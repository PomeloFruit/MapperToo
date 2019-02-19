
#pragma once

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "OSMID.h"
#include "ezgl/point.hpp"

#include <string>
#include <iostream>
#include <map>

#define ROADWIDTH 3
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

class infoStrucs {
public:
    std::map<OSMID, const OSMWay*> WayMap;

    std::vector<subwayData> SubwayInfo;

    std::vector<intersectionData> IntersectionInfo;

    std::vector<streetSegData> StreetSegInfo;

    std::vector<featureData> FeatureInfo;

    std::vector<std::vector<ezgl::point2d>> FeaturePointVec;

    std::vector<POIData> POIInfo;
};

