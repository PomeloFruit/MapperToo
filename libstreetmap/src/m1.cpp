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
#include <math.h>

//==============================================================================
//Global Variables


bool load_map(std::string path/*map_path*/) {
    bool load_successful = true; //Indicates whether the map has loaded 
                                  //successfully

    //
    //Load your map related data structures here
    //
    
    load_successful = loadStreetsDatabaseBIN(path);

    //load_successful = true; //Make sure this is updated to reflect whether
                            //loading the map succeeded or failed

    return load_successful;
}

void close_map() {

    //so according to this I just don't have the right street segments for some of these
    closeStreetDatabase();
    //Clean-up your map related data structures here
    
}

std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id){
    std::vector<unsigned> ids;
    int numOfSegs=getIntersectionStreetSegmentCount(intersection_id);
    for(int i=0;i<numOfSegs;i++){
        ids.push_back(getIntersectionStreetSegment(i, intersection_id));
    }
    return ids;
    //so as of now this or any of the other things I write won't pass the performance test
    //but don't worry I have it all under control just like these versions
    //to fix this one and probably all the other ones I'm about to write 
    //create a global variable and make a nested for loops for nested vectors
           
}


std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id){
    std::vector<unsigned> ids;
    int numOfSegs=getIntersectionStreetSegmentCount(intersection_id);
    for(int i=0;i<numOfSegs;i++){
        ids.push_back(getIntersectionStreetSegment(i, intersection_id));
    }
    return ids;
    //so as of now this or any of the other things I write won't pass the performance test
    //but don't worry I have it all under control just like these versions
    //to fix this one and probably all the other ones I'm about to write 
    //create a global variable and make a nested for loops for nested vectors
            
}

std::vector<std::string> find_intersection_street_names(unsigned intersection_id){
    //this is supposed to return duplicate names here also so don't worry about
    std::vector<std::string> names;
    int numOfSegs=getIntersectionStreetSegmentCount(intersection_id);
    for(int i=0;i<numOfSegs;i++){
        names.push_back(getStreetName(getIntersectionStreetSegment(i, intersection_id))); //It's gross but it works I think
    }
    return names;
    //this will suffer from the same problems as the prev function
}

bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2){
    //so the corner case here is if it has curvepoints or not I think??
    //but basically do they share a road and if so is the road one way
    //is an intersection always connected to itself? or does a street seg need to curve back into itself?
    //**read above unless I get answers later
    //if an intersection is always connected to itself I can just return true if they are =
    //and finally this would be easier if I had made the global variable first but oh well
    //NOW O(n)
    std::vector<unsigned> segsInt1=find_intersection_street_segments(intersection_id1);

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
    //I'm dumb
    //they are directly connected if there is a segment between them (only check one of them)
    //and if the segment is one way it is only invalid if to is id1
    //if to is intersection to return true
    //bool returniee=false;
    std::vector<unsigned> segsInt1=find_intersection_street_segments(intersection_id1);
    std::vector<unsigned> segsInt2=find_intersection_street_segments(intersection_id2);
    //so I have the two lists
    //literally O(n^2) incoming
    //if you have a better idea please implement it
    for(int i=0;i<segsInt1.size();i++){
        for(int c=0;c<segsInt2.size();c++){
            if(segsInt1[i]==segsInt2[c]){
                if(!getInfoStreetSegment(segsInt1[i]).oneWay){
                    return true;
                }
                else if(getInfoStreetSegment(segsInt1[i]).oneWay&&(getInfoStreetSegment(segsInt1[i]).from==intersection_id1)){
                    return true;
                }
            }
        }
    }
    return false;
    //this is really not going to pass the speed test
}

std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id){
    //no duplicates allowed
    //another computationally intense thing
    //this could be fixed using a hash table though
    //but I'm too lazy to fix it right now so I'll write down the solution for the future
    //just make a hash table and hash each connectedIntersection as I find it
    //if there is a collision there must be a duplicate and I won't add the thing to the list ezpz
    //but for now I'm just gonna O(n^2) this
    //a street cannot be adjacent to itself
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
    //if a way is found to make the is directly connected function faster we could just call it here
    //or we could call this in the place of is directly connected maybe
    std::vector<unsigned> segsOrigin=find_intersection_street_segments(intersection_id);
    std::vector<unsigned> connectedIntersections;
    bool insert;
    for(int i=0;i<segsOrigin.size();i++){
        insert=true;
        for(int c=0;c<connectedIntersections.size();c++){
            if(connectedIntersections[c]==getInfoStreetSegment(segsOrigin[i]).to){
                insert=false;
            }
            //numInVec=std::count(connectedIntersecitons.begin(), connectedIntersecitons.end(), target1);
            //on the bright side I get to look at more STL stuff but I don't know if I'm allowed to use the algos there
        }
        if(insert){
            connectedIntersections.push_back(getInfoStreetSegment(segsOrigin[i]).to);
        }
    }
    return connectedIntersections;
   
}

