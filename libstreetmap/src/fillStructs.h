#pragma once

#include "globals.h"
#include "latLonToXY.h"

#include "OSMID.h"


class populateData{
public:
    void initialize(infoStrucs info, mapBoundary xy);
    void populateOSMWayInfo(infoStrucs info);
    void populateOSMSubwayInfo(infoStrucs info);
    int getRoadType(const OSMWay* wayPtr);
    std::string getOSMSubwayName(const OSMNode* currentPtr);
    void populateStreetSegInfo(infoStrucs info);
    void populateIntersectionInfo(infoStrucs info);
    void populateFeatureInfo(infoStrucs info, mapBoundary xy);
    void populatePOIInfo(infoStrucs info);
};

