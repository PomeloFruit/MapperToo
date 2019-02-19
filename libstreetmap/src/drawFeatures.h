#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "ezgl/graphics.hpp"

class featureDrawing {
public:
    void setFeatureColour(int type, ezgl::renderer &g);

    void drawFeatures(int numFeatures, infoStrucs info, ezgl::renderer &g);

    void drawPOI(int numPOI, mapBoundary xy, ezgl::renderer &g);
};