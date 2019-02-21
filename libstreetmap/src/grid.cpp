#include "grid.h"
#include "latLonToXY.h"

#include <algorithm>

void streetGrid::populateGrid(mapBoundary coord){
    dLat = (coord.maxLat - coord.minLat)/100.0; 
    dLon = (coord.maxLon - coord.minLon)/100.0; 
    
    int xIndex, yIndex; 
    
    double latPOI, lonPOI, latInt, lonInt; 
    
    for(int i = 0; i < 101; i++){
        std::vector<std::vector<unsigned>> temp; 
        for(int j = 0; j < 101; j++){
            std::vector<unsigned> temp2;  
            temp.push_back(temp2); 
        }
        poiGrid.push_back(temp); 
        intGrid.push_back(temp);  
    }
    
    for(int i = 0; i < getNumPointsOfInterest(); i++){
        latPOI = getPointOfInterestPosition(i).lat(); 
        lonPOI = getPointOfInterestPosition(i).lon(); 
        
        xIndex = int((lonPOI - coord.minLon)/dLon); 
        yIndex = int((latPOI - coord.minLat)/dLat);
        poiGrid[xIndex][yIndex].push_back(i); 
    }
    
    for(int i = 0; i < getNumIntersections(); i++){
        latInt = getIntersectionPosition(i).lat(); 
        lonInt = getIntersectionPosition(i).lon(); 
        
        xIndex = int((lonInt - coord.minLon)/dLon); 
        yIndex = int((latInt - coord.minLat)/dLat);
        
        //std::cout << "xIndex: " << xIndex << " yIndex: " << yIndex << std::endl; 
        intGrid[xIndex][yIndex].push_back(i); 
    }
    
//    for(int i = 0; i < 100; i++){
//        for(int j = 0; j < 100; j++){
//            for(unsigned k = 0; k < poiGrid[i][j].size(); k++){
//                std::cout<<poiGrid[i][j][k]<<std::endl;
//            }
//        }
//    }
}


void streetGrid::findMinimumPOI(LatLon position, int &POI, int xIndex, int yIndex){    
    double min = find_distance_between_two_points(position, getPointOfInterestPosition(POI)); 
    std::vector<unsigned> gridBlock = poiGrid[xIndex][yIndex]; 
    
    //std::cout<<"Size: "<<poiGrid[xIndex][yIndex].size()<<std::endl;
    if(gridBlock.size() > 0){
        for(unsigned i = 0; i < gridBlock.size(); i++){
            //std::cout<<"I: "<<i<<std::endl;
            double temp = find_distance_between_two_points(position, getPointOfInterestPosition(gridBlock[i]));
            if(temp < min){
                min = temp; 
                POI = gridBlock[i];
                isFullPOI = true; 
                //std::cout<<"POI: "<<POI<<std::endl;
            }
        }
    }
    //std::cout<<"STOP"<<std::endl; 
}


void streetGrid::findMinimumInt(LatLon position, int &Int, int xIndex, int yIndex){    
    double min = find_distance_between_two_points(position, getIntersectionPosition(Int)); 
    std::vector<unsigned> gridBlock = intGrid[xIndex][yIndex]; 
    
    if(gridBlock.size() > 0){
        for(unsigned i = 0; i < gridBlock.size(); i++){
            double temp = find_distance_between_two_points(position, getIntersectionPosition(gridBlock[i]));
            if(temp < min){
                min = temp; 
                Int = gridBlock[i];
                isFullInt = true; 
            }
        }
    }
}


int streetGrid::findNearestPOI(LatLon position, mapBoundary coord){
    int nearestPOIIndex = 0; 
    
    int xIndex = int((position.lon()-coord.minLon)/dLon);
    int yIndex = int((position.lat()-coord.minLat)/dLat);
    
    int count = 0; 
    //std::cout<<"POI xIndex: "<<xIndex<<" POI yIndex: "<<yIndex<<std::endl;
    
    int tempX, tempY; 
    int radius = 1; 
    for(int t = 0; t < 2; t++){
        do{
            for(int i = xIndex-radius; i<=xIndex+radius; i++){
                if(i < 0){
                    tempX = 0;
                }else if (i > 100){
                    tempX = 100; 
                }else{
                    tempX = i;
                }
                for(int j = yIndex-radius; j <= yIndex+radius; j++){
                    //std::cout<<"I: "<<i<<" J: "<<j<<std::endl;
                    if(j < 0){
                        tempY = 0; 
                    }else if (j > 100){
                        tempY = 100;
                    }else{
                        tempY = j; 
                    }
                    count++;
                    //std::cout<<"tempX: " <<tempX<<" tempY: "<<tempY<<std::endl;
                    bool found = false;
                    for(auto it = check.begin(); it != check.end(); it++){
                        if(it->first == tempX && it->second == tempY){
                            found = true;
                        }else{
                            check.push_back(std::make_pair(tempX, tempY)); 
                        }
                    }
                    if(!found){
                        findMinimumPOI(position, nearestPOIIndex, tempX, tempY); 
                    }
                    //std::cout<<"STOP RIGHT HERE"<<std::endl;
                } 
            }
            radius++;
            //std::cout<<"radius: "<<radius<<std::endl;
        } while(!isFullPOI);
    }
    
    isFullPOI = false; 
    check.clear(); 
    //std::cout<<"square searched: "<<count<<" nearest POI: "<<nearestPOIIndex<<" Name: "<<getPointOfInterestName(nearestPOIIndex)<<std::endl;
    return nearestPOIIndex; 
}

int streetGrid::findNearestInt(LatLon position, mapBoundary coord){
    int nearestIntIndex = 0; 
    
    int xIndex = int((position.lon()-coord.minLon)/dLon);
    int yIndex = int((position.lat()-coord.minLat)/dLat);
    
    int count = 0; 
    //std::cout<<"INT xIndex: "<<xIndex<<" INT yIndex: "<<yIndex<<std::endl;
    
    int tempX, tempY; 
    int radius = 1;  
    for(int t = 0; t < 2; t++){
        do{
            for(int i = xIndex-radius; i<=xIndex+radius; i++){
                if(i < 0){
                    tempX = 0;
                }else if (i > 100){
                    tempX = 100; 
                }else{
                    tempX = i;
                }
                for(int j = yIndex-radius; j <= yIndex+radius; j++){
                    //std::cout<<"I: "<<i<<" J: "<<j<<std::endl;
                    if(j < 0){
                        tempY = 0; 
                    }else if (j > 100){
                        tempY = 100;
                    }else{
                        tempY = j; 
                    }
                    count++;
                    bool found = false;
                    //std::cout<<"tempX: " <<tempX<<" tempY: "<<tempY<<std::endl;
                    for(auto it = check.begin(); it != check.end(); it++){
                        if(it->first == tempX && it->second == tempY){
                            found = true;
                        }else{
                            check.push_back(std::make_pair(tempX, tempY)); 
                        }
                    }

                    if(!found){
                        findMinimumInt(position, nearestIntIndex, tempX, tempY); 

                    }
                    //std::cout<<"STOP RIGHT HERE"<<std::endl;
                }

            }
            radius++;
            //std::cout<<"radius: "<<radius<<std::endl;
        } while(!isFullInt);
    }
    
    isFullInt = false;
    check.clear(); 
    //std::cout<<"square searched: "<<count<<" nearest Int: "<<nearestIntIndex<<" Name: "<<getIntersectionName(nearestIntIndex)<<std::endl;
    return nearestIntIndex; 
}