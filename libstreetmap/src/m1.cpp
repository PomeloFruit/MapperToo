/* 
 * Copyright 2018 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include <map>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include <string>
#include <iostream>

//==============================================================================
//Global Variables

//===========================REQUIRES WORK========================================
std::map<int, std::vector<unsigned>> streetNameMap;
std::map<std::string, std::vector<unsigned>> streetNameIndexMap;

std::unordered_map<int, std::vector<unsigned>> streetMapFast; 
std::vector<std::vector<unsigned>> streetIntersections; 

std::vector<std::vector<unsigned>> streetSegIDVector;
std::vector<std::vector<std::string>> streetSegNameVector;

bool load_map(std::string path/*map_path*/) {
    bool load_successful = false; //Indicates whether the map has loaded 
                                  //successfully

    //
    //Load your map related data structures here
    //

    int segmentStreetID;
    int numOfSegs;

    load_successful = loadStreetsDatabaseBIN(path);
    if(load_successful){
        for(int i=0;i<getNumStreetSegments();i++){
            segmentStreetID = getInfoStreetSegment(i).streetID;
            
            //creating regular map
            if(streetNameMap.count(segmentStreetID)== 0){
                streetNameMap.insert(std::make_pair(segmentStreetID,std::vector<unsigned>()));
                streetNameMap[segmentStreetID].clear();
            }
            streetNameMap[segmentStreetID].push_back(unsigned(i));
            
            //creating unordered map 
            if(streetMapFast.count(segmentStreetID) == 0){
                streetMapFast.insert(std::make_pair(segmentStreetID, std::vector<unsigned>()));
                streetMapFast[segmentStreetID].clear(); 
            }
            streetMapFast[segmentStreetID].push_back(unsigned(i));

        }
        
        //creating vector of intersection vectors
        std::vector<unsigned>::iterator intersectionIt;
        std::vector<unsigned> toBeInserted;
        unsigned intersectionID1,intersectionID2;
        for(int i = 0; i < getNumStreets(); i++){
            std::vector<unsigned> allSegments = find_street_street_segments(unsigned(i)); 
            toBeInserted.clear();
            int numSegments = allSegments.size(); 
            for(int j = 0; j < numSegments; j++){
                intersectionID1 = getInfoStreetSegment(allSegments[j]).from;
                intersectionID2 = getInfoStreetSegment(allSegments[j]).to; 
                
                intersectionIt = std::find(toBeInserted.begin(), toBeInserted.end(), intersectionID1); 
                if(intersectionIt == toBeInserted.end()){
                    toBeInserted.push_back(intersectionID1); 
                }
                
                intersectionIt = std::find(toBeInserted.begin(), toBeInserted.end(), intersectionID2); 
                if(intersectionIt == toBeInserted.end()){
                    toBeInserted.push_back(intersectionID2); 
                }
                
            }
            streetIntersections.push_back(toBeInserted); 
        }
        
        
         
        //changed this probably want to revert 
        std::string currentStreetName;
   
        for(int i=0;i<getNumStreets();i++){
            currentStreetName = getStreetName(i);
            std::transform(currentStreetName.begin(), currentStreetName.end(), currentStreetName.begin(), ::tolower);
            for(int j = 0; j < int(currentStreetName.length()); j++){
                std::string temp = currentStreetName.substr(0, j); 
                if(streetNameIndexMap.count(temp) == 0){
                    streetNameIndexMap.insert(std::make_pair(temp, std::vector<unsigned>()));
                    streetNameIndexMap[temp].clear();
                }
                streetNameIndexMap[temp].push_back(unsigned(i)); 
            }
        }
        
        //if(streetNameMap.size()!=(getNumStreets()-1)){
        //	load_successful = false;
        //} 
        
        
        std::vector<unsigned> intersectionIds;
        std::vector<std::string> segNames;
        for(int i=0;i<getNumIntersections();i++){
            numOfSegs=getIntersectionStreetSegmentCount(i);
            segNames.clear();
            intersectionIds.clear();

            for(int j=0;j<numOfSegs;j++){
                segNames.push_back(getStreetName((getInfoStreetSegment(getIntersectionStreetSegment(j, i))).streetID));
                intersectionIds.push_back(getIntersectionStreetSegment(j, i));           
                
            }
            streetSegNameVector.push_back(segNames);
            streetSegIDVector.push_back(intersectionIds);
        }

    }

    return load_successful;
}


