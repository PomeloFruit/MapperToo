#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "ezgl/graphics.hpp"

class featureDrawing {
public:
    // converts POI type from words to number code
    int classifyPOI (std::string type);
    
    // sets the feature colours based on feature type 
    void setFeatureColour(int type, ezgl::renderer &g, bool special);

    // draws all features
    void drawFeatures(int numFeatures, infoStrucs &info, ezgl::renderer &g, double currentArea, double startArea);

    // draws all POI
    void drawPOI(int numPOI, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double screenRatio);
    
    // draws one poi at index i
    void drawOnePOI(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    // draws the clicked POI
    void drawClickedPOI(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    // draws all subways if draw is true
    void drawSubways(int draw, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    // draws one subway station at index i
    void drawOneSubway(unsigned i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    // draws the subway route if draw is true
    void drawSubwayRoute(int draw, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    // draws one subway route at index r with route of type t
    void drawOneSubwayRoute(unsigned r, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, int t);
    
    // draws a section of subway, highlighted if high is true, train/subway based on t
    void drawStraightSubwaySection(LatLon &pt1, LatLon &pt2, mapBoundary &xy, ezgl::renderer &g, bool high, int t);
    
    // draws text above POI
    void drawTextOnPOI(ezgl::renderer &g, infoStrucs &info);\

private:
    //POI counter limits 
    int foodDrinkLimit;
    int touristLimit; 
    int shopsLimit;

    int foodDrinkPOICounter;
    int touristPOICounter;
    int shopsPOICounter; 

    // stores POIs that have been drawn already
    std::vector<std::pair<ezgl::point2d, int>> drawnPOIs;
};