#pragma once

#include "LatLon.h"

class mapBoundary {
public: 
    int numOfIntersections;
    
    double maxLat, minLat, maxLon, minLon;
        
    double averageLat;
    
    double xMax, xMin, yMax, yMin; 
    
    void initialize(); 
    
    double getMaxLat();
    
    double getMinLat();
    
    double getMaxLon();
    
    double getMinLon(); 
    
    double getAverageLat();
    
    float xFromLon(float lon); 
    
    float yFromLat(float lat);
    
    LatLon LatLonFromXY(double x, double y);
};
