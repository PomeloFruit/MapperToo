/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   drawText.h
 * Author: sartori2
 * 
 * Created on February 19, 2019, 9:28 PM
 */
#pragma once
#include "drawText.h"

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include "ezgl/graphics.hpp"
#include <math.h>

class drawText{
    public:
        mapBoundary xy;
        //Gives drawText access to the current xy object
        void initilize();
        //Determines which text needs to be drawn and where it should be drawn and if it needs to be drawn
        void createText(int numStreetSegs, int numStreets, infoStrucs &info, ezgl::renderer &g);
        
    private:
        //initial area is the area that was shown at the start of the program (or on load of a new map)
        double initialArea;
        //Determines the angle at which the text should be placed and also loads in a bool that tells if one way directions should be flipped
        std::pair<double, bool> findAngle(LatLon &initialPosition, LatLon &finalPosition);
        //For when the street contains curvepoints
        //This function determines on which segment of the street segment to draw the name
        int indexOfLargestGoodCurvepoint(int streetSegment, ezgl::rectangle& curBounds, infoStrucs &info);
        //Determines if a given point is in bounds
        bool inBounds(ezgl::rectangle& curBounds, LatLon &position);
        //Determines space and maxCount
        void spaceMaxCountDeterminer(int& space, int& maxCount, bool drawPrimary, bool drawSecondary, bool drawResidential);
        //Determines the new initial and final positions
        std::pair<LatLon, LatLon> positionDeterminer(infoStrucs &info, int index, ezgl::rectangle currentRectangle);
        
};
//ok so I do angle then I set that and draw the text
//and I gotta grab all the stret ids
//based on street size & area shown I'll show the street name every xyz streetsegs
//how do I get things to unshow though
//maybe I make text in "inbetween" segs smaller or soemthing idk
//for now I'm just gonna go ahead and draw them all