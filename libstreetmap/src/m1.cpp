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
#include "m2.h"
#include "StreetsDatabaseAPI.h"
#include "globals.h"
#include "latLonToXY.h"
#include "grid.h"
#include "drawText.h"

#include <map>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>

//============================= Type Declarations =============================

typedef std::vector<unsigned> IDVector;

//=========================== Forward Declarations ===========================

double find_street_length_preload(unsigned street_id);
double find_street_segment_length_preload(unsigned street_segment_id);
double find_street_segment_travel_time_preload(unsigned street_segment_id);

//============================= Global Variables ============================= 

//unordered map of <street ids, street segments with the same street ids>
std::unordered_map<int, std::vector<unsigned>> streetIDMap; 

//map of <partial street name, street ids with the same partial street name>
//(ie. the name Bloor becomes five vectors: b, bl, blo, bloo, bloor)
std::map<std::string, std::vector<unsigned>> partialStreetNameMap;

//vector of vectors of intersections on a street 
std::vector<std::vector<unsigned>> streetIntersectionsVector; 

//vector of street segment ID indexed on intersection ID
std::vector<std::vector<unsigned>> intersectionSegIDVector;

//vector of street segment names indexed on intersection Id
std::vector<std::vector<std::string>> intersectionSegNameVector;

//vector of street segment lengths indexed on segment ID
std::vector<double> segLengthVector;

//vector of street lengths indexed on street ID
std::vector<double> streetLengthVector;

//vector of street segment travel times indexed on segment ID
std::vector<double> segTravelTimeVector;

streetGrid streetBlock; 
//========================= Function Implementations =========================

/* load_map function
 * - populates all appropriate data in global data structures
 * - calls for API to load street database
 * 
 * @param path <string> - file path for map
 * @return load_successful<boolean> - whether the map has loaded successfully
 */

