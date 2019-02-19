#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include <math.h>
#include "latLonToXY.h"

void mapBoundary::initialize(){
    maxLat = getMaxLat();
    minLat = getMinLat();
    maxLon = getMaxLon();
    minLon = getMinLon(); 
    averageLat = getAverageLat();
}

double mapBoundary::getMaxLat(){
    double max = getIntersectionPosition(0).lat(); 
    int numOfIntersections = getNumIntersections();
    
    for(int i = 0; i < numOfIntersections; i++){
        if(getIntersectionPosition(i).lat() > max){
            max = getIntersectionPosition(i).lat(); 
        }
    }
    
    return max;
}

double mapBoundary::getMinLat(){
    double min = getIntersectionPosition(0).lat(); 
    int numOfIntersections = getNumIntersections();
    
    for(int i = 0; i < numOfIntersections; i++){
        if(getIntersectionPosition(i).lat() < min){
            min = getIntersectionPosition(i).lat(); 
        }
    }
    
    return min; 
}

double mapBoundary::getMaxLon(){
    double max = getIntersectionPosition(0).lon(); 
    int numOfIntersections = getNumIntersections();
    
    for(int i = 0; i < numOfIntersections; i++){
        if(getIntersectionPosition(i).lon() > max){
            max = getIntersectionPosition(i).lon(); 
        }
    }
    
    return max; 
}

double mapBoundary::getMinLon(){
    double min = getIntersectionPosition(0).lon(); 
    int numOfIntersections = getNumIntersections();
    
    for(int i = 0; i < numOfIntersections; i++){
        if(getIntersectionPosition(i).lon() < min){
            min = getIntersectionPosition(i).lon(); 
        }
    }
    
    return min; 
}

double mapBoundary::getAverageLat(){
    double averageLatInRad = DEG_TO_RAD*(maxLat+minLat)/2;
    
    return averageLatInRad; 
}


float mapBoundary::xFromLon(float lon){
    float projectionFactor,x;
    
    projectionFactor = cos(averageLat);
    x = lon*projectionFactor;
    
    return x;
}


float mapBoundary::yFromLat(float lat){
    return lat;
}