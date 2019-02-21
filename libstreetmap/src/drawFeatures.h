#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "ezgl/graphics.hpp"

class featureDrawing {
public:
    void setFeatureColour(int type, ezgl::renderer &g);
    
    int classifyPOI (std::string type);
    
    void setPOIColour (int type, ezgl::renderer &g); 

    void drawFeatures(int numFeatures, infoStrucs &info, ezgl::renderer &g);

    void drawPOI(int numPOI, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea);
    
    void drawClickedPOI(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea);
    
<<<<<<< HEAD
    void drawOnePOI(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    void drawSubways(bool draw, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    void drawOneSubway(unsigned i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
=======
    void drawOnePOI(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea);
>>>>>>> Created functions to change the size of the POI's and the roads
};