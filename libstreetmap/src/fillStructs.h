#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include <string>

#include "OSMID.h"

// class contains all functions needed to fill global.h info data structures
class populateData{
public:
    const unsigned MAXLOADNODES = 100000;
    const unsigned MAXNODES = 6000000;
    
    // calls  populate functions for critical map element structures
    void initialize(infoStrucs &info, mapBoundary &xy);
    
    void clear(infoStrucs &info); 
    
    // calls  populate function for non-critical map elements
    void loadAfterDraw(infoStrucs &info);
    
    // fills the waymap in info
    void populateOSMWayInfo(infoStrucs &info);
    
    // fills StreetSegInfo in info
    void populateStreetSegInfo(infoStrucs &info);
    
    // fills intersection info in info
    void populateIntersectionInfo(infoStrucs &info);
    
    // fills feature info in info and feature points vec with xy points
    void populateFeatureInfo(infoStrucs &info, mapBoundary &xy);
    
    // fills POI information
    void populatePOIInfo(infoStrucs &info);
    
    // fills subway information
    void populateOSMSubwayInfo(infoStrucs &info);
    
    // determines if the relation is a subway pointer
    int checkIfSubwayRoute(const OSMRelation* relPtr);
    
    // returns road type
    int getRoadType(const OSMWay* wayPtr); 
    
    // determines if feature is open
    bool isFeatureOpen(LatLon pt1, LatLon pt2);
    
    // determines if node is subway
    bool checkIfSubway(const OSMNode* nodePtr);

    // determines if k and v match relationship tags
    bool checkOSMRelationTags(const OSMRelation* relPtr, std::string k, std::string v);
    
    // fills subway route info in info
    void getOSMSubwayRelations(infoStrucs &info);
    
    // returns all routes belonging to node
    std::vector< unsigned > checkIfSubwayRouteNode(const OSMNode*, infoStrucs &info);
    
    // returns the name of the node
    std::string getOSMNodeName(const OSMNode* nodePtr);
    
    // returns the value with key k in tags of relation
    std::string getOSMRelationInfo(const OSMRelation* relPtr, std::string k);
};

