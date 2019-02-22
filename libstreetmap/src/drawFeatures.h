#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "ezgl/graphics.hpp"

class featureDrawing {
public:
    int classifyPOI (std::string type);
    
    void setPOIColour (int type, ezgl::renderer &g); 
    
    void setFeatureColour(int type, ezgl::renderer &g, bool special);

    void drawFeatures(int numFeatures, infoStrucs &info, ezgl::renderer &g);

    void drawPOI(int numPOI, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea);

    void drawOnePOI(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea); /////////why are their duplicates???
    
    void drawOnePOI(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    void drawClickedPOI(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double adjustmentFactor, double currentArea);
    
    void drawSubways(int draw, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    void drawOneSubway(unsigned i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    void drawSubwayRoute(int draw, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    void drawOneSubwayRoute(unsigned r, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, int t);
    
    void drawStraightSubwaySection(LatLon &pt1, LatLon &pt2, mapBoundary &xy, ezgl::renderer &g, bool high, int t);
};