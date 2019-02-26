#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include <string>

#include "OSMID.h"

// class contains all functions needed to fill global.h info data structures
class populateData{
public:  
    // calls  populate functions for critical map element structures
    void initialize(infoStrucs &info, mapBoundary &xy);
    
    // clears data structures 
    void clear(infoStrucs &info); 
    
    // stores each street in the map along with its streetType (ie Trunk, Motorway etc..) 
    void classifyStreetType(int i, infoStrucs &info);
    
    // Holds the number of each street type in each map - 0 for Motorway, 1 for Primary ... 4 for Service, 5 for Trunk
    void streetTypeArray(infoStrucs &info);
    
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
    
    // classifies POIs based on POI type
    int classifyPOI(std::string type);
    
private:
    //vector to store all types of tourist attractions 
    std::vector<std::string> tourist {"aquarium", "artwork", "attraction", "gallery",
            "information", "museum", "theme_park", "zoo", "college", "university", 
            "brothel", "arts_centre", "casino", "cinema", "fountain", "gambling", 
            "music_venue", "nightclub", "planetarium", "theatre", "marketplace", 
            "townhall", "stadium"
    };

    //vector to store all types of food and drink locations
    std::vector<std::string> foodDrink {"alcohol", "bakery", "beverages", "coffee", 
            "confectionery", "convenience", "ice_cream","pastry", "seafood", "bar",
            "bbq", "cafe", "food_court", "pub", "restaurant"
    }; 

    //vector to store all types of shopping locations
    std::vector<std::string> shops {"department_store", "general", "kiosk", "mall", 
            "supermarket", "wholesale", "bag", "boutique", "clothes" , "jewelry", 
            "leather", "shoes", "watches", "variety", "second_hand", "beauty", 
            "cosmetics", "erotic", "hairdresser", "herbalist", "massage", 
            "medical_supply", "perfumery", "tattoo", "electrical", "florist", 
            "antiques", "candles", "interior_decoration", "computer", "robot", 
            "electronics", "mobile_phone", "radiotechnics", "fishing", "fuel", 
            "outdoor", "scuba_diving", "ski", "sports", "swimming_pool", "art", 
            "collector", "craft", "games", "model", "music", "musical_instrument", 
            "photo", "camera", "video", "video_games", "anime", "books", "gift", 
            "stationery", "ticket", "cannabis", "e-cigarette", "tobacco", "toys", 
            "travel_agency"
    };
    
    const unsigned MAXLOADNODES = 100000;
    const unsigned MAXNODES = 6000000;
};