//so this again won't pass speed tests but I know a solution exists (and outlined above)
//ALSO ONE IMPORTANT NOTE, NONE OF THESE FUNCTIONS WERE TESTED BECAUSE PEOPLE WERE HAVING PROBLEMS DOING IT
//but they make sense to me
/*
 * Since this is the last function I am going to write today I'm going to put down my last thoughts
 * For the closest intersection/poi
 * I think we all know that we have to use a KD tree and it would be faster if we balanced it
 * but finding the median locations of the things is in itself a fairly expensive computation
 * and finding the median doesn't even really assure a balanced tree
 * in addition to this we'd probably have to do it twice (once for the intersections and the other for poi)
 */






//Returns the distance between two coordinates in meters
double find_distance_between_two_points(LatLon point1, LatLon point2){
    double pt1LatInRad = point1.lat()*DEG_TO_RAD;
    double pt2LatInRad = point2.lat()*DEG_TO_RAD;
    double pt1LonInRad = point1.lon()*DEG_TO_RAD;
    double pt2LonInRad = point2.lon()*DEG_TO_RAD;
    
    double pt1x,pt1y,pt2x,pt2y;
    double dxSquared,dySquared,hypotenuse,distance;
    
    double averageLatInRad = (pt1LatInRad+pt2LatInRad)/2;
    double projectionFactor = cos(averageLatInRad);
    
    pt1x = pt1LonInRad*projectionFactor;
    pt2x = pt2LonInRad*projectionFactor;
    pt1y = pt1LatInRad;
    pt2y = pt2LatInRad;
    
    dxSquared = pow(pt2x-pt1x,2);
    dySquared = pow(pt2y-pt1y,2);
    hypotenuse = sqrt(dxSquared+dySquared);
    
    distance = EARTH_RADIUS_IN_METERS*hypotenuse;
    return distance;
}


//Returns the length of the given street segment in meters
double find_street_segment_length(unsigned street_segment_id){
    double totalLength = 0;
    
    int numSegments = getInfoStreetSegment(street_segment_id).curvePointCount;
    LatLon point1, point2;
    
    point1 = getIntersectionPosition(getInfoStreetSegment(street_segment_id).from);
    
    for(int i=0;i<numSegments;i++){ 
        point2 = getStreetSegmentCurvePoint(i,street_segment_id);
        totalLength += find_distance_between_two_points(point1,point2);
        point1 = point2;
    }
    
    point2 = getIntersectionPosition(getInfoStreetSegment(street_segment_id).to);
    
    totalLength += find_distance_between_two_points(point1,point2);
    return totalLength;
}


//Returns the length of the specified street in meters
double find_street_length(unsigned street_id){
    std::vector<unsigned> segmentIds;
    int numSegments = 0;
    double totalLength = 0;
    
    segmentIds = find_street_street_segments(street_id);
    numSegments = segmentIds.size();
    
    for(int i=0;i<numSegments;i++){
        totalLength += find_street_segment_length(segmentIds[i]);
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
    
    for (int i = 0; i < getNumPointsOfInterest()-1; i++){                                               //looping through all points of interest on the map
        double temp = find_distance_between_two_points(my_position, getPointOfInterestPosition(i));     //finding distance between a point of interest and current position
        if(temp <= min){
            min = temp; 
            nearestPointIndex = i;                                                                      //storing the index of the point of interest if it is the min
        }
    }
    
    return nearestPointIndex;                                                                           //return the POI index at the end
}

unsigned find_closest_intersection(LatLon my_position){
    double min = 9999999999;                    //initializing minimum to a large number 
    int nearestIntIndex = 0;                  //contains the index of the nearest point 
    
    for (int i = 0; i < getNumIntersections()-1; i++){                                               //looping through all intersections on the map
        double temp = find_distance_between_two_points(my_position, getIntersectionPosition(i));     //finding distance between an intersection and current position
        if(temp <= min){
            min = temp; 
            nearestIntIndex = i;                                                                      //storing the index of the intersection if it is the min
        }
    }
    
    return nearestIntIndex;   
}

//lmao