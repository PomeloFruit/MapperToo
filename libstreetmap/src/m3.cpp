#include "m1.h"
#include "m2.h"
#include "m3.h"

#include "LatLon.h"
#include "latLonToXY.h"
#include "StreetsDatabaseAPI.h"
#include "directionInfo.h"
#include "directionObject.h"

#include <math.h>
#include <vector>
#include <list>
#include <algorithm>
#include <queue>
#include <string>
#include <limits>
#include <iostream>

std::vector<unsigned> bfsPath(Node *sourceNode, const unsigned destID, const double rtPen, const double ltPen);

double travelTimeAdd(unsigned existingSeg, unsigned newSeg, const double rt_penalty, 
                                                    const double lt_penalty);
std::vector<unsigned> getFinalPath(Node *currNode,unsigned start);


// Returns a path (route) between the start intersection and the end
// intersection, if one exists. This routine should return the shortest path
// between the given intersections, where the time penalties to turn right and
// left are given by right_turn_penalty and left_turn_penalty, respectively (in
// seconds).  If no path exists, this routine returns an empty (size == 0)
// vector.  If more than one path exists, the path with the shortest travel
// time is returned. The path is returned as a vector of street segment ids;
// traversing these street segments, in the returned order, would take one from
// the start to the end intersection.


std::vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start, 
        const unsigned intersect_id_end, const double right_turn_penalty, 
                                        const double left_turn_penalty){
    
    std::vector<unsigned> path;
    Node start = dir.Nodes[intersect_id_start];
    path = bfsPath(&start, intersect_id_end, right_turn_penalty, left_turn_penalty);
    
    return path;
}

struct compareQ {
    bool operator()(const waveElem &a, const waveElem &b){
        return a.travelTime > b.travelTime;
    }
};

std::vector<unsigned> bfsPath(Node *sourceNode, const unsigned destID, 
                                            const double rtPen, const double ltPen) {
    std::priority_queue< waveElem, std::vector<waveElem>, compareQ> wavefront;
    std::vector< unsigned > changedNodes;
    wavefront.push(waveElem(NONODE, sourceNode, NOEDGE, 0));
  
    while(!wavefront.empty()){
        
        // get next intersection node
        waveElem wave = wavefront.top();
        wavefront.pop();
        
        Node *currNode = wave.node;

        // if best path from this wave or if wave is at start
        if(wave.travelTime < currNode->bestTime){
            
            // if this was better path to node, update
            currNode->reachingEdge = wave.edgeID;
            currNode->bestTime = wave.travelTime;
            if(wave.reachingNode != NONODE){
                currNode->reachingNode = &(dir.Nodes[wave.reachingNode]);
            }
            changedNodes.push_back(currNode->id);
            
            // if at destination
            if(currNode->id == destID){
                
                //extract the path from the nodes
                std::vector<unsigned> path;
                path = getFinalPath(currNode,sourceNode->id);
                
                //reset all affected nodes values
                for(unsigned i=0 ; i<changedNodes.size() ; i++){
                    Node &temp = dir.Nodes[changedNodes[i]];
                    temp.reachingNode = NULL;
                    temp.reachingEdge = NOEDGE;
                    temp.bestTime = NOTIME;
                }
                
                return path;
            }
        
            for(unsigned i=0 ; i < currNode->outEdges.size(); i++){
                Node *toNode = currNode->toNodes[i];
                unsigned toEdge = currNode->outEdges[i];
                
                // if same segment as current
                if(wave.edgeID == toEdge){
                    continue;
                }

                double toNodeTime;
                toNodeTime = currNode->bestTime + travelTimeAdd(wave.edgeID,toEdge,rtPen, ltPen);
                wavefront.push(waveElem(currNode->id, toNode, toEdge, toNodeTime));
            }
        }
    }
    return std::vector<unsigned>();
}

std::vector<unsigned> getFinalPath(Node *currNode, unsigned start){
    std::vector<unsigned> reversed;
   
    while(currNode!=NULL){
        if(currNode->reachingEdge != NOEDGE){
            reversed.push_back(currNode->reachingEdge);

            if(getInfoStreetSegment(currNode->reachingEdge).to == start || 
                    getInfoStreetSegment(currNode->reachingEdge).from == start){
                break;
            }
        }
        

        currNode = currNode->reachingNode;
    }

    std::vector<unsigned> properOrder;
    
    for(int i=reversed.size()-1; i>=0; i--){
        properOrder.push_back(reversed[i]);
    }
    return properOrder;
}




// completed and tested
// ==================================================================================================================

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
        time = time + travelTimeAdd(path[i-1], path[i], right_turn_penalty, left_turn_penalty);
    }
    
    return time;
}

double travelTimeAdd(unsigned existingSeg, unsigned newSeg, const double rt_penalty, 
                                                    const double lt_penalty){
    double time = 0;
    
    if(existingSeg != NOEDGE){
        TurnType turn = find_turn_type(existingSeg, newSeg);
        if(turn == TurnType::RIGHT){
            time = rt_penalty;
        } else if (turn == TurnType::LEFT){
            time = lt_penalty;
        }
    }
    
    time = time + find_street_segment_travel_time(newSeg);
    
    return time;
}