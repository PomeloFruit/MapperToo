#include "latLonToXY.h"
#include "LatLon.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include <math.h>

void mapBoundary::initialize(){
    numOfIntersections = getNumIntersections();
    
    maxLat = getMaxLat();
    minLat = getMinLat();
    maxLon = getMaxLon();
    minLon = getMinLon();
    averageLat = getAverageLat(); 
    
    xMax = xFromLon(maxLon);
    xMin = xFromLon(minLon);
    yMax = yFromLat(maxLat);
    yMin = yFromLat(minLat);  
}

double mapBoundary::getMaxLat(){
    double max;
    
    max = getIntersectionPosition(0).lat(); 
    
    for(int i = 0; i < numOfIntersections; i++){
        if(getIntersectionPosition(i).lat() > max){
            max = getIntersectionPosition(i).lat(); 
        }
    }
    
    return max;
}

double mapBoundary::getMinLat(){
    double min;
    
    min = getIntersectionPosition(0).lat(); 

    for(int i = 0; i < numOfIntersections; i++){
        if(getIntersectionPosition(i).lat() < min){
            min = getIntersectionPosition(i).lat(); 
        }
    }
    
    return min; 
}

double mapBoundary::getMaxLon(){
    double max;
    
    max = getIntersectionPosition(0).lon(); 
    
    for(int i = 0; i < numOfIntersections; i++){
        if(getIntersectionPosition(i).lon() > max){
            max = getIntersectionPosition(i).lon(); 
        }
    }
    
    return max; 
}

double mapBoundary::getMinLon(){
    double min;
    
    min = getIntersectionPosition(0).lon(); 

    for(int i = 0; i < numOfIntersections; i++){
        if(getIntersectionPosition(i).lon() < min){
            min = getIntersectionPosition(i).lon(); 
        }
    }
    
    return min; 
}

double mapBoundary::getAverageLat(){
    return DEG_TO_RAD*(maxLat+minLat)/2;
}

// projection factor is cos(averageLat))
float mapBoundary::xFromLon(float lon){
    return lon * cos(averageLat);
}


float mapBoundary::yFromLat(float lat){
    return lat;
}

LatLon mapBoundary::LatLonFromXY(double x, double y){
    float lat, lon;
    
    lat = y;
    lon = x / cos(averageLat);
    
    LatLon convCoord(lat,lon);
    
    return convCoord;
}

