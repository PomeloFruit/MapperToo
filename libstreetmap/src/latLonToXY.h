#pragma once

class mapBoundary {
public: 
    double maxLat;
    
    double minLat;
    
    double maxLon;
    
    double minLon;
    
    double averageLat; 
    
    void initialize(); 
    
    double getMaxLat();
    
    double getMinLat();
    
    double getMaxLon();
    
    double getMinLon(); 
    
    double getAverageLat();
    
    float xFromLon(float lon); 
    
    float yFromLat(float lat); 
};
