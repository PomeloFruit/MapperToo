#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include <string>

#include "OSMID.h"

class populateData{
public:
    // calls  populate functions for critical map element structures
    void initialize(infoStrucs &info, mapBoundary &xy);
    
    // calls  populate function for non-critical map elements
    void loadAfterDraw(infoStrucs &info);
    
    // populates the waymap in info
    void populateOSMWayInfo(infoStrucs &info);
    
    void populateStreetSegInfo(infoStrucs &info);
    
    void populateIntersectionInfo(infoStrucs &info);
    
    void populateFeatureInfo(infoStrucs &info, mapBoundary &xy);
    
    void populatePOIInfo(infoStrucs &info);
    
    void populateOSMSubwayInfo(infoStrucs &info);
    
    int checkIfSubwayRoute(const OSMRelation* relPtr);
    
    int getRoadType(const OSMWay* wayPtr); 
    
    bool isFeatureOpen(LatLon pt1, LatLon pt2);
    
    bool checkIfSubway(const OSMNode* nodePtr);

    bool checkOSMRelationTags(const OSMRelation* relPtr, std::string k, std::string v);
    
    void getOSMSubwayRelations(infoStrucs &info);
    
    std::vector< unsigned > checkIfSubwayRouteNode(const OSMNode*, infoStrucs &info);
    
    std::string getOSMNodeName(const OSMNode* nodePtr);
    
    std::string getOSMRelationInfo(const OSMRelation* relPtr, std::string k);
};

