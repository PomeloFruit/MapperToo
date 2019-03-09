#include "m1.h"
#include "m2.h"
#include "m3.h"

#include "LatLon.h"
#include "latLonToXY.h"
#include "StreetsDatabaseAPI.h"

#include <math.h>
#include <vector>
#include <string>
#include <iostream>

// Returns the turn type between two given segments.
// street_segment1 is the incoming segment and street_segment2 is the outgoing
// one.
// If the two street segments do not intersect, turn type is NONE.
// Otherwise if the two segments have the same street ID, turn type is 
// STRAIGHT.  
// If the two segments have different street ids, turn type is LEFT if 
// going from street_segment1 to street_segment2 involves a LEFT turn 
// and RIGHT otherwise.  Note that this means that even a 0-degree turn
// (same direction) is considered a RIGHT turn when the two street segments
// have different street IDs.
TurnType find_turn_type(unsigned street_segment1, unsigned street_segment2){
    InfoStreetSegment segInfo1, segInfo2;
    segInfo1 = getInfoStreetSegment(street_segment1);
    segInfo2 = getInfoStreetSegment(street_segment2);
    
    if(segInfo1.streetID == segInfo2.streetID){
        return TurnType::STRAIGHT;        
    }
    
    int numCurvePts1, numCurvePts2;
    
    numCurvePts1 = segInfo1.curvePointCount;
    numCurvePts2 = segInfo2.curvePointCount;
    LatLon ptFrom, ptCommon, ptTo;
    
    if(segInfo1.from == segInfo2.from){
        
        if(numCurvePts1 > 0){
            ptFrom = getStreetSegmentCurvePoint(0, street_segment1);
        } else {
            ptFrom = getIntersectionPosition(segInfo1.to);
        }
        
        ptCommon = getIntersectionPosition(segInfo1.from);
        
        if(numCurvePts2 > 0){
            ptTo = getStreetSegmentCurvePoint(0, street_segment2);
        } else {
            ptTo = getIntersectionPosition(segInfo2.to);
        }
        
    } else if (segInfo1.from == segInfo2.to) {
        
        if(numCurvePts1 > 0){
            ptFrom = getStreetSegmentCurvePoint(0, street_segment1);
        } else {
            ptFrom = getIntersectionPosition(segInfo1.to);
        }
        
        ptCommon = getIntersectionPosition(segInfo1.from);
        
        if(numCurvePts2 > 0){
            ptTo = getStreetSegmentCurvePoint(numCurvePts2-1, street_segment2);
        } else {
            ptTo = getIntersectionPosition(segInfo2.from);
        }
        
    } else if (segInfo1.to == segInfo2.from) {
        
        if(numCurvePts1 > 0){
            ptFrom = getStreetSegmentCurvePoint(numCurvePts1-1, street_segment1);
        } else {
            ptFrom = getIntersectionPosition(segInfo1.from);
        }
        
        ptCommon = getIntersectionPosition(segInfo1.to);
        
        if(numCurvePts2 > 0){
            ptTo = getStreetSegmentCurvePoint(0, street_segment2);
        } else {
            ptTo = getIntersectionPosition(segInfo2.to);
        }
        
    } else if (segInfo1.to == segInfo2.to) {
        
        if(numCurvePts1 > 0){
            ptFrom = getStreetSegmentCurvePoint(numCurvePts1-1, street_segment1);
        } else {
            ptFrom = getIntersectionPosition(segInfo1.from);
        }
        
        ptCommon = getIntersectionPosition(segInfo1.to);
        
        if(numCurvePts2 > 0){
            ptTo = getStreetSegmentCurvePoint(numCurvePts2-1, street_segment2);
        } else {
            ptTo = getIntersectionPosition(segInfo2.from);
        }
        
    } else { // no intersections common
        return TurnType::NONE;
    }
    
    double aAngle, bAngle, dAngle;
    double aXSeg, aYSeg, bXSeg, bYSeg;

    aXSeg = ptCommon.lon() - ptFrom.lon();
    aYSeg = ptCommon.lat() - ptFrom.lat();
    bXSeg = ptTo.lon() - ptCommon.lon();
    bYSeg = ptTo.lat() - ptCommon.lat();

    // calculate angle of entrance seg, exit seg, and find difference
    aAngle = atan2(aYSeg,aXSeg);
    bAngle = atan2(bYSeg,bXSeg);
    dAngle = aAngle - bAngle;
    
    // if turn angle -PI < dAngle < 0 or if dAngle > PI
    if((dAngle < 0 && dAngle > -M_PI)|| dAngle > M_PI){ 
        return TurnType::LEFT;
    } else {
        return TurnType::RIGHT;
    }
}


// Returns the time required to travel along the path specified, in seconds.
// The path is given as a vector of street segment ids, and this function can
// assume the vector either forms a legal path or has size == 0.  The travel
// time is the sum of the length/speed-limit of each street segment, plus the
// given right_turn_penalty and left_turn_penalty (in seconds) per turn implied
// by the path.  If the turn type is STRAIGHT, then there is no penalty
double compute_path_travel_time(const std::vector<unsigned>& path, const double right_turn_penalty, 
                                                                    const double left_turn_penalty){
    double time = 0;
    
    // no path exists, so return 0
    if(path.size() == 0){
        return time;
    }
    
    // add first segment travel time
    time = find_street_segment_travel_time(path[0]);
    
    // add turn penalties between all segments and add segment travel times
    for(unsigned i = 1; i<path.size(); i++){
        TurnType turn = find_turn_type(path[i-1], path[i]);
        if(turn == TurnType::RIGHT){
            time = time + right_turn_penalty;
        } else if (turn == TurnType::LEFT){
            time = time + left_turn_penalty;
        }
        time = time + find_street_segment_travel_time(path[i]);
    }
    
    return time;
}


//// Returns a path (route) between the start intersection and the end
//// intersection, if one exists. This routine should return the shortest path
//// between the given intersections, where the time penalties to turn right and
//// left are given by right_turn_penalty and left_turn_penalty, respectively (in
//// seconds).  If no path exists, this routine returns an empty (size == 0)
//// vector.  If more than one path exists, the path with the shortest travel
//// time is returned. The path is returned as a vector of street segment ids;
//// traversing these street segments, in the returned order, would take one from
//// the start to the end intersection.
//std::vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start, 
//                  const unsigned intersect_id_end,
//                  const double right_turn_penalty, 
//                  const double left_turn_penalty);
