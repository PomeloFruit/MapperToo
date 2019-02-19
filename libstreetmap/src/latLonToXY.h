#pragma once

class mapBoundary {
public: 
    int numOfIntersections;
    
    double maxLat, minLat, maxLon, minLon;
        
    double averageLat;
    
    void initialize(); 
    
    void setAverageLat(); 
    
    double getMaxLat();
    
    double getMinLat();
    
    double getMaxLon();
    
    double getMinLon(); 
    
    double getAverageLat();
    
    float xFromLon(float lon); 
    
    float yFromLat(float lat); 
};
