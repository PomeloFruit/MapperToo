#pragma once

#include <string>
#include "globals.h"
#include "latLonToXY.h"

class clickActions {
public:
    std::string clickedOnIntersection(double x, double y, mapBoundary &xy, infoStrucs &info);
    std::string clickedOnPOI(double x, double y, mapBoundary &xy, infoStrucs &info);
};

