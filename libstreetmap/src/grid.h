#pragma once

#include "grid.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "globals.h"
#include "latLonToXY.h"

class streetGrid{
public:
    
    std::vector<std::vector<std::vector<unsigned> > > poiGrid;
    std::vector<std::vector<std::vector<unsigned> > > intGrid;
    
    std::vector<std::pair<int, int>> check;

    double dLat; 
    double dLon; 
    bool isFullPOI = false;
    bool isFullInt = false;
    mapBoundary coord; 
    
    void populateGrid();
    
    void findMinimumPOI(LatLon position, int &POI, int xIndex, int yIndex);
    void findMinimumInt(LatLon position, int &Int, int xIndex, int yIndex);
    int findNearestPOI(LatLon position);
    int findNearestInt(LatLon position); 
};
