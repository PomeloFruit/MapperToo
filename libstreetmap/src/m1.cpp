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
#include <algorithm> //what does algorithm do????????
#include <string>
#include <iostream>

//=========================== Type Declarations ===========================
typedef std::vector<unsigned> IDVector;

//=========================== Forward Declarations ===========================
double find_street_length_preload(unsigned street_id);
double find_street_segment_length_preload(unsigned street_segment_id);
double find_street_segment_travel_time_preload(unsigned street_segment_id);

//============================= Global Variables ============================= 

//map of <street ids, street segments with the same street ids>
std::map<int, std::vector<unsigned>> streetNameMap;

//This is an unordered map of <street ids, street segments with the same street ids>
std::unordered_map<int, std::vector<unsigned>> streetMapFast; 

//This is a map of if <partial street name, street ids with the same partial street name>
//ie the name Bloor becomes five vectors: b, bl, blo, bloo, bloor
std::map<std::string, std::vector<unsigned>> streetNameIndexMap;

//This is a vector of vectors of intersections on a street 
std::vector<std::vector<unsigned>> streetIntersections; 



std::vector<std::vector<unsigned>> streetSegIDVector;
std::vector<std::vector<std::string>> streetSegNameVector;
std::vector<double> segLength;
std::vector<double> streetLength;
std::vector<double> segTravelTime;


//========================= Function Implementations =========================




///////////////////////
// do not put variable declarations in for loops!!!!!!!
//////////////////////
bool load_map(std::string path/*map_path*/) {
    bool load_successful = false; 
    
    //Indicates whether the map has loaded successfully
    load_successful = loadStreetsDatabaseBIN(path);
    
    if(load_successful){      
        
        //clears all global data structures before filling with new data
        streetNameMap.clear();
        streetMapFast.clear();
        streetNameIndexMap.clear();
        streetIntersections.clear();
        streetSegIDVector.clear();
        segLength.clear();
        streetLength.clear();
        segTravelTime.clear();
        
        int segStreetID;
        
        for(unsigned i=0; i<unsigned(getNumStreetSegments()); i++){
            
            segStreetID = getInfoStreetSegment(i).streetID;
            
            // inserts segmentID into vector at respective streetID position
            // within streetNameMap
            if(streetNameMap.count(segStreetID)== 0){
                streetNameMap.insert(std::make_pair(segStreetID,IDVector()));
                streetNameMap[segStreetID].clear();
            }
            streetNameMap[segStreetID].push_back(i);
            
            // inserts segmentID into vector at respective streetID position
            // within streetMapFast
            if(streetMapFast.count(segStreetID) == 0){
                streetMapFast.insert(std::make_pair(segStreetID,IDVector()));
                streetMapFast[segStreetID].clear(); 
            }
            streetMapFast[segStreetID].push_back(i);
            
            // calculates and add segment length to segLength vector
            // calculates and add travel time of segment to segTravelTime vector
            segLength.push_back(find_street_segment_length_preload(i));
            segTravelTime.push_back(find_street_segment_travel_time_preload(i));
        }
        
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        
        std::string currentStreetName;
        IDVector::iterator intersectionIt;
        IDVector toBeInserted; //needs to be renamed!!!!!!!!!!!!!!!!!!!!!
        unsigned intersectionID1,intersectionID2;
        
        for(unsigned i=0; i<unsigned(getNumStreets()); i++){
            IDVector allSegments = find_street_street_segments(i); 
            currentStreetName = getStreetName(i);
            toBeInserted.clear();
            
            std::transform(currentStreetName.begin(), currentStreetName.end(), 
                                        currentStreetName.begin(), ::tolower);
            
            for(int j = 1; j <= int(currentStreetName.length()); j++){
                std::string temp = currentStreetName.substr(0, j); 
                
                if(streetNameIndexMap.count(temp) == 0){
                    streetNameIndexMap.insert(std::make_pair(temp,IDVector()));
                    streetNameIndexMap[temp].clear();
                }
                streetNameIndexMap[temp].push_back(i); 
            }
            
            
            streetLength.push_back(find_street_length_preload(i));
            
            int numSegments = allSegments.size(); 
            
            
            for(int j = 0; j < numSegments; j++){
                intersectionID1 = getInfoStreetSegment(allSegments[j]).from;
                intersectionID2 = getInfoStreetSegment(allSegments[j]).to; 
                
                intersectionIt = std::find(toBeInserted.begin(), toBeInserted.end(), 
                                                        intersectionID1); 
                if(intersectionIt == toBeInserted.end()){
                    toBeInserted.push_back(intersectionID1); 
                }
                
                intersectionIt = std::find(toBeInserted.begin(), toBeInserted.end(),
                                                        intersectionID2); 
                if(intersectionIt == toBeInserted.end()){
                    toBeInserted.push_back(intersectionID2); 
                }
                
            }
            streetIntersections.push_back(toBeInserted); 
        }
        
        
        
        
        
        int numOfSegs;
        IDVector intersectionIds;
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
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    
    
    return load_successful;
}

/* close_map function
 * - clears all data in global data structures
 * - calls for API to close street database
 * 
 * @param none
 * @return void
 */
void close_map() {
    streetNameMap.clear();
    streetMapFast.clear();
    streetNameIndexMap.clear();
    streetIntersections.clear();
    streetSegIDVector.clear();
    segLength.clear();
    streetLength.clear();
    segTravelTime.clear();
    closeStreetDatabase();
}


std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id){
    IDVector streetSegs;
    streetSegs.clear();
    
    try {
        streetSegs = streetSegIDVector.at(intersection_id);
    } catch(std::exception& e) {
        std::cout << "Intersection not found!" << std::endl;
    }
    
    return streetSegs;  
}


