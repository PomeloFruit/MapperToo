#pragma once

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include "ezgl/graphics.hpp"

class roadDrawing {

public: 
    //Draws a straight line on the map according to the specifications set out in setRoadColourSize. Called by drawStreetRoads
    void setRoadColourSize(int type, bool highlight, ezgl::renderer &g, double startArea, double currentArea);
    //Determines if the intersection should be drawn as normal or loaded in with a png
    //Then draws the intersection as specified
    void drawStreetRoads(int numSegs, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double startArea, double currentArea, ezgl::rectangle currentRectangle);
    //Determines if a given point is in bounds
    bool inBounds(mapBoundary &xy, ezgl::rectangle& curBounds, LatLon& position);
    //Draws a straight line on the map according to the specifications set out in setRoadColourSize. Called by drawStreetRoads
    void drawStraightStreet(LatLon &pt1, LatLon &pt2, mapBoundary &xy, ezgl::renderer &g);
    //Determines if an intersections is to be drawn and calls a function to draw it if it is
    void drawIntersections(int numInter, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    //Draws special intersections
    void drawSpecialIntersections(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
    //Determines if the intersection should be drawn as normal or loaded in with a png
    //Then draws the intersection as specified
    void drawOneIntersection(int i, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g);
};

