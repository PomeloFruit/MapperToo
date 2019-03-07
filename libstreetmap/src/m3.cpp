#include "m1.h"
#include "m2.h"
#include "m3.h"

#include "LatLon.h"
#include "latLonToXY.h"

#include <math.h>
#include <vector>
#include <string>


// Turn type: specifies if the next ture is right, left or going straight
//enum class TurnType
//{
//    STRAIGHT, // going straight
//    RIGHT, // turning right
//    LEFT, // turning left
//    NONE // no turn detected
//};


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
    
    std::vector<unsigned> intersections;
    
    intersections = find_intersection_ids_from_street_ids(segInfo1.streetID, segInfo2.streetID);
    
    if(intersections.size() == 0){
        return TurnType::NONE;
    }
    
    // now must be left or right
    LatLon ptFrom, ptCommon, ptTo;
    int numCurvePts1, numCurvePts2;
    
    numCurvePts1 = segInfo1.curvePointCount;
    numCurvePts2 = segInfo2.curvePointCount;
    
    ptCommon = getStreetSegmentCurvePoint(intersections[0]);
    
    if(numCurvePts1 > 0){
        ptFrom = getStreetSegmentCurvePoint(numCurvePts1-1, street_segment1);
    } else { //no curve points, straight segment
        ptFrom = getIntersectionPosition(segInfo1.from);
    }
    
    if(numCurvePts2 > 0){
        ptTo = getStreetSegmentCurvePoint(0, street_segment2);
    } else { //no curve points, straight segment
        ptTo = getIntersectionPosition(segInfo1.to);
    }
    
    double xFrom, yFrom, xCommon, yCommon, xTo, yTo;
    
    xFrom = xyM.xFromLon(ptFrom.lon());
    yFrom = xyM.yFromLat(ptFrom.lat());
    xCommon = xyM.xFromLon(ptCommon.lon());
    yCommon = xyM.yFromLat(ptCommon.lat());
    xTo = xyM.xFromLon(ptTo.lon());
    yTo = xyM.yFromLat(ptTo.lat());
    
    double aXSeg, aYSeg, bXSeg, bYSeg, dotProd, determinant, turnAngle;
    aXSeg = xCommon-xFrom;
    aYSeg = yCommon-yFrom;
    bXSeg = xTo-xCommon;
    bYSeg = yTo-yCommon;
    
    dotProd = (aXSeg * bXSeg) + (bXSeg * bYSeg);
    determinant = (aXSeg * bYSeg) - (aYSeg * bXSeg);
    turnAngle = atan2(determinant, dotProd);
    
    if(turnAngle>180){
        return TurnType::LEFT;
    } else {
        return TurnType::RIGHT;
    }
};

InfoStreetSegment getInfoStreetSegment(StreetSegmentIndex streetSegmentIdx);

// fetch the latlon of the i'th curve point (number of curve points specified in 
// InfoStreetSegment)
LatLon getStreetSegmentCurvePoint(int i, StreetSegmentIndex streetSegmentIdx);
    
    
}


// Returns the time required to travel along the path specified, in seconds.
// The path is given as a vector of street segment ids, and this function can
// assume the vector either forms a legal path or has size == 0.  The travel
// time is the sum of the length/speed-limit of each street segment, plus the
// given right_turn_penalty and left_turn_penalty (in seconds) per turn implied
// by the path.  If the turn type is STRAIGHT, then there is no penalty
double compute_path_travel_time(const std::vector<unsigned>& path, 
                                const double right_turn_penalty, 
                                const double left_turn_penalty);


// Returns a path (route) between the start intersection and the end
// intersection, if one exists. This routine should return the shortest path
// between the given intersections, where the time penalties to turn right and
// left are given by right_turn_penalty and left_turn_penalty, respectively (in
// seconds).  If no path exists, this routine returns an empty (size == 0)
// vector.  If more than one path exists, the path with the shortest travel
// time is returned. The path is returned as a vector of street segment ids;
// traversing these street segments, in the returned order, would take one from
// the start to the end intersection.
std::vector<unsigned> find_path_between_intersections(
		  const unsigned intersect_id_start, 
                  const unsigned intersect_id_end,
                  const double right_turn_penalty, 
                  const double left_turn_penalty);
