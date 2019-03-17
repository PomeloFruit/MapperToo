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

// structure contains compare operator for sorting priority queue
// sorts queue using scores in waveElem such that lowest score is placed at front of queue
struct compareScore {
    bool operator()(const waveElem &a, const waveElem &b){
        return a.score > b.score;
    }
};


/* find_path_between_intersections function
 * - finds the fastest path between start and end intersection id
 * - computes fastest path taking into account left/right turn penalties
 * 
 * @param sourceNode <Node *> - pointer to the start intersection node
 * @param destID <unsigned> - intersection Id of end intersection
 * @param rtPen <double> - time penalty for doing a right turn (seconds)
 * @param ltPen <double> - time penalty for doing a left turn (seconds)
 * 
 * @return path <std::vector<unsigned>> - the ordered set of street segment ids making
 *                                        up the fastest path between start and end (if exists)
 */

std::vector<unsigned> getPath(Node *sourceNode, const unsigned destID, const double rtPen, const double ltPen) {
    std::vector<unsigned> path;
    path.clear();
    
    std::priority_queue< waveElem, std::vector<waveElem>, compareScore> wavefront;
    std::vector< unsigned > changedNodes;
    wavefront.push(waveElem(NONODE, sourceNode, NOEDGE, 0, 0));
    
    LatLon end = Dir.intersectionPos[destID];
  
    // repeat until all possible paths are checked or path is found
    while(!wavefront.empty()){
        
        // get next intersection node
        waveElem wave = wavefront.top();
        wavefront.pop();
        
        Node *currNode = wave.node;

        // if best path from this wave
        if(wave.travelTime < currNode->bestTime){
            
            // if this was better path to node, update
            currNode->reachingEdge = wave.edgeID;
            currNode->bestTime = wave.travelTime;
            if(static_cast<int> (wave.reachingNode) != NONODE){
                currNode->reachingNode = &(Dir.Nodes[wave.reachingNode]);
            }
            changedNodes.push_back(currNode->id);
            
            // if at destination, get the path and exit loop
            if(currNode->id == destID){
                
                //extract the path from the nodes
                path = getFinalPath(currNode,sourceNode->id);
                break;
            }
        
            // add all nodes that can be reached to wavefront
            for(unsigned i=0 ; i < currNode->outEdges.size(); i++){
                Node *toNode = currNode->toNodes[i];
                unsigned toEdge = currNode->outEdges[i];
                
                // if same segment as current
                if(wave.edgeID == toEdge){
                    continue;
                }

                // calculate score and time for the waveElem
                double toNodeTime, score;
                toNodeTime = currNode->bestTime + travelTimeAdd(wave.edgeID,toEdge,rtPen,ltPen);
                score = getNewScore(toNode->id, end, toNodeTime);
                
                // add node to wavefront
                wavefront.push(waveElem(currNode->id, toNode, toEdge, toNodeTime, score));
            }
        }
    }
    
    //reset all affected nodes values
    for(unsigned i=0 ; i<changedNodes.size() ; i++){
        Node &temp = Dir.Nodes[changedNodes[i]];
        temp.reachingNode = NULL;
        temp.reachingEdge = NOEDGE;
        temp.bestTime = NOTIME;
    }

    return path;
}


/* getNewScore function
 * - calculates the A* score values for priority queue sorting
 * - adds time taken to reach Node + (underestimated) ETA to destination
 * - always underestimates the time to reach destination using fastest speed limit in city
 *      - ensures A* will always find fastest path
 * 
 * @param newPoint <unsigned> - intersection ID of the new node addition
 * @param end <LatLon> - latlon position point of the destination intersections
 * @param time <double> - time it took to reach the current node before adding new node
 * 
 * @return score <double> - the score of the new wave element
 */

double getNewScore(unsigned newPoint, LatLon end, double time){
    LatLon start = Dir.intersectionPos[newPoint];
        
    double pointToEndDistance = find_distance_between_two_points(start, end);
   
    double estTimeToDest = pointToEndDistance * Dir.secPerMeter;
    
    double score = time + estTimeToDest;
    
    return score;
}


/* getFinalPath function
 * - backtracks on the path used to reach currNode (destination node)
 * - inserts backtracked segment to the front of list, so that directions go from start to end
 * 
 * @param currNode <Node *> - pointer to the destination intersection 
 * @param start <unsigned> - intersection ID for the start intersection
 * 
 * @return finalPath <std::vector<unsigned>> - an ordered list of street segment ids to get from
 *                                               start to end intersection
 */

