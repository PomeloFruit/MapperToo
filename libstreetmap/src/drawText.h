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

#include "drawText.h"

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include "ezgl/graphics.hpp"
#include <math.h>

class drawText{
    public:
        mapBoundary xy; 
        
        void initilize();
        void createText(int numStreetSegs, int numStreets, infoStrucs &info, ezgl::renderer &g);
    private:
        std::pair<double, bool> findAngle(LatLon &initialPosition, LatLon &finalPosition);
        int indexOfLargestGoodCurvepoint(int streetSegment, ezgl::rectangle& curBounds, infoStrucs &info);
        bool inBounds(ezgl::rectangle& curBounds, LatLon &position);
};
//ok so I do angle then I set that and draw the text
//and I gotta grab all the stret ids
//based on street size & area shown I'll show the street name every xyz streetsegs
//how do I get things to unshow though
//maybe I make text in "inbetween" segs smaller or soemthing idk
//for now I'm just gonna go ahead and draw them all