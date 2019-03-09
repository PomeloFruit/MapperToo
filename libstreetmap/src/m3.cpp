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

mapBoundary xyM;

double angleCorrectionFromDirection(double xSeg, double ySeg, double angle);
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
    xyM.initialize();
    InfoStreetSegment segInfo1, segInfo2;
    segInfo1 = getInfoStreetSegment(street_segment1);
    segInfo2 = getInfoStreetSegment(street_segment2);
    
        //std::cout << "==================================================================\n";

    std::cout << getStreetName(segInfo1.streetID) << std::endl;
    std::cout << getStreetName(segInfo2.streetID) << std::endl;
    
    if(segInfo1.streetID == segInfo2.streetID){
       // std::cout << "returned straight" << std::endl;
        return TurnType::STRAIGHT;        
    }
    
    int numCurvePts1, numCurvePts2;
    
    numCurvePts1 = segInfo1.curvePointCount;
    numCurvePts2 = segInfo2.curvePointCount;
    LatLon ptFrom, ptCommon, ptTo, tempPt;
    
    if(segInfo1.from == segInfo2.from){
         std::cout << "from from" <<std::endl;
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
         std::cout << "from to" <<std::endl;
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
         std::cout << "to from" <<std::endl;
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
        std::cout << "to to" <<std::endl;
        
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
         
    double xFrom, yFrom, xCommon, yCommon, xTo, yTo;
    
    xFrom = xyM.xFromLon(ptFrom.lon());
    yFrom = xyM.yFromLat(ptFrom.lat());
    xCommon = xyM.xFromLon(ptCommon.lon());
    yCommon = xyM.yFromLat(ptCommon.lat());
    xTo = xyM.xFromLon(ptTo.lon());
    yTo = xyM.yFromLat(ptTo.lat());
    
    double aXSeg, aYSeg, bXSeg, bYSeg;

    aXSeg = xCommon - xFrom;
    aYSeg = yCommon - yFrom;
    bXSeg = xTo - xCommon;
    bYSeg = yTo - yCommon;
    std::cout << aXSeg << " " << aYSeg << " " << bXSeg << " " << bYSeg <<std::endl;
    
    double aAngle, bAngle, dAngle, a1Angle, b1Angle, d1Angle;
    
    const double RAD_TO_DEG = 57.29577951308232087679815481410517033240547246656432154916;
    
    aAngle = atan2(aYSeg,aXSeg)*RAD_TO_DEG;
    bAngle = atan2(bYSeg,bXSeg)*RAD_TO_DEG;
    std::cout << "angle1A " << aAngle << " angle1B " << bAngle << std::endl;
    dAngle = aAngle - bAngle;
    std::cout << " angle1D " << dAngle << std::endl;
    
    if(dAngle>180){
        dAngle = dAngle - 360;
    } else if(dAngle<-180){
        dAngle = dAngle +360;
    }

    if(dAngle < 0){
            //std::cout << "returned left" << std::endl;
        return TurnType::LEFT;
    } else {
           // std::cout << "returned right" << std::endl;
        return TurnType::RIGHT;
    }
}

double angleCorrectionFromDirection(double xSeg, double ySeg, double angle){
    bool up, right;
    
    if(xSeg >= 0){ //up
        right = true;
    } else {
        right = false;
    }

    if(ySeg >= 0){ //up
        up = true;
    } else {
        up = false;
    }
    
    if(up && !right){ // up and left (quad 2))
        angle = 180 - angle;
    } else if (!up && !right) { // down and right (quad 3)
        angle = 180 + angle;
    } else if (!up && right) { // down and left (quad 4))
        angle = 360 - angle;
    } // else up and right (quad 1, no change))
    
    return angle;
}

//// Returns the time required to travel along the path specified, in seconds.
//// The path is given as a vector of street segment ids, and this function can
//// assume the vector either forms a legal path or has size == 0.  The travel
//// time is the sum of the length/speed-limit of each street segment, plus the
//// given right_turn_penalty and left_turn_penalty (in seconds) per turn implied
//// by the path.  If the turn type is STRAIGHT, then there is no penalty
//double compute_path_travel_time(const std::vector<unsigned>& path, 
//                                const double right_turn_penalty, 
//                                const double left_turn_penalty);
//
//
//// Returns a path (route) between the start intersection and the end
//// intersection, if one exists. This routine should return the shortest path
//// between the given intersections, where the time penalties to turn right and
//// left are given by right_turn_penalty and left_turn_penalty, respectively (in
//// seconds).  If no path exists, this routine returns an empty (size == 0)
//// vector.  If more than one path exists, the path with the shortest travel
//// time is returned. The path is returned as a vector of street segment ids;
//// traversing these street segments, in the returned order, would take one from
//// the start to the end intersection.
//std::vector<unsigned> find_path_between_intersections(
//		  const unsigned intersect_id_start, 
//                  const unsigned intersect_id_end,
//                  const double right_turn_penalty, 
//                  const double left_turn_penalty);
