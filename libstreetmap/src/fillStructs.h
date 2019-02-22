#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include <string>

#include "OSMID.h"

class populateData{
public:
    
    void initialize(infoStrucs &info, mapBoundary &xy);
    
    void loadAfterDraw(infoStrucs &info);
    
    void populateOSMWayInfo(infoStrucs &info);
    
    int getRoadType(const OSMWay* wayPtr);
    
    bool isFeatureOpen(LatLon pt1, LatLon pt2);
    
    void populateStreetSegInfo(infoStrucs &info);
    
    void populateIntersectionInfo(infoStrucs &info);
    
    void populateFeatureInfo(infoStrucs &info, mapBoundary &xy);
    
    void populatePOIInfo(infoStrucs &info);
    
    void populateOSMSubwayInfo(infoStrucs &info);
    
    bool checkIfSubway(const OSMNode* nodePtr);
    
    std::string getOSMNodeName(const OSMNode* nodePtr);
    
    void getOSMSubwayRelations(infoStrucs &info);
    
    std::vector< unsigned > checkIfSubwayRouteNode(const OSMNode*, infoStrucs &info);
    
    int checkIfSubwayRoute(const OSMRelation* relPtr);
    
    bool checkOSMRelationTags(const OSMRelation* relPtr, std::string k, std::string v);
    
    std::string getOSMRelationInfo(const OSMRelation* relPtr, std::string k);

};

