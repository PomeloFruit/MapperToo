// prevent duplicate includes
#pragma once

#include <string>
#include "LatLon.h"
#include "ezgl/application.hpp"
#include "latLonToXY.h"
#include "globals.h"

// class contains most of the functions needed to act on clicks on the map
// and searches in the text fields
class clickActions {
public:
    // finds the clicked intersection and highlights it,
    // returns message with information on intersection if any
    std::string clickedOnIntersection(double x, double y, mapBoundary &xy, infoStrucs &info);
    
    // finds the clicked POI and highlights it,
    // returns message with information on POI if any
    std::string clickedOnPOI(double x, double y, mapBoundary &xy, infoStrucs &info);
    
    // finds the clicked subway/train stop and highlights it and route (if found)
    // returns message with information on POI if any
    std::string clickedOnSubway(double x, double y, mapBoundary &xy, infoStrucs &info);
    
    // finds the intersections from all streets in street1 + streeet2 vectors
    std::vector<unsigned> findIntersectionsFromStreets(std::vector<unsigned> street1ID, std::vector<unsigned> street2ID);
    
    // find item being searched, can be street/intersection/POI/Feature/subway
    // return message what was found or error if not found
    std::string searchOnMap(infoStrucs &info);
    
    // finds a path between two intersections and returns it on the screen
    void searchForDirections(infoStrucs &info);
    
    // find the nearest subway station to pt
    unsigned findNearestSubway(infoStrucs &info, LatLon pt);
    
    // finds the type of match from searching in text field
    // -1 for no input, 0 for no match, >0 for number of matches, -2 for POI, -3 for Subway, -4 for feature 
    int findMatches(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info);
    
    // finds all the POI with the userInput name, and stores it in streetID
    void findPOIByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info);
    
    /// finds all the subways with the userInput name, and stores it in streetID
    void findSubwaysByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info);
    
    // finds all the features with the userInput name, and stores it in streetID
    void findFeaturesByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info);
    
    // converts the match types from above into error messages (if needed))
    std::string getMessagesFromMatches(int match1, int match2);
    
    // raises flag within highID street to make it highlighted
    void highlightStreet(infoStrucs &info, unsigned highID);
    void highlightStreet(infoStrucs &info, std::vector<unsigned> highID);

    // raises flag within highID POI to make it highlighted
    void highlightPOI(infoStrucs &info, unsigned highID);
    void highlightPOI(infoStrucs &info, std::vector<unsigned> &highID);
    
    // raises flag within highID intersection to make it highlighted
    void highlightIntersection(infoStrucs &info, unsigned highID);
    void highlightIntersection(infoStrucs &info, std::vector<unsigned> &highID);
    
    // raises flag within highID subway station (and route if possible) to make it highlighted
    void highlightSubway(infoStrucs &info, unsigned highID);
    void highlightSubway(infoStrucs &info, std::vector<unsigned> &highID);
    
    // raises flag within highID feature to make it highlighted
    void highlightFeature(infoStrucs &info, unsigned highID);
    void highlightFeature(infoStrucs &info, std::vector<unsigned> &highID);
    
    // clears all flags previously raised, removing their highlighting
    void clearPreviousHighlights(infoStrucs &info);
    
    // returns a pair of LatLon points representing the top left and bottom right of the street
    std::pair< LatLon, LatLon > getCornerStreetSeg(infoStrucs &info); 
};