void close_map() {
    //Clean-up your map related data structures here
    streetNameMap.clear();
    streetNameIndexMap.clear();
    streetSegIDVector.clear();
    streetSegNameVector.clear();
    streetMapFast.clear(); 
    closeStreetDatabase();
}



std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id){
    return streetSegIDVector[intersection_id];        
}


std::vector<std::string> find_intersection_street_names(unsigned intersection_id){
    return streetSegNameVector[intersection_id];
}

bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2){
    std::vector<unsigned> segsInt1=find_intersection_street_segments(intersection_id1);
    if(intersection_id1==intersection_id2){
        return true;
    }
    for(unsigned i=0;i<segsInt1.size();i++){
        if((unsigned(getInfoStreetSegment(segsInt1[i]).to)==intersection_id2)){
            return true;
        }
        else if((unsigned(getInfoStreetSegment(segsInt1[i]).to)==intersection_id1)&&(unsigned(getInfoStreetSegment(segsInt1[i]).from)==intersection_id2)){
            if(!getInfoStreetSegment(segsInt1[i]).oneWay){
                return true;
            }
        }
    }
    return false;
}


std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id){
    std::vector<unsigned> segsOrigin=find_intersection_street_segments(intersection_id);
    std::vector<unsigned> connectedIntersections;
    bool insert;
    bool invalidTo;
    bool invalidFrom;
    for(unsigned i=0;i<segsOrigin.size();i++){
        insert=true;
        invalidTo=false;
        invalidFrom=false;
        for(unsigned c=0;c<connectedIntersections.size();c++){
            if((connectedIntersections[c]==unsigned(getInfoStreetSegment(segsOrigin[i]).to)||(unsigned(getInfoStreetSegment(segsOrigin[i]).to)==intersection_id))){
                invalidTo=true;
            }
            if((connectedIntersections[c]==unsigned(getInfoStreetSegment(segsOrigin[i]).from))||(unsigned(getInfoStreetSegment(segsOrigin[i]).from)==intersection_id)){
                invalidFrom=true;
            }
        }
        //insert=true;
        if((getInfoStreetSegment(segsOrigin[i]).oneWay)&&(unsigned(getInfoStreetSegment(segsOrigin[i]).to)==intersection_id)){
            insert=false;
        }
        if(insert){
            if((unsigned(getInfoStreetSegment(segsOrigin[i]).to)!=intersection_id)&&!invalidTo){
                connectedIntersections.push_back(getInfoStreetSegment(segsOrigin[i]).to);
            }
            else if(!invalidFrom){
                connectedIntersections.push_back(getInfoStreetSegment(segsOrigin[i]).from);
            }
        }
    }
    return connectedIntersections;
}

std::vector<unsigned> find_street_street_segments(unsigned street_id){
    return streetMapFast[street_id];  
}

std::vector<unsigned> find_all_street_intersections(unsigned street_id){
    return streetIntersections[street_id]; 
}

std::vector<unsigned> find_intersection_ids_from_street_ids(unsigned street_id1,unsigned street_id2){
    std::vector<unsigned> street1IntersectionVector, street2IntersectionVector, intersectingIDs;
    unsigned currentIntersection;
    int numIntStreet1, numIntStreet2;

    street1IntersectionVector = find_all_street_intersections(street_id1);
    street2IntersectionVector = find_all_street_intersections(street_id2);
    numIntStreet1 = street1IntersectionVector.size();
    numIntStreet2 = street2IntersectionVector.size();

    for(int id1=0;id1<numIntStreet1;id1++){
	for(int id2=0;id2<numIntStreet2;id2++){
	    currentIntersection = street1IntersectionVector[id1];
	    if(currentIntersection  == street2IntersectionVector[id2]){
		intersectingIDs.push_back(currentIntersection);
	    }
	}
    }
    return intersectingIDs;
}

