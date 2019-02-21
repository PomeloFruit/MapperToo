#pragma once

#include <string>
#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include "ezgl/application.hpp"

class clickActions {
public:
    std::string clickedOnIntersection(double x, double y, mapBoundary &xy, infoStrucs &info);
    std::string clickedOnPOI(double x, double y, mapBoundary &xy, infoStrucs &info);
    std::string clickedOnSubway(double x, double y, mapBoundary &xy, infoStrucs &info);
    std::string searchOnMap(infoStrucs &info, ezgl::application *&application);
    
    unsigned findNearestSubway(infoStrucs &info, LatLon pt);
    
    int findMatches(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info);
    void findPOIByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info);
    void findSubwaysByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info);
    void findFeaturesByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info);
    
    std::string getMessagesFromMatches(int match1, int match2);
    
    void highlightStreet(infoStrucs &info, unsigned highID);
    void highlightStreet(infoStrucs &info, std::vector<unsigned> highID);

    void highlightPOI(infoStrucs &info, unsigned highID);
    void highlightPOI(infoStrucs &info, std::vector<unsigned> &highID);
    
    void highlightIntersection(infoStrucs &info, unsigned highID);
    void highlightIntersection(infoStrucs &info, std::vector<unsigned> &highID);
    
    void highlightSubway(infoStrucs &info, unsigned highID);
    void highlightSubway(infoStrucs &info, std::vector<unsigned> &highID);
    
    void highlightFeature(infoStrucs &info, unsigned highID);
    void highlightFeature(infoStrucs &info, std::vector<unsigned> &highID);
    
    void clearPreviousHighlights(infoStrucs &info);
};

