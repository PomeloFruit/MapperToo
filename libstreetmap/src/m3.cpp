#include "m1.h"
#include "m2.h"
#include "m3.h"

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

#define NOINTERSECTION -100
#define SAMESTREET -99

std::vector<unsigned> getPath(Node *sourceNode, const unsigned destID, const double rtPen, const double ltPen);

double travelTimeAdd(unsigned existingSeg, unsigned newSeg, const double rt_penalty, const double lt_penalty);

double findAngleBetweenSegs(unsigned street_segment1, unsigned street_segment2);

double findAngleBetweenThreePoints(LatLon ptFrom, LatLon ptCommon, LatLon ptTo);

double getNewScore(unsigned newPoint, LatLon end, double time);

std::vector<unsigned> getFinalPath(Node *currNode,unsigned start);

std::vector<std::string> pathToWords(std::vector<unsigned> path);




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
    Node start = Dir.Nodes[intersect_id_start];
    path = getPath(&start, intersect_id_end, right_turn_penalty, left_turn_penalty);
    return path;
}


// contains compare operator for sorting priority queue
// sorts queue using scores in waveElem
struct compareScore {
    bool operator()(const waveElem &a, const waveElem &b){
        return a.score > b.score;
    }
};

std::vector<unsigned> getPath(Node *sourceNode, const unsigned destID, 
                                            const double rtPen, const double ltPen) {
    std::priority_queue< waveElem, std::vector<waveElem>, compareScore> wavefront;
    std::vector< unsigned > changedNodes;
    wavefront.push(waveElem(NONODE, sourceNode, NOEDGE, 0, 0));
    
    LatLon end = Dir.intersectionPos[destID];
  
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
            
            // if at destination
            if(currNode->id == destID){
                
                //extract the path from the nodes
                std::vector<unsigned> path;
                path = getFinalPath(currNode,sourceNode->id);
                
                //reset all affected nodes values
                for(unsigned i=0 ; i<changedNodes.size() ; i++){
                    Node &temp = Dir.Nodes[changedNodes[i]];
                    temp.reachingNode = NULL;
                    temp.reachingEdge = NOEDGE;
                    temp.bestTime = NOTIME;
                }
                
                return path;
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
    
    // if nothing found, return empty vector
    return std::vector<unsigned>();
}


/* getNewScore function
 * - calculates the A* values for priority queue sorting
 * - adds time taken to reach Node + ETA to destination
 * - always underestimates the time needed to reach destination
 *      - ensures A* will always find fastest path
 *      - uses max KmH within city
 */
double getNewScore(unsigned newPoint, LatLon end, double time){
    LatLon start = Dir.intersectionPos[newPoint];
        
    double pointToEndDistance = find_distance_between_two_points(start, end);
   
    double estTimeToDest = pointToEndDistance * Dir.secPerMeter;
    
    double score = time + estTimeToDest;
    
    return score;
}

std::vector<unsigned> getFinalPath(Node *currNode, unsigned start){
    std::vector<unsigned> reversed;
   
    while(currNode!=NULL){
        if(static_cast<int> (currNode->reachingEdge) != NOEDGE){
            reversed.push_back(currNode->reachingEdge);

            if(static_cast<unsigned>(getInfoStreetSegment(currNode->reachingEdge).to) == start || 
                    static_cast<unsigned>(getInfoStreetSegment(currNode->reachingEdge).from) == start){
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


TurnType find_turn_type(unsigned street_segment1, unsigned street_segment2){
   
    double angle = findAngleBetweenSegs(street_segment1, street_segment2);
    
    // if turn angle -PI < dAngle < 0 or if dAngle > PI
    if(angle == SAMESTREET){
        return TurnType::STRAIGHT;        
    } else if(angle == NOINTERSECTION){
        return TurnType::NONE;     
    } else if((angle < 0 && angle > -M_PI)|| angle > M_PI){ 
        return TurnType::LEFT;
    } else {
        return TurnType::RIGHT;
    }
}

double findAngleBetweenSegs(unsigned street_segment1, unsigned street_segment2){
    InfoStreetSegment segInfo1, segInfo2;
    segInfo1 = getInfoStreetSegment(street_segment1);
    segInfo2 = getInfoStreetSegment(street_segment2);
    
    if(segInfo1.streetID == segInfo2.streetID){
        return SAMESTREET;     
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
        return NOINTERSECTION;
    }
    
    return findAngleBetweenThreePoints(ptFrom, ptCommon, ptTo);    
}

double findAngleBetweenThreePoints(LatLon ptFrom, LatLon ptCommon, LatLon ptTo){
    double aAngle, bAngle, cAngle;
    double aXSeg, aYSeg, bXSeg, bYSeg;

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
    
    if(static_cast<int>(existingSeg) != NOEDGE){
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


//for now I'm just going to make dAngle a global since I don't really want to write a shitload of new code for shit
//but that means when I push this the code here will not work because I'm going to un-make it a global
//as I think it would be better to consult the team on what to do in this situation
//it seems prudent to split that turn function into maybe 2 anyways
//but I'm not just gonna do that without asking everyone
//fuck even this should be split
/*
std::vector<std::string> pathToWords(std::vector<unsigned> path){
    std::vector<std::string> theGospel;
    if(path.size()==0){
        std::cout<<"NO PATH FOUND"<<'\n';
        return theGospel;
    }
    const double NOTURN=0.174533;
    const double SMALLTURN=0.698132;
    int distance=find_street_segment_length(path[0]);//showing people doubles looks bad
    InfoStreetSegment segInfoPrev;
    InfoStreetSegment segInfoCur;
    std::string toBeInserted="";
    if(path.size()>1){
        for(int i=1;i<path.size();i++){
            std::string toBeInserted="";
            segInfoPrev = getInfoStreetSegment(path[i-1]);
            segInfoCur = getInfoStreetSegment(path[i]);
//            std::cout<<"GETTING NAMES"<<'\n';
//////////////////////            std::string newStreetName=getStreetName(segInfoCur.streetID);
//////////////////////            std::string oldStreetName=getStreetName(segInfoPrev.streetID);
//            std::cout<<"GOT NAMES"<<'\n';
            std::string newStreetName="gee";
            std::string oldStreetName="whiz";
            //if I make a turn:
            if(segInfoPrev.streetID!=segInfoCur.streetID){
                if(distance<=1){
                    TurnType turn=find_turn_type(path[i-1], path[i]);
                    //I know the turn type and the abs angle
                    if(angleSegs>M_PI){
                        angleSegs=angleSegs-M_PI;
                    }

                    if(angleSegs<NOTURN){
                        toBeInserted="Continue straight onto "+newStreetName;
                        theGospel.push_back(toBeInserted);
                        toBeInserted="";
                    }
                    else if(angleSegs<SMALLTURN){
                        if(turn==TurnType::RIGHT){
                            toBeInserted="Slight right onto "+newStreetName;
                            theGospel.push_back(toBeInserted);
                            toBeInserted="";
                        }
                        else{
                            toBeInserted="Slight left onto "+newStreetName;
                            theGospel.push_back(toBeInserted);
                            toBeInserted="";
                        }
                    }
                    else{
                        if(turn==TurnType::RIGHT){
                            toBeInserted="Right onto "+newStreetName;
                            theGospel.push_back(toBeInserted);
                            toBeInserted="";
                        }
                        else{
                            toBeInserted="Left onto "+newStreetName;
                            theGospel.push_back(toBeInserted);
                            toBeInserted="";
                        }
                    }  
                }
                else{
                    toBeInserted="Go straight "+std::to_string(distance)+" meters on "+oldStreetName;
                    theGospel.push_back(toBeInserted);
                    toBeInserted="";
                    TurnType turn=find_turn_type(path[i-1], path[i]);
                    //I know the turn type and the abs angle
                    if(angleSegs>M_PI){
                        angleSegs=angleSegs-M_PI;
                    }

                    if(angleSegs<NOTURN){
                        toBeInserted="Continue straight onto "+newStreetName;
                        theGospel.push_back(toBeInserted);
                        toBeInserted="";
                    }
                    else if(angleSegs<SMALLTURN){
                        if(turn==TurnType::RIGHT){
                            toBeInserted="Slight right onto "+newStreetName;
                            theGospel.push_back(toBeInserted);
                            toBeInserted="";
                        }
                        else{
                            toBeInserted="Slight left onto "+newStreetName;
                            theGospel.push_back(toBeInserted);
                            toBeInserted="";
                        }
                    }
                    else{
                        if(turn==TurnType::RIGHT){
                            toBeInserted="Right onto "+newStreetName;
                            theGospel.push_back(toBeInserted);
                            toBeInserted="";
                        }
                        else{
                            toBeInserted="Left onto "+newStreetName;
                            theGospel.push_back(toBeInserted);
                            toBeInserted="";
                        }
                    }
                }
            }
            else{
                distance=distance+find_street_segment_length(path[i]);
            }
        }
    }
    else{
        toBeInserted="Go straight "+std::to_string(distance)+" meters on "+"AHH I NEED HELP HERE";
        theGospel.push_back(toBeInserted);
    }
        
//    std::cout<<"PRINTING NAMES"<<'\n';
    for(int i=0;i<theGospel.size();i++){
        std::string toBePrinted=theGospel[i]+'\n';
        std::cout<<toBePrinted;
    }
    std::cout<<"DONE"<<'\n';
    
    return theGospel;
    //////////////////////    angleSegs=abs(dAngle);
}
*/