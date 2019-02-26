#pragma once

#include "grid.h"
#include "drawFeatures.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "globals.h"
#include "latLonToXY.h"
#include "fillStructs.h"

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
    featureDrawing ft;
    
    // populates the Grid structures with POIs and intersection IDs
    void populateGrid();
    
    // finds the nearest POI in the selected block
    void findMinimumPOI(LatLon position, int &POI, int xIndex, int yIndex);
    
    // finds the nearest intersection in the selected block
    void findMinimumInt(LatLon position, int &Int, int xIndex, int yIndex);
    
    // finds the nearest POI in the grid
    int findNearestPOI(LatLon position);
    
    // finds the nearest intersection in the grid
    int findNearestInt(LatLon position); 
};