std::vector<std::string> find_intersection_street_names(unsigned intersection_id){
    std::vector<std::string> streetNames;
    streetNames.clear();
    
    try {
        streetNames = streetSegNameVector.at(intersection_id);
    } catch(std::exception& e) {
        std::cout << "Intersection not found!" << std::endl;
    }
    
    return streetNames;
}

//=======================comment this===============================
bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2){
    IDVector segsInt1=find_intersection_street_segments(intersection_id1);
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

//=======================comment this===============================
std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id){
    IDVector segsOrigin=find_intersection_street_segments(intersection_id);
    IDVector connectedIntersections;
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
    IDVector segIDs;
    segIDs.clear();
    
    try {
        segIDs = streetMapFast.at(street_id);
    } catch(std::exception& e) {
        std::cout << "Street not found!" << std::endl;
    }
    
    return segIDs;
}

std::vector<unsigned> find_all_street_intersections(unsigned street_id){ 
    IDVector intersections;
    intersections.clear();
    
    try {
        intersections = streetIntersections.at(street_id);
    } catch(std::exception& e) {
        std::cout << "Street not found!" << std::endl;
    }
    
    return intersections;
}

//=======================comment this===============================
std::vector<unsigned> find_intersection_ids_from_street_ids(unsigned street_id1,unsigned street_id2){
    IDVector street1IntersectionVector, street2IntersectionVector, intersectingIDs;
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


//DONE commenting between aaaaaaa and bbbbbbbbbb
//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

/* find_distance_between_two_points function
 *  - calculates the distance between point1 and point2
 * 
 *  - finds the average latitude of the 2 points in radians for to
 *    get accurate projection factor
 *  - multiplies both points' longitude by projection factor to get x coordinate
 *  - finds different between points' latitude to get y coordinate
 *  - finds hypotenuse of x and y coordinates
 *  - follows given formula to get distance in meters
 * 
 * @param street_segment_id <unsigned> - id for street segment
 * @return distance <double> - distance between point1 and point2 in meters
 */

double find_distance_between_two_points(LatLon point1, LatLon point2){
    double pt1x,pt1y,pt2x,pt2y;
    double dxSquared,dySquared,hypotenuse,distance;
    
    double pt1LatInRad = point1.lat()*DEG_TO_RAD;
    double pt2LatInRad = point2.lat()*DEG_TO_RAD;
    double averageLatInRad = (pt1LatInRad+pt2LatInRad)/2.0;
    double projectionFactor = cos(averageLatInRad);
    
    pt1x = point1.lon()*projectionFactor;
    pt2x = point2.lon()*projectionFactor;
    pt1y = point1.lat();
    pt2y = point2.lat();
    
    dxSquared = pow(pt2x-pt1x,2);
    dySquared = pow(pt2y-pt1y,2);
    hypotenuse = sqrt(dxSquared+dySquared);
    distance = hypotenuse*EARTH_RADIUS_IN_METERS*DEG_TO_RAD;
    
    return distance;
}


/* find_street_segment_length_preload function
 * *** for use in load_map function to pre-calculate all segment lengths when
 *     loading map ***
 *  - finds the number of curve points within the segment
 *  - call find_distance_between_two_points to find distance between start+curve,
 *    curve+curve, and curve+end points
 *  - add all distances between points into totalLength
 * 
 * @param street_segment_id <unsigned> - id for street segment
 * @return totalLength <double> - length of street_segment_id in meters
 */

double find_street_segment_length_preload(unsigned street_segment_id){
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


/* find_street_segment_length function
 * - attempts to find segment length of street__segment_id within segLength vector
 *     - if fails to find street segment, error message is printed
 * 
 * @param street_segment_id <unsigned> - id for street segment
 * @return length <double> - length of street_segment_id in meters
 */

double find_street_segment_length(unsigned street_segment_id){
    double length = 0;
    
    try {
        length = segLength.at(street_segment_id);
    } catch(std::exception& e) {
        std::cout << "Street segment not found!" << std::endl;
    }
    
    return length;
}


/* find_street_length_preload function
 * *** for use in load_map function to pre-calculate all street lengths when
 *     loading map ***
 *  - finds all the segments of street_id
 *  - totals up all segment lengths into totalLength
 * 
 * @param street_id <unsigned> - id for street
 * @return totalLength <double> - length of street_id in meters
 */

double find_street_length_preload(unsigned street_id){
    IDVector segmentIds;
    int numSegments = 0;
    double totalLength = 0;
    
    segmentIds = find_street_street_segments(street_id);
    numSegments = segmentIds.size();
    
    for(int i=0;i<numSegments;i++){
        totalLength = totalLength + find_street_segment_length(segmentIds[i]);
    }
    
    return totalLength;
}


/* find_street_length function
 * - attempts to find street length of street_id within streetLength vector
 *     - if fails to find street, error message is printed
 * 
 * @param street_id <unsigned> - id for street
 * @return length <double> - length of street_id in meters
 */

double find_street_length(unsigned street_id){
    double length = 0;
    
    try {
        length = streetLength.at(street_id);
    } catch(std::exception& e) {
        std::cout << "Street not found!" << std::endl;
    }
    
    return length;
}


/* find_street_segment_travel_time_preload function
 * *** for use within load_map function to pre-calculate all travel times when
 *     loading map ***
 * - retrieves the speed limit in km/h from API
 * - converts speed limit from km/h to m/s
 * - retrieves length of segment in meters
 * - calculates time needed to travel segment in seconds
 * 
 * @param street_segment_id <unsigned> - id for street segment
 * @return time <double> - time to travel on street segment in seconds
 */

double find_street_segment_travel_time_preload(unsigned street_segment_id){
    double length, time; 
    double speedLimitMS;
    float speedLimitKmH;
    
    speedLimitKmH = getInfoStreetSegment(street_segment_id).speedLimit;
    speedLimitMS = speedLimitKmH/3.6;
    
    length = find_street_segment_length(street_segment_id);
    time = length / speedLimitMS;
    
    return time;
}


/* find_street_segment_travel_time function
 * - attempts to find travel time for street_segment_id within segTravelTime vector
 *     - if fails to find street segment, error message is printed
 * 
 * @param street_segment_id<unsigned> - id for street segment
 * @return time <double> - time to travel on street segment in seconds
 */

double find_street_segment_travel_time(unsigned street_segment_id){
    double time = 0;
    
    try {
        time = segTravelTime.at(street_segment_id);
    } catch(std::exception& e) {
        std::cout << "Street segment not found!" << std::endl;
    }
    
    return time;
}

//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb


//COMMMMMENNNTTTTT this not line by line sir!!!!!
//=======================================================================================================
//=======================================================================================================
//=======================================================================================================
//=======================================================================================================


///change the min values to something less random, use earth radius if unsure.....

unsigned find_closest_point_of_interest(LatLon my_position){
    double min = 9999999999;
    //contains the index of the nearest point 
    int nearestPointIndex = 0; 
    
    //iterating through all points of interest on the map
    for (int i = 0; i < getNumPointsOfInterest(); i++){      
        //finding distance between a point of interest and current position                                         
        double temp = find_distance_between_two_points(my_position, getPointOfInterestPosition(i));   
        if(temp <= min){
            //storing the index of the point of interest if it is the min
            min = temp; 
            nearestPointIndex = i;                                                                      
        }
    }
    
    return unsigned(nearestPointIndex); 
}

///change the min values to something less random, use earth radius if unsure.....
unsigned find_closest_intersection(LatLon my_position){
    double min = 9999999999;   
    //contains the index of the nearest point                                                                          
    int nearestIntIndex = 0;                                                                            
    
    //iterating through all intersections on the map
    for (int i = 0; i < getNumIntersections(); i++){          
        //finding distance between an intersection and current position
        double temp = find_distance_between_two_points(my_position, getIntersectionPosition(i));                       
        if(temp <= min){
            //storing the index of the intersection if it is the min
            min = temp; 
            nearestIntIndex = i;                                                                        
        }
    }
    return unsigned(nearestIntIndex);   

}
//=======================================================================================================
//=======================================================================================================
//=======================================================================================================
//=======================================================================================================
//=======================================================================================================


//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

/* find_street_ids_from_partial_street_name function
 * - converts street_prefix string to lower case
 * - attempts to find match to street_prefix within the streetNameIndexMap
 *     - if fails to find match, error message is printed
 * 
 * @param street_prefix <string> - portion of street name to be searched
 * @return matchIDs <vector<unsigned>> - all street ids matching street_prefix
 */

std::vector<unsigned> find_street_ids_from_partial_street_name(std::string street_prefix){
    IDVector matchIDs;
    
    std::transform(street_prefix.begin(), street_prefix.end(), street_prefix.begin(), ::tolower);
    
    try {
        matchIDs = streetNameIndexMap.at(street_prefix);
    } catch(std::exception& e) {
        std::cout << "Match not found!" << std::endl;
    }
    
    return matchIDs;
}

//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb