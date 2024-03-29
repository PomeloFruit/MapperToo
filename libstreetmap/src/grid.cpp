#include "grid.h"
#include "latLonToXY.h"
#include "drawFeatures.h"

#include <algorithm>


/* populateGrid function
 * - populates the poiGrid and the intGrid with POIs and Intersections into a 3D vector
 * - 1st Dimension - xIndex, 2nd Dimension - yIndex, 3rd Dimension - bin containing all relevant IDs  
 * 
 * @param void
 * 
 * @return void
 */

void streetGrid::populateGrid(){
    coord.initialize(); 
    
    //split the map into a 100 by 100 grid
    dLat = (coord.maxLat - coord.minLat)/100.0; 
    dLon = (coord.maxLon - coord.minLon)/100.0; 
    
    int xIndex, yIndex; 
    
    double latPOI, lonPOI, latInt, lonInt; 
    
    //creating 3D vector
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
    
    //storing POIs into the correct grid
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
    
    //storing Intersections into the correct grid
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


/* findMinimumPOI function
 * - finds the nearest POI of all the POIs in the selected grid
 * 
 * @param position <LatLon> - the current selected position
 * @param POI <int> - the current nearest POI (1D)
 * @param xIndex <int> - the x index of the grid (2D)
 * 
 * @return void
 */

void streetGrid::findMinimumPOI(LatLon position, int &POI, int xIndex, int yIndex){    
    double min = find_distance_between_two_points(position, getPointOfInterestPosition(POI)); 
    std::vector<unsigned> gridBlock = poiGrid[xIndex][yIndex]; 
    double temp; 
    if(gridBlock.size() > 0){     
        for(unsigned i = 0; i < gridBlock.size(); i++){
            temp = find_distance_between_two_points(position, getPointOfInterestPosition(gridBlock[i]));
            if(temp < min){
                min = temp; 
                POI = gridBlock[i];
                isFullPOI = true; 
            }
        }
    }
}


/* findMinimumInt function
 * - finds the nearest intersection of all intersections in the selected grid
 * 
 * @param position <LatLon> - the current selected position
 * @param Int <int> - the current nearest intersection
 * @param xIndex <int> - the x index of the grid (1D)
 * @param yIndex <int> - the y index of the grid (2D)
 * 
 * @return void
 */

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


/* findNearestPOI function
 * - loops through grid blocks until the nearest POI is found 
 * - searches in a radial manner starting with 9 blocks, then 25, 49 etc..
 * 
 * @param position <LatLon> - the current selected position
 * 
 * @return nearestPOIIndex <int> the ID of the nearest POI
 */

int streetGrid::findNearestPOI(LatLon position){
    int nearestPOIIndex = 0; 
    
    int xIndex = int((position.lon()-coord.minLon)/dLon);
    int yIndex = int((position.lat()-coord.minLat)/dLat);
    
    int tempX, tempY; 
    int radius = 1; 
    int limit = 2;
    bool extraSearch = false;
    
    /*
     * Searches radially through blocks in the grid until the nearest POI is found 
     * each iteration of the do-while loop increases the radius of search by 1
     * After the nearest POI is found, the search is performed additionally for up to 
     * 1.5 times the radius to confirm that the POI is the nearest. 
     */
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
                    if(j < 0){
                        tempY = 0; 
                    }else if (j > 100){
                        tempY = 100;
                    }else{
                        tempY = j; 
                    }
                    
                    //if the block was already searched, do not search it again
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
                } 
            }
            radius++;
        } while(!isFullPOI);
        if(!extraSearch){
            limit = int(ceil(double(radius)*1.5))-radius;
            extraSearch = true;
        }
    }
    
    isFullPOI = false; 
    check.clear(); 
    return nearestPOIIndex; 
}


/* findNearestInt function
 * - loops through grid blocks until the nearest intersection is found 
 * - searches in a radial manner starting with 9 blocks, then 25, 49 etc..
 * 
 * @param position <LatLon> - the current selected position
 * 
 * @return nearestIntIndex <int> the ID of the nearest intersection
 */

int streetGrid::findNearestInt(LatLon position){
    int nearestIntIndex = 0; 
    
    int xIndex = int((position.lon()-coord.minLon)/dLon);
    int yIndex = int((position.lat()-coord.minLat)/dLat);
    
    int tempX, tempY; 
    int radius = 1;  
    int limit = 2;
    bool extraSearch = false;
    
    /*
     * Searches radially through blocks in the grid until the nearest intersection is found 
     * each iteration of the do-while loop increases the radius of search by 1
     * After the nearest intersection is found, the search is performed additionally for up to 
     * 1.5 times the radius to confirm that the intersection is the nearest. 
     */
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
                    if(j < 0){
                        tempY = 0; 
                    }else if (j > 100){
                        tempY = 100;
                    }else{
                        tempY = j; 
                    }
                    
                    //if the block was already searched, do not search it again
                    bool found = false;
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
                }

            }
            radius++;
        } while(!isFullInt);
        if(!extraSearch){
            limit = int(ceil(double(radius)*1.5))-radius;
            extraSearch = true;
        }
    }
    
    isFullInt = false;
    check.clear(); 
    return nearestIntIndex; 
}