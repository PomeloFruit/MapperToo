#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"

#include "OSMID.h"

class populateData{
public:
    void initialize(infoStrucs &info, mapBoundary &xy);
    void populateOSMWayInfo(infoStrucs &info);
    void populateOSMSubwayInfo(infoStrucs &info);
    int getRoadType(const OSMWay* wayPtr);
    bool isFeatureOpen(LatLon pt1, LatLon pt2);
    std::string getOSMSubwayName(const OSMNode* currentPtr);
    void populateStreetSegInfo(infoStrucs &info);
    void populateIntersectionInfo(infoStrucs &info);
    void populateFeatureInfo(infoStrucs &info, mapBoundary &xy);
    void populatePOIInfo(infoStrucs &info);
};