bool load_map(std::string path/*map_path*/) {
    bool load_successful = false; 
    std::string path_osm;
    const std::string STREETEXT = "street.bin";
    const std::string OSMEXT = "osm.bin";

    //Indicates whether the map has loaded successfully
    load_successful = loadStreetsDatabaseBIN(path);
    std::cout<<"loaded: "<<path<<std::endl;
    
    if(load_successful){
        
        //load osm map data
        path_osm = path.substr(0, path.size()-(STREETEXT.size()+1));
        path_osm = path_osm + OSMEXT;
        loadOSMDatabaseBIN(path_osm);
        
        std::vector<unsigned> collisionList(getNumIntersections(), 0);
        
        std::cout<<"loaded: "<<path_osm<<std::endl;
        
        //clears all global data structures before filling with new data
        streetIDMap.clear();
        partialStreetNameMap.clear();
        streetIntersectionsVector.clear();
        intersectionSegIDVector.clear();
        segLengthVector.clear();
        streetLengthVector.clear();
        segTravelTimeVector.clear();
        streetBlock.poiGrid.clear();
        streetBlock.intGrid.clear();
                
        //==== streetIDMap & segLengthVector & segTravelTimeVector ====
        int segStreetID;
        
        for(unsigned i=0; i<unsigned(getNumStreetSegments()); i++){
            
            segStreetID = getInfoStreetSegment(i).streetID;
                      
            // inserts segmentID into vector at respective streetID position
            // within streetIDMap
            if(streetIDMap.count(segStreetID) == 0){
                streetIDMap.insert(std::make_pair(segStreetID,IDVector()));
                streetIDMap[segStreetID].clear(); 
            }
            streetIDMap[segStreetID].push_back(i);
            
            // calculates and add segment length to segLengthVector vector
            // calculates and add travel time of segment to segTravelTimeVector vector
            segLengthVector.push_back(find_street_segment_length_preload(i));
            segTravelTimeVector.push_back(find_street_segment_travel_time_preload(i));
        }
        
        //==== partialStreetNameMap & streetLengthVector & streetIntersectionsVector ====
        std::string currentStreetName;
        IDVector allSegments; 
        IDVector::iterator intersectionIt;
        IDVector intersectionOnStreet;
        unsigned intersectionID1,intersectionID2;
        int numSegments;

        for(unsigned i=0; i<unsigned(getNumStreets()); i++){
            allSegments = find_street_street_segments(i); 
            numSegments = allSegments.size(); 
            intersectionOnStreet.clear();
            currentStreetName = getStreetName(i);

            //converts street name to lower case
            std::transform(currentStreetName.begin(), currentStreetName.end(), 
                                        currentStreetName.begin(), ::tolower);
            
            
            //Creates the partialStreetNameMap and inserts all street segments 
            //that contain the same partial street name
            
            
            for(int j = 1; j <= int(currentStreetName.length()); j++){
                std::string temp = currentStreetName.substr(0, j); 
                
                if(partialStreetNameMap.count(temp) == 0){
                    partialStreetNameMap.insert(std::make_pair(temp,IDVector()));
                    partialStreetNameMap[temp].clear();
                }
                partialStreetNameMap[temp].push_back(i); 
            }
            
            
            streetLengthVector.push_back(find_street_length_preload(i));
            
            
            //Creates the streetIntersectionsVector 
            //Inserts intersectionOnStreet, a vector of intersections ids on a 
            //street into streetIntersectionsVector
            for(int j = 0; j < numSegments; j++){
                intersectionID1 = getInfoStreetSegment(allSegments[j]).from;
                intersectionID2 = getInfoStreetSegment(allSegments[j]).to; 
                
                if(collisionList[intersectionID1]==0){
                    intersectionOnStreet.push_back(intersectionID1);
                    collisionList[intersectionID1]++;
                }
                if(collisionList[intersectionID2]==0){
                    intersectionOnStreet.push_back(intersectionID2);
                    collisionList[intersectionID2]++;
                }
////                intersectionIt = std::find(intersectionOnStreet.begin(), 
////                            intersectionOnStreet.end(), intersectionID1); 
////                if(intersectionIt == intersectionOnStreet.end()){
////                    intersectionOnStreet.push_back(intersectionID1); 
////                }
////                
////                intersectionIt = std::find(intersectionOnStreet.begin(), 
////                            intersectionOnStreet.end(), intersectionID2); 
////                if(intersectionIt == intersectionOnStreet.end()){
////                    intersectionOnStreet.push_back(intersectionID2); 
////                }
                
            }
            for(unsigned j=0;j<intersectionOnStreet.size();j++){
                collisionList[intersectionOnStreet[j]]=0;
            }
            streetIntersectionsVector.push_back(intersectionOnStreet); 
        }
        
        //====intersectionSegNameVector & intersectionSegIDVector====
        /* Here 2 vectors are created one to populate with street segment ID's 
         * and the other with street names both of those vectors are populated 
         * in the for loop and are then pushed into the larger global variables
         * the value at which the global variables (intersectionSegNameVector 
         * & intersectionSegIDVector) are indexed corresponding to the intersectionID
         */
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
            intersectionSegNameVector.push_back(segNames);
            intersectionSegIDVector.push_back(intersectionIds);
        }
        streetBlock.populateGrid();
       // dt.initilize(getNumStreetSegments());
    }    
    
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
    streetIDMap.clear();
    partialStreetNameMap.clear();
    streetIntersectionsVector.clear();
    intersectionSegIDVector.clear();
    segLengthVector.clear();
    streetLengthVector.clear();
    segTravelTimeVector.clear();
    streetBlock.poiGrid.clear();
    streetBlock.intGrid.clear(); 
    closeStreetDatabase();
    closeOSMDatabase();
}


/* find_intersection_street_segments function
 * - attempts to return all street segment id's connected to intersection_id
 * 
 * @param intersection_id2 <unsigned> - id for source intersection
 * @return streetSegs vector<unsigned> - street segment ids connected to intersection_id
 */

std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id){
    IDVector streetSegs;
    streetSegs.clear();
    
    try {
        streetSegs = intersectionSegIDVector.at(intersection_id);
    } catch(std::exception& e) {
        std::cout << "Intersection not found!" << std::endl;
    }
    
    return streetSegs;  
}


/* find_intersection_street_names function
 * - attempts to return street names connected to intersection_id
 * 
 * @param intersection_id2 <unsigned> - id for source intersection
 * @return streetNames vector<std::string>- street segment names connected to intersection_id
 */

std::vector<std::string> find_intersection_street_names(unsigned intersection_id){
    std::vector<std::string> streetNames;
    streetNames.clear();
    
    try {
        streetNames = intersectionSegNameVector.at(intersection_id);
    } catch(std::exception& e) {
        std::cout << "Intersection not found!" << std::endl;
    }
    
    return streetNames;
}


