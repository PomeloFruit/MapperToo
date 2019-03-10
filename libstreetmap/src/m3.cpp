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
#include <queue>
#include <string>
#include <limits>
#include <iostream>

std::vector<unsigned> bfsPath(Node *sourceNode, const unsigned destID, const double rtPen, const double ltPen);

double travelTimeAdd(unsigned existingSeg, unsigned newSeg, const double rt_penalty, 
                                                    const double lt_penalty);
std::vector<unsigned> getFinalPath(Node *currNode,unsigned start);


//std::vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start, 
//                                                    const unsigned intersect_id_end, 
//                                                    const double right_turn_penalty, 
//                                                    const double left_turn_penalty){
//    
//    return std::vector<unsigned>();
//}


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
    std::cout << "-=---3-3-3--3-3-3-33-3--3-3-3-3-3--33" << std::endl;
    std::cout << "start " << intersect_id_start << "end" << intersect_id_end << std::endl;
    std::cout << intersect_id_start << "  " << intersect_id_end << " " <<  compute_path_travel_time(path, right_turn_penalty, 
                                                                    left_turn_penalty) <<  std::endl;
    path = bfsPath(&start, intersect_id_end, right_turn_penalty, left_turn_penalty);
    
    return path;
}

std::vector<unsigned> bfsPath(Node *sourceNode, const unsigned destID, 
                                            const double rtPen, const double ltPen) {
    std::list< waveElem > wavefront;

    std::vector< unsigned > changedNodes;
    wavefront.push_back(waveElem(NONODE, sourceNode, NOEDGE, 0));
    int minCycles = sourceNode->outEdges.size();
    int numCycles = 0;
    
    while(!wavefront.empty()){
        numCycles++;
        if(destID == 1025){
             std::cout << "before start " << wavefront.size() << std::endl;
        }
        // get next intersection node
        waveElem wave = wavefront.front();
        wavefront.pop_front();
        //wave.node->reachingEdge = wave.edgeID;
        
        //std::cout << "start " << wavefront.size() << std::endl;
        
        Node *currNode = wave.node;
         if(destID == 1025){
             std::cout << currNode->id << std::endl;
        }
      //  std::cout << currNode->id << std::endl;
        // if best path from this wave or if wave is at start
        if(wave.travelTime < currNode->bestTime){
            // if this was better path to node, update
             if(destID == 1025){
                 std::cout << "pushed " << currNode->id << " using edge " << wave.edgeID << std::endl;
                 std::cout << "before edge " << currNode->reachingEdge << "after "<< wave.edgeID << std::endl;
                 std::cout << "before time " << currNode->bestTime << "after "<< wave.travelTime << std::endl;
            }
            
            
            currNode->reachingEdge = wave.edgeID;
            currNode->bestTime = wave.travelTime;
            if(wave.reachingNode != NONODE){
                currNode->reachingNode = &(dir.Nodes[wave.reachingNode]);
            }
            changedNodes.push_back(currNode->id);
        }
                
        // found the end
        if((numCycles >= minCycles) && (currNode->id == destID)){
            // std::cout << "========================================" << std::endl;
           // std::cout << "found " << std::endl;
            if(destID == 1025){
             std::cout << currNode->id << std::endl;
        }
            
            std::vector<unsigned> path;
            path = getFinalPath(currNode,sourceNode->id);
            for(unsigned i=0 ; i<changedNodes.size() ; i++){
                Node &temp = dir.Nodes[changedNodes[i]];
                temp.reachingNode = NULL;
                temp.reachingEdge = NOEDGE;
                temp.bestTime = NOTIME;
            }
            return path;
        }
        
        //std::cout << currNode->outEdges.size() << std::endl;
        for(unsigned i=0 ; i < currNode->outEdges.size(); i++){
           
            //std::cout << "hi ";
            
            Node *toNode = currNode->toNodes[i];
            unsigned toEdge = currNode->outEdges[i];
            if(destID == 1025){
             std::cout << wave.edgeID <<  "..to node ... " <<  currNode->toNodes[i]->id << "..to edge.." << toEdge << std::endl;
        }
            

            if(wave.edgeID == toEdge){
                //std::cout << "error " << std::endl;
                continue;
            }
            
            double toNodeTime;
            toNodeTime = currNode->bestTime + travelTimeAdd(wave.edgeID,toEdge,rtPen, ltPen);
            wavefront.push_back(waveElem(currNode->id, toNode, toEdge, toNodeTime));
            //std::cout << "not skipped " << std::endl;
        }
        
       // std::cout << "end " << wavefront.size() << std::endl;

    }
    std::cout << "not found" << std::endl;
    return std::vector<unsigned>();
}

std::vector<unsigned> getFinalPath(Node *currNode, unsigned start){
   // std::cout << "====="<<std::endl;
    std::vector<unsigned> reversed;
    //std::cout << "find path " << std::endl;
  //  std::cout << getIntersectionName(currNode->id) << std::endl;
    while(currNode!=NULL){
        if(currNode->reachingEdge != NOEDGE){
            reversed.push_back(currNode->reachingEdge);
            
            if(getInfoStreetSegment(currNode->reachingEdge).oneWay){
                ///std::cout << "wowowowieeee" <<std::endl;
            }
            
            
            if(reversed.size() < 100){

                std::cout << "from " << getInfoStreetSegment(currNode->reachingEdge).from << 
                        "to " << getInfoStreetSegment(currNode->reachingEdge).to <<
                        " using " << currNode->reachingEdge << std::endl;
            }
            if(getInfoStreetSegment(currNode->reachingEdge).to == start || getInfoStreetSegment(currNode->reachingEdge).from == start){
                std::cout << "reached finish" <<std::endl;
                break;
            }
        }
        

       // std::cout << "taking seg " << currNode->reachingEdge << ".to get to ." << currNode->id << std::endl;
        currNode = currNode->reachingNode;
        if(currNode != NULL){
      //      std::cout << "====taking seg " << currNode->reachingEdge << ".to get to ." << currNode->id << std::endl;
        }
    }
    //std::cout << "completed" << reversed.size() << std::endl;
    std::vector<unsigned> proper;
    for(int i=reversed.size()-1; i>=0; i--){
        proper.push_back(reversed[i]);
      //  std::cout << reversed[i] << std::endl;
    }
    return proper;
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