#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include "ezgl/graphics.hpp"

class roadDrawing {

public: 
<<<<<<< HEAD
    void setRoadColourSize(int type, bool highlight, ezgl::renderer &g);
=======
    void setRoadColourSize(int type, ezgl::renderer &g, mapBoundary &xy, double startArea, double currentArea);
>>>>>>> Created functions to change the size of the POI's and the roads
    
    void drawStreetRoads(int numSegs, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double startArea, double currentArea);
    
    void drawStraightStreet(LatLon &pt1, LatLon &pt2, mapBoundary &xy, ezgl::renderer &g);
    
    void drawIntersections(int numInter, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    void drawSpecialIntersections(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    
    void drawOneIntersection(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
};