std::vector<unsigned> getFinalPath(Node *currNode, unsigned start){
    std::vector<unsigned> finalPath;
   
    while(currNode!=NULL){
        if(static_cast<int> (currNode->reachingEdge) != NOEDGE){
            // add segment used to reach node to front of list
            finalPath.insert(finalPath.begin(), currNode->reachingEdge);

            // if backtracked back to start, we are done
            if(static_cast<unsigned>(getInfoStreetSegment(currNode->reachingEdge).to) == start || 
                    static_cast<unsigned>(getInfoStreetSegment(currNode->reachingEdge).from) == start){
                break;
            }
        }
        
        // backtrack one node
        currNode = currNode->reachingNode;
    }
    
    return finalPath;
}


/* findAngleBetweenSegs function
 * - finds the angle between seg1(in) and seg2(out), taking into account directions
 * - if no intersection or segments are part of the same street, will return a defined constant
 * 
 * @param street_segment1 <unsigned> - street segment ID of entrance to intersection
 * @param street_segment2 <unsigned> - street segment ID of exit from intersection
 * 
 * @return <double> - the angle between seg1(in) and seg2(out), or codes for same street/no intersection
 */

double findAngleBetweenSegs(unsigned street_segment1, unsigned street_segment2){
    InfoStreetSegment segInfo1, segInfo2;
    segInfo1 = getInfoStreetSegment(street_segment1);
    segInfo2 = getInfoStreetSegment(street_segment2);
    
    // same street id on both segs, same street
    if(segInfo1.streetID == segInfo2.streetID){
        return SAMESTREET;     
    }
    
    int numCurvePts1, numCurvePts2;
    
    numCurvePts1 = segInfo1.curvePointCount;
    numCurvePts2 = segInfo2.curvePointCount;
    LatLon ptFrom, ptCommon, ptTo;
    
    // all the possible combinations of point-point interactions
    // determines which direction traveling in the segs (from->to / to->from)
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
        return NOINTERSECTION;
    }
    
    return findAngleBetweenThreePoints(ptFrom, ptCommon, ptTo);    
}


/* findAngleBetweenThreePoints function
 * - finds the angle between ptFrom->ptCommon and ptCommon->ptTo
 * - calculates the angle in relation to the x axis of both entrance and exit segments
 * - finds the difference between the entrance and exit, representing turn angle
 * 
 * @param ptFrom <LatLon> - latlon position of the closest straight line point to intersection
 * @param ptCommon <LatLon> - latlon position of the intersection point
 * @param ptTo <LatLon> - latlon position of the closest straight line point from intersection
 * 
 * @return cAngle <double> - the angle between ptFrom->ptCommon and ptCommon->ptTo
 */

double findAngleBetweenThreePoints(LatLon ptFrom, LatLon ptCommon, LatLon ptTo){
    double aAngle, bAngle, cAngle;
    double aXSeg, aYSeg, bXSeg, bYSeg;

    // calculate the length of entrance and exit in x and y directions
    aXSeg = ptCommon.lon() - ptFrom.lon();
    aYSeg = ptCommon.lat() - ptFrom.lat();
    bXSeg = ptTo.lon() - ptCommon.lon();
    bYSeg = ptTo.lat() - ptCommon.lat();

    // calculate angle of entrance seg, exit seg, and find difference
    aAngle = atan2(aYSeg,aXSeg);
    bAngle = atan2(bYSeg,bXSeg);
    cAngle = aAngle - bAngle;
    
    return cAngle;
}


/* travelTimeAdd function
 * - finds the travel time of newSeg in addition to the turn time delay to get there
 * 
 * @param existingSeg <unsigned> - segment id for entrance to intersection
 * @param newSeg <unsigned> - segment id of exit to intersection (addition to path)
 * @param rt_penalty <double> - the time penalty to add for making a right turn
 * @param lt_penalty <double> - the time penalty to add for making a left turn
 * 
 * @return time <double> - the time to travel the extra segment
 */

double travelTimeAdd(unsigned existingSeg, unsigned newSeg, const double rt_penalty, const double lt_penalty){
    double time = 0;
    
    // add turn delay time (if any) to time between existing and new seg)
    if(static_cast<int>(existingSeg) != NOEDGE){
        TurnType turn = find_turn_type(existingSeg, newSeg);
        if(turn == TurnType::RIGHT){
            time = rt_penalty;
        } else if (turn == TurnType::LEFT){
            time = lt_penalty;
        }
    }
    
    // add travel time along the new segment
    time = time + find_street_segment_travel_time(newSeg);
    
    return time;
}