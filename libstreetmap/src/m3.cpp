#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m3Helpers.h"

#include "LatLon.h"
#include "latLonToXY.h"
#include "StreetsDatabaseAPI.h"
#include "directionInfo.h"

#include <math.h>
#include <vector>
#include <list>
#include <algorithm>
#include <queue>
#include <string>
#include <limits>
#include <iostream>

//=========================== Function Definitions ===========================

/* find_path_between_intersections function
 * - finds the fastest path between start and end intersection id
 * - computes fastest path taking into account left/right turn penalties
 * 
 * @param intersect_id_start <unsigned> - intersection Id of start intersection
 * @param intersect_id_end <unsigned> - intersection Id of end intersection
 * @param right_turn_penalty <double> - time penalty for doing a right turn (seconds)
 * @param left_turn_penalty <double> - time penalty for doing a left turn (seconds)
 * 
 * @return path <std::vector<unsigned>> - the ordered set of street segment ids making
 *                                        up the fastest path between start and end
 */

std::vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start, 
        const unsigned intersect_id_end, const double right_turn_penalty, const double left_turn_penalty){
    
    std::vector<unsigned> path;
    if(intersect_id_start != intersect_id_end){
        Node start = Dir.Nodes[intersect_id_start];
        path = getPath(&start, intersect_id_end, right_turn_penalty, left_turn_penalty);
    }
    return path;
}


/* find_turn_type function
 * - finds the turn type based on angle between entrance and exit segments
 * - based on header determined turn types
 * 
 * @param street_segment1 <unsigned> - street segment ID of entrance to intersection
 * @param street_segment2 <unsigned> - street segment ID of exit from intersection
 * 
 * @return <TurnType> - the turn type between seg1(in) and seg2(out)
 */

TurnType find_turn_type(unsigned street_segment1, unsigned street_segment2){
   
    double angle = findAngleBetweenSegs(street_segment1, street_segment2);
    
    // if turn angle -PI < dAngle < 0 or if dAngle > PI
    if(angle == SAMESTREET){ // same id
        return TurnType::STRAIGHT;        
    } else if(angle == NOINTERSECTION){ // segments dont intersect
        return TurnType::NONE;     
    } else if((angle < 0 && angle > -M_PI)|| angle > M_PI){ //negative angle
        return TurnType::LEFT;
    } else {
        return TurnType::RIGHT;
    }
}


/* compute_path_travel_time function
 * - finds the travel time on the path with left/right turn penalties
 * - adds all segment travel times, and adds all turn penalties needed between different segs
 * 
 * @param path <std::vector<unsigned>> - a ordered list of all the street segments making up the path
 * @param right_turn_penalty <double> - the time penalty to add for making a right turn
 * @param left_turn_penalty <double> - the time penalty to add for making a left turn
 * 
 * @return time <double> - the time to travel the path, 0 if no continuous path exists
 */

double compute_path_travel_time(const std::vector<unsigned>& path, const double right_turn_penalty, const double left_turn_penalty){
    double time = 0;
    
    // no path exists, so return 0
    if(path.size() == 0){
        return time;
    }
    
    // add first segment travel time
    time = find_street_segment_travel_time(path[0]);
    
    // add turn penalties between all segments and add segment travel times
    for(unsigned i = 1; i<path.size(); i++){
        time = time + travelTimeAdd(path[i-1], path[i], right_turn_penalty, left_turn_penalty);
    }
        
    return time;
}