/* are_directly_connected function
 * Checks both "to" and "from" of street segments connected to intersection_id1
 * if either of them are connected to intersection_id2 unless the road is one way
 * then it checks if intersection_id1 is "to" if it is it will return false
 * otherwise it will return true (if intersection_id2 is found) 
 * 
 * @param intersection_id1 <unsigned> - id for source intersection
 * @param intersection_id2 <unsigned> - id for destination intersection
 * @return <bool> - if they are directly connected it will return true otherwise false
 */

bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2){
    IDVector segsInt1=find_intersection_street_segments(intersection_id1);
    if(intersection_id1==intersection_id2){
        return true;
    }
    for(unsigned i=0;i<segsInt1.size();i++){
        if((unsigned(getInfoStreetSegment(segsInt1[i]).to)==intersection_id2)){
            return true;
        }
        else if((unsigned(getInfoStreetSegment(segsInt1[i]).to)==intersection_id1)
                    && (unsigned(getInfoStreetSegment(segsInt1[i]).from)==intersection_id2)){
            if(!getInfoStreetSegment(segsInt1[i]).oneWay){
                return true;
            }
        }
    }
    return false;
}


/* find_adjacent_intersections function
 * checks "to" and "from" values of all connected street segments
 * it determines if each "to" and "from" values are valid
 * validity is based upon if intersection_id is "to" or "from"
 * and if the intersection that is currently being indexed is intersection_id
 * it then determines if it should insert into the list or not
 * if intersection_id is "to" and the segment is one way it should not insert
 * it then inserts into the list based on isInvalidTo and isInvalidFrom
 * 
 * @param intersection_id <unsigned> - id for source intersection
 * @return connectedIntersections vector<unsigned> - contains id's of all connected intersections
 */

std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id){
    IDVector segsOrigin=find_intersection_street_segments(intersection_id);
    IDVector connectedIntersections;
    bool insert=false;        //true if the destination intersection is reachable from the intersection_id
    unsigned intersection2;
    for(int c=0;unsigned(c)<segsOrigin.size();c++){
        insert=true;
        
        if(unsigned(getInfoStreetSegment(segsOrigin[c]).to)==intersection_id){
            intersection2=unsigned(getInfoStreetSegment(segsOrigin[c]).from);
            //I still need to find duplicates 
        }
        else{
            intersection2=unsigned(getInfoStreetSegment(segsOrigin[c]).to);

        }
        
        if(are_directly_connected(intersection_id, intersection2)){
            for(int i=0;unsigned(i)<connectedIntersections.size();i++){
                if(intersection2==connectedIntersections[i]){
                    insert=false;
                }
            }
        }
        else{
            insert=false;
        }
        if(insert){
            connectedIntersections.push_back(intersection2);
        }
    }
    
    return connectedIntersections;
}


/*find_street_street_segments function
 * -returns a vector of street segments for the given street_id
 * 
 * @param street_id <unsigned> - the street index number of the desired street
 * @return segIDs<ID> - a vector of street segments with the same street ID as street_id
 */

std::vector<unsigned> find_street_street_segments(unsigned street_id){
    IDVector segIDs;
    segIDs.clear();
    
    try {
        segIDs = streetIDMap.at(street_id);
    } catch(std::exception& e) {
        std::cout << "Street not found!" << std::endl;
    }
    
    return segIDs;
}


/*find_all_street_intersections function
 * -return a vector of intersections for the given street_id
 * 
 * @param street_id <unsigned> - the street index number of the desired street
 * @return intersections<IDVector> - a vector of intersections that are on the specified street 
 */

std::vector<unsigned> find_all_street_intersections(unsigned street_id){ 
    IDVector intersections;
    intersections.clear();
    
    try {
        intersections = streetIntersectionsVector.at(street_id);
    } catch(std::exception& e) {
        std::cout << "Street not found!" << std::endl;
    }
    
    return intersections;
}