//Returns the distance between two coordinates in meters
double find_distance_between_two_points(LatLon point1, LatLon point2){
    double pt1LatInRad = point1.lat()*DEG_TO_RAD;
    double pt2LatInRad = point2.lat()*DEG_TO_RAD;
    
    double pt1x,pt1y,pt2x,pt2y;
    double dxSquared,dySquared,hypotenuse,distance;
    double averageLatInRad = (pt1LatInRad+pt2LatInRad)/2.0;
    double projectionFactor = cos(averageLatInRad);
    
    pt1x = point1.lon()*projectionFactor;
    pt2x = point2.lon()*projectionFactor;
    pt1y = point1.lat();
    pt2y = point2.lat();
    
    dxSquared = pow(pt2x-pt1x,2);
    dySquared = pow(pt2y-pt1y,2);
    hypotenuse = sqrt(dxSquared+dySquared);
    
    distance = EARTH_RADIUS_IN_METERS*hypotenuse*DEG_TO_RAD;
    return distance;
}

//=====================precalculate this in load map ===========================
//Returns the length of the given street segment in meters
double find_street_segment_length(unsigned street_segment_id){
    double totalLength = 0;
    
    int numSegments = getInfoStreetSegment(street_segment_id).curvePointCount;
    LatLon point1, point2;
    
    point1 = getIntersectionPosition(getInfoStreetSegment(street_segment_id).from);
    
    for(int i=0;i<numSegments;i++){ 
        point2 = getStreetSegmentCurvePoint(i,street_segment_id);
        totalLength = totalLength + find_distance_between_two_points(point1,point2);
        point1 = point2;
    }
    
    point2 = getIntersectionPosition(getInfoStreetSegment(street_segment_id).to);
    
    totalLength = totalLength + find_distance_between_two_points(point1,point2);
    return totalLength;
}

//=====================precalculate this in load map ===========================
//Returns the length of the specified street in meters
double find_street_length(unsigned street_id){
    std::vector<unsigned> segmentIds;
    int numSegments = 0;
    double totalLength = 0;
    
    segmentIds = find_street_street_segments(street_id);
    numSegments = segmentIds.size();
    
    for(int i=0;i<numSegments;i++){
        totalLength = totalLength + find_street_segment_length(segmentIds[i]);
    }
    return totalLength;
}

//Returns the travel time to drive a street segment in seconds 
//(time = distance/speed_limit)
//convert km/h to m/s = km/h * 1000 / 3600 = km/h / 3.6
double find_street_segment_travel_time(unsigned street_segment_id){
    double length, time; 
    double speedLimitMS;
    float speedLimitKmH;
    
    speedLimitKmH = getInfoStreetSegment(street_segment_id).speedLimit;
    speedLimitMS = speedLimitKmH/3.6;
    
    length = find_street_segment_length(street_segment_id);
    time = length / speedLimitMS;
    
    return time;
}

unsigned find_closest_point_of_interest(LatLon my_position){
    double min = 9999999999;                    //initializing minimum to a large number 
    int nearestPointIndex = 0;                  //contains the index of the nearest point 
    
    for (int i = 0; i < getNumPointsOfInterest(); i++){                                               //looping through all points of interest on the map
        double temp = find_distance_between_two_points(my_position, getPointOfInterestPosition(i));   //finding distance between a point of interest and current position
        if(temp <= min){
            min = temp; 
            nearestPointIndex = i;                                                                      //storing the index of the point of interest if it is the min
        }
    }
    
    return unsigned(nearestPointIndex);                                                                           //return the POI index at the end
}

