#include "grid.h"
#include "latLonToXY.h"
#include "drawFeatures.h"

#include <algorithm>

void streetGrid::populateGrid(){
    coord.initialize(); 
//    pop.initialize(info, coord); 
    
    dLat = (coord.maxLat - coord.minLat)/100.0; 
    dLon = (coord.maxLon - coord.minLon)/100.0; 
    
    int xIndex, yIndex; 
    
    double latPOI, lonPOI, latInt, lonInt; 

    for(int i = 0; i < 101; i++){
        std::vector<std::vector<unsigned>> temp1; 
        std::vector<std::vector<unsigned>> temp2; 
        for(int j = 0; j < 101; j++){
            temp1.push_back(std::vector<unsigned>());
            temp2.push_back(std::vector<unsigned>()); 
        }
        poiGrid.push_back(temp1);
        intGrid.push_back(temp2);
        temp1.clear();
        temp2.clear(); 
    }
    
    for(int i = 0; i < getNumPointsOfInterest(); i++){
        latPOI = getPointOfInterestPosition(i).lat(); 
        lonPOI = getPointOfInterestPosition(i).lon(); 
        
        xIndex = int((lonPOI - coord.minLon)/dLon); 
        yIndex = int((latPOI - coord.minLat)/dLat);
        
        if(xIndex < 0){
            xIndex = 0;
        } else if (xIndex > 100){
            xIndex = 100; 
        }
        
        if(yIndex < 0){
            yIndex = 0;
        } else if (yIndex > 100){
            yIndex = 100; 
        }
        poiGrid[xIndex][yIndex].push_back(i); 
    }
    
    for(int i = 0; i < getNumIntersections(); i++){
        latInt = getIntersectionPosition(i).lat(); 
        lonInt = getIntersectionPosition(i).lon(); 
        
        xIndex = int((lonInt - coord.minLon)/dLon); 
        yIndex = int((latInt - coord.minLat)/dLat);
        
        if(xIndex < 0){
            xIndex = 0;
        } else if (xIndex > 100){
            xIndex = 100; 
        }
        
        if(yIndex < 0){
            yIndex = 0;
        } else if (yIndex > 100){
            yIndex = 100; 
        }
        
        intGrid[xIndex][yIndex].push_back(i); 
    }
    
}


//void streetGrid::clearGrid(){
//    pop.clear(info);
//    poiGrid.clear();
//    intGrid.clear(); 
//}

void streetGrid::findMinimumPOI(LatLon position, int &POI, int xIndex, int yIndex){    
    double min = find_distance_between_two_points(position, getPointOfInterestPosition(POI)); 
    std::vector<unsigned> gridBlock = poiGrid[xIndex][yIndex]; 
//    int poiType;
    double temp; 
    //std::cout<<"Size: "<<poiGrid[xIndex][yIndex].size()<<std::endl;
    if(gridBlock.size() > 0){     
        for(unsigned i = 0; i < gridBlock.size(); i++){
            //std::cout<<"I: "<<i<<std::endl;
//            poiType = ft.classifyPOI(getPointOfInterestType(gridBlock[i]));
//            
//            if(poiType == 1 && info.poiButtonStatus[0] == 1){
//                temp = find_distance_between_two_points(position, getPointOfInterestPosition(gridBlock[i]));
//            }else if(poiType == 2 && info.poiButtonStatus[1] == 1){
//                temp = find_distance_between_two_points(position, getPointOfInterestPosition(gridBlock[i]));
//            }else if(poiType == 3 && info.poiButtonStatus[2] == 1){
//                temp = find_distance_between_two_points(position, getPointOfInterestPosition(gridBlock[i]));
//            }
            temp = find_distance_between_two_points(position, getPointOfInterestPosition(gridBlock[i]));
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


int streetGrid::findNearestPOI(LatLon position){
    int nearestPOIIndex = 0; 
    
    int xIndex = int((position.lon()-coord.minLon)/dLon);
    int yIndex = int((position.lat()-coord.minLat)/dLat);
    
    //int count = 0; 
    //std::cout<<"POI xIndex: "<<xIndex<<" POI yIndex: "<<yIndex<<std::endl;
    
    int tempX, tempY; 
    int radius = 1; 
    int limit = 2;
    bool extraSearch = false; 
    for(int t = 0; t < limit; t++){
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
                        //count++;
                        findMinimumPOI(position, nearestPOIIndex, tempX, tempY); 
                    }
                    //std::cout<<"STOP RIGHT HERE"<<std::endl;
                } 
            }
            radius++;
            //std::cout<<"radius: "<<radius<<std::endl;
        } while(!isFullPOI);
        if(!extraSearch){
            limit = int(ceil(double(radius)*1.5))-radius;
            extraSearch = true;
        }
    }
    
    isFullPOI = false; 
    check.clear(); 
    //std::cout<<"square searched: "<<count<<" nearest POI: "<<nearestPOIIndex<<" Name: "<<getPointOfInterestName(nearestPOIIndex)<<std::endl;
    return nearestPOIIndex; 
}

int streetGrid::findNearestInt(LatLon position){
    int nearestIntIndex = 0; 
    
    int xIndex = int((position.lon()-coord.minLon)/dLon);
    int yIndex = int((position.lat()-coord.minLat)/dLat);
    
    //int count = 0; 
    //std::cout<<"INT xIndex: "<<xIndex<<" INT yIndex: "<<yIndex<<std::endl;
    
    int tempX, tempY; 
    int radius = 1;  
    int limit = 2;
    bool extraSearch = false; 
    for(int t = 0; t < limit; t++){
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
                        //count++;
                        findMinimumInt(position, nearestIntIndex, tempX, tempY); 

                    }
                    //std::cout<<"STOP RIGHT HERE"<<std::endl;
                }

            }
            radius++;
            //std::cout<<"radius: "<<radius<<std::endl;
        } while(!isFullInt);
        if(!extraSearch){
            limit = int(ceil(double(radius)*1.5))-radius;
            extraSearch = true;
        }
    }
    
    isFullInt = false;
    check.clear(); 
    //std::cout<<"square searched: "<<count<<" nearest Int: "<<nearestIntIndex<<" Name: "<<getIntersectionName(nearestIntIndex)<<std::endl;
    return nearestIntIndex; 
}