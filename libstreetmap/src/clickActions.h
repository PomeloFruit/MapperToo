#pragma once

#include <string>
#include "globals.h"
#include "latLonToXY.h"
#include "ezgl/application.hpp"

class clickActions {
public:
    std::string clickedOnIntersection(double x, double y, mapBoundary &xy, infoStrucs &info);
    std::string clickedOnPOI(double x, double y, mapBoundary &xy, infoStrucs &info);

    std::string searchOnMap(infoStrucs &info);
    int findMatches(infoStrucs &info, std::vector<unsigned> &streetID, std::string userInput);
    std::string getMessagesFromMatches(int match1, int match2);
    
    void highlightOnePOI(infoStrucs &info, unsigned highID);
    void highlightOneIntersection(infoStrucs &info, unsigned highID);
};