/* find_intersection_ids_from_street_ids function
 *  - finds intersections that are connected by both street_id1 and street_id2
 * 
 *  - gathers all intersections from street_id1 and street_id2
 *  - compares intersection IDs from both streets
 *      - if match is found, intersection ID is added to instersection IDs
 * 
 * @param street_id1 <unsigned> - id for street 1
 * @param street_id2 <unsigned> - id for street 2
 * @return distance std::vector<unsigned> - ids of intersections in both street_id1 and street_id2
 */

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
 * - attempts to find segment length of street__segment_id within segLengthVector vector
 *     - if fails to find street segment, error message is printed
 * 
 * @param street_segment_id <unsigned> - id for street segment
 * @return length <double> - length of street_segment_id in meters
 */

double find_street_segment_length(unsigned street_segment_id){
    double length = 0;
    
    try {
        length = segLengthVector.at(street_segment_id);
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
 * - attempts to find street length of street_id within streetLengthVector vector
 *     - if fails to find street, error message is printed
 * 
 * @param street_id <unsigned> - id for street
 * @return length <double> - length of street_id in meters
 */

double find_street_length(unsigned street_id){
    double length = 0;
    
    try {
        length = streetLengthVector.at(street_id);
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
 * - attempts to find travel time for street_segment_id within segTravelTimeVector vector
 *     - if fails to find street segment, error message is printed
 * 
 * @param street_segment_id<unsigned> - id for street segment
 * @return time <double> - time to travel on street segment in seconds
 */

double find_street_segment_travel_time(unsigned street_segment_id){
    double time = 0;
    
    try {
        time = segTravelTimeVector.at(street_segment_id);
    } catch(std::exception& e) {
        std::cout << "Street segment not found!" << std::endl;
    }
    
    return time;
}


/*find_closest_POI function 
 * -increments through all POIs and calculates the distance between my_position and the POI
 * -stores the index of the minimum distance 
 * 
 * @param my_position <LatLon> - latitude and longitude coordinates of the desired position 
 * @return nearestPointIndex<unsigned> - the index of the nearest POI
 */


unsigned find_closest_point_of_interest(LatLon my_position){
//    double min = EARTH_RADIUS_IN_METERS;
//    int nearestPOIIndex = 0; 
//
//    for(int i = 0; i < getNumPointsOfInterest(); i++){
//        double temp = find_distance_between_two_points(my_position, getPointOfInterestPosition(i));
//        if(temp <= min){
//            min = temp;
//            nearestPOIIndex = i;  
//        }
//    }
//    std::cout<<"POI:" << nearestPOIIndex<<std::endl;
//    return unsigned(nearestPOIIndex); 
    int poi = streetBlock.findNearestPOI(my_position);
    return unsigned(poi); 
}

/*find_closest_intersection function 
 * -increments through all intersections and calculates the distance between my_position and the intersection
 * -stores the index of the minimum distance 
 * 
 * @param my_position <LatLon> - latitude and longitude coordinates of the desired position 
 * @return nearestIntIndex<unsigned> - the index of the nearest intersection
 */

unsigned find_closest_intersection(LatLon my_position){
//    double min = EARTH_RADIUS_IN_METERS;
//    int nearestIntIndex = 0; 
//
//    for(int i = 0; i < getNumIntersections(); i++){
//        double temp = find_distance_between_two_points(my_position, getIntersectionPosition(i));
//        if(temp <= min){
//            min = temp;
//            nearestIntIndex = i;
//        }
//    }
//    std::cout<<"INT:" << nearestIntIndex<<std::endl;
//    return unsigned(nearestIntIndex); 
    
    int intersection = streetBlock.findNearestInt(my_position);  
    return unsigned(intersection); 
}

/* find_street_ids_from_partial_street_name function
 * - converts street_prefix string to lower case
 * - attempts to find match to street_prefix within the partialStreetNameMap
 *     - if fails to find match, error message is printed
 * 
 * @param street_prefix <string> - portion of street name to be searched
 * @return matchIDs <vector<unsigned>> - all street ids matching street_prefix
 */

std::vector<unsigned> find_street_ids_from_partial_street_name(std::string street_prefix){
    IDVector matchIDs;
    
    std::transform(street_prefix.begin(), street_prefix.end(), street_prefix.begin(), ::tolower);
    
    try {
        matchIDs = partialStreetNameMap.at(street_prefix);
    } catch(std::exception& e) {
        //std::cout << "Match not found!" << std::endl;
    }
    
    return matchIDs;
}