unsigned find_closest_intersection(LatLon my_position){
    double min = 9999999999;                                                                            //initializing minimum to a large number 
    int nearestIntIndex = 0;                                                                            //contains the index of the nearest point 
        
    for (int i = 0; i < getNumIntersections(); i++){                                                  //looping through all intersections on the map
        double temp = find_distance_between_two_points(my_position, 
                getIntersectionPosition(i));                                                            //finding distance between an intersection and current position
        if(temp <= min){
            min = temp; 
            nearestIntIndex = i;                                                                        //storing the index of the intersection if it is the min
        }
    }
    return unsigned(nearestIntIndex);   

}

//===============================NEEDS WORK=========================================
std::vector<unsigned> find_street_ids_from_partial_street_name(std::string street_prefix){
    std::vector<unsigned> streetIDMatch, IDsFromMap; 
    std::transform(street_prefix.begin(), street_prefix.end(), street_prefix.begin(), ::tolower);
    std::string currentName;
    
    int idLower1,idLower2,idUpper1,idUpper2,lowestID, highestID,numLowerIDs,numUpperIDs;
    
    std::map<std::string,std::vector<unsigned>>::iterator nameLowerIt = streetNameIndexMap.lower_bound(street_prefix);
    std::map<std::string,std::vector<unsigned>>::iterator nameUpperIt = streetNameIndexMap.upper_bound(street_prefix);
    
    nameLowerIt++;
    nameUpperIt--;
    
    /*
    numLowerIDs = (nameLowerIt->second).size();
    numUpperIDs = (nameUpperIt->second).size();
        
    lowestID=getNumStreets()-1;
    highestID=0;
    
    idLower1 = nameLowerIt->second[0];
    idLower2 = nameLowerIt->second[numLowerIDs];
    idUpper1 = nameUpperIt->second[0];
    idUpper2 = nameUpperIt->second[numUpperIDs];
    
    if(idLower1<lowestID){
        lowestID=idLower1;
    }
    if(idLower2<lowestID){
        lowestID=idLower2;
    }
    if(idUpper1<lowestID){
        lowestID=idUpper1;
    }
    if(idUpper2<lowestID){
        lowestID=idUpper2;
    }
    
    if(idLower1>highestID){
        highestID=idLower1;
    }
    if(idLower2>highestID){
        highestID=idLower2;
    }
    if(idUpper1>highestID){
        highestID=idUpper1;
    }
    if(idUpper2>highestID){
        highestID=idUpper2;
    }
    
    /*
    if(lowestID<0){
        lowestID=0;
    }
    if(highestID>(getNumStreets()-1){
        highestID=getNumStreets()-1;
    }

    streetIDMatch.clear();
    
    if((nameLowerIt != streetNameIndexMap.end()) && (nameUpperIt != streetNameIndexMap.end())){
        std::cout << "===================================================================================" << std::endl;
        std::cout << "lower___+++++__" << lowestID << std::endl;
        std::cout << "upper___+++++__" << highestID << std::endl;
    }

    
    for(int i=lowestID; i <= highestID;i++){
        currentName = getStreetName(i);
        IDsFromMap = streetNameIndexMap[currentName];
        
        std::cout << street_prefix << "_____" << currentName << std::endl;
        
        if(currentName.compare(0, street_prefix.size(), street_prefix) == 0){
	    for(int id=0;id < IDsFromMap.size();id++){
                //streetIDMatch.insert(streetIDMatch.end(), IDsFromMap.begin(), IDsFromMap.end());

                streetIDMatch.push_back(IDsFromMap[id]);
	    }
	}
    }
    */
    
    while(nameUpperIt != nameLowerIt){
	currentName = "";
	IDsFromMap.clear();

	currentName = nameLowerIt->first;
	IDsFromMap = nameLowerIt->second;
        
        std::cout << street_prefix << "_____" << currentName << std::endl;
        std::cout << IDsFromMap[0] << std::endl;
        
	if(currentName.compare(0, street_prefix.size(), street_prefix) == 0){
	    for(int i=0;i<IDsFromMap.size();i++){
                streetIDMatch.insert(streetIDMatch.end(), IDsFromMap.begin(), IDsFromMap.end());

               // streetIDMatch.push_back(IDsFromMap[i]);
	   }
	}
	nameLowerIt++;
   }
    
    return streetIDMatch; 
}

