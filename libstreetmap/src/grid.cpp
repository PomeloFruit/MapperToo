#include "grid.h"
#include "latLonToXY.h"

//void streetGrid::populateGrid(mapBoundary coordinates){ 
//    dLat = (coordinates.maxLat-coordinates.minLat)/100.0; 
//    dLon = (coordinates.maxLon-coordinates.minLon)/100.0;
//    
//    int xIndex, yIndex; 
//    
//    double latPOI, lonPOI, latInt, lonInt; 
//    
//    for(int i = 0; i < getNumPointsOfInterest(); i++){
//        latPOI = getPointOfInterestPosition(i).lat();
//        lonPOI = getPointOfInterestPosition(i).lon(); 
//        
//        xIndex = int((lonPOI-coordinates.minLon)/dLon); 
//        yIndex = int((latPOI-coordinates.minLat)/dLat);
//        
//    }
//    
//    
//    for(int i = 0; i < getNumPointsOfInterest(); i++){
//        latPOI = getPointOfInterestPosition(i).lat();
//        
//        yIndex = int((latPOI-coordinates.minLat)/dLat);
//        //std::cout << "POI Index: " << index << std::endl;
//        
//        if(poiGrid.count(yIndex) == 0){
//            poiGrid.insert(std::make_pair(yIndex, std::vector<unsigned>()));
//            poiGrid[yIndex].clear(); 
//        }
//        poiGrid[yIndex].push_back(i); 
//    }
//    
//    for(int i = 0; i < getNumIntersections(); i++){
//        latInt = getIntersectionPosition(i).lat();
//        
//        yIndex = int((latInt-coordinates.minLat)/dLat);
//        //std::cout << "Intersection Index: " << index << std::endl;
//        
//        if(intGrid.count(yIndex) == 0){
//            intGrid.insert(std::make_pair(yIndex, std::vector<unsigned>()));
//            intGrid[yIndex].clear();
//        }
//        intGrid[yIndex].push_back(i); 
//    }
//}
//
//unsigned streetGrid::findNearestPOI(LatLon position, mapBoundary coordinates){
//    double min = EARTH_RADIUS_IN_METERS;
//    int nearestPOIIndex = 0; 
//    
//    int index = int((position.lat()-coordinates.minLat)/dLat);
//    std::vector<unsigned> gridBlock = poiGrid[index]; 
//
//    for(unsigned i = 0; i < gridBlock.size(); i++){
//        double temp = find_distance_between_two_points(position, getPointOfInterestPosition(gridBlock[i]));
//        if(temp <= min){
//            min = temp;
//            nearestPOIIndex = gridBlock[i];
//        }
//    }
//    return unsigned(nearestPOIIndex); 
//}
//
//unsigned streetGrid::findNearestInt(LatLon position, mapBoundary coordinates){
//    double min = EARTH_RADIUS_IN_METERS;                                                                           
//    int nearestIntIndex = 0;   
//
//    int index = int((position.lat()-coordinates.minLat)/dLat);
//    std::vector<unsigned> gridBlock = intGrid[index];
//    
//    for(unsigned i = 0; i < gridBlock.size(); i++){
//        double temp = find_distance_between_two_points(position, getIntersectionPosition(gridBlock[i]));
//        if(temp <= min){
//            min = temp;
//            nearestIntIndex = gridBlock[i]; 
//        }
//    }
//    return unsigned(nearestIntIndex);   
//
//}

