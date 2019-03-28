#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m4.h"
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

struct pathTime {
    std::vector<unsigned> path;
    double time;
};

#define PICKUP 0
#define DROPOFF 1
#define DEPOT 2

// ==================================================================================
void fillAllPathTimes(std::vector<std::vector<pathTime>>& pathTimes,
                      std::vector<std::vector<pathTime>>& depotTimes,
                      const std::vector<DeliveryInfo>& deliveries,
                      const  std::vector<unsigned>& depots,
                      const double right_turn_penalty,
                      const double left_turn_penalty);

void multiDestPath(Node *sourceNode, const std::vector<unsigned>& allDest, 
        std::vector<pathTime>& pathTimes, const double rtPen, const double ltPen);

// ==================================================================================

std::vector<CourierSubpath> traveling_courier(
		const std::vector<DeliveryInfo>& deliveries,
	       	const std::vector<unsigned>& depots, 
		const float right_turn_penalty, 
		const float left_turn_penalty, 
		const float truck_capacity){
    
    std::vector<CourierSubpath> courierPath;

    unsigned bestInter1;
    unsigned bestInter2;
    std::vector<unsigned> bestInts;
    std::vector<unsigned> intTypes; //0 for pickup, 1 for dropoff, 2 for depot
    bestInts.clear();
    intTypes.clear();
    
    double remainingWeight = truck_capacity;
    double minTime;
    
    unsigned numDeliveries = deliveries.size();
    unsigned numDepots = depots.size();
    unsigned totDest = 2*numDeliveries + numDepots;
    
    std::vector<bool> picked;
    std::vector<bool> dropped;
    picked.resize(numDeliveries);
    dropped.resize(numDeliveries);
    unsigned numPicked = 0;
    unsigned numDropped = 0;
    
    std::vector<std::vector<pathTime>> pathTimes;
    std::vector<std::vector<pathTime>> depotTimes;
    pathTimes.resize(2*numDeliveries);
    depotTimes.resize(numDepots);
       
    // get all path / time from start-dest combinations ===========================================
    
    for(unsigned i=0 ; i<pathTimes.size() ; i++){
        pathTimes[i].resize(totDest);
    }
    
    for(unsigned i=0 ; i<depotTimes.size() ; i++){
        depotTimes[i].resize(numDeliveries);
    }
    
    fillAllPathTimes(pathTimes, depotTimes, deliveries, depots, right_turn_penalty, left_turn_penalty);

    // get best depot to delivery pickup ===========================================================
    
    minTime = 99999999999999999;
    
    for(unsigned i=0 ; i<depotTimes.size(); i++){
        for(unsigned j=0 ; j<depotTimes[i].size(); j++){
            if(depotTimes[i][j].time < minTime && depotTimes[i][j].time != 0){
                minTime = depotTimes[i][j].time;
                bestInter1 = i; //depot
                bestInter2 = j; //delivery pickup
            }
        }
        
    }
    
    // account for pickup dropoff 2x
    bestInter2 = bestInter2 * 2;
    bestInts.push_back(bestInter1);
    intTypes.push_back(DEPOT);
    bestInts.push_back(bestInter2);
    intTypes.push_back(PICKUP);
    picked[bestInter2/2] = true;
    remainingWeight =  remainingWeight - deliveries[bestInter2/2].itemWeight;
    numPicked++;
   // std::cout << bestInter2 << " at " << minTime << std::endl;
        
    // do all the deliveries ===========================================================
    while(numPicked < numDeliveries || numDropped < numDeliveries){
        bool isCurrentPickUp;
        minTime = 99999999999999999;
        bestInter1 = bestInter2;
        
        // get best pickup/dropoff anything combination
        for(unsigned j=0 ; j<pathTimes.size(); j++){
            
            if(pathTimes[bestInter1][j].time < minTime){
                if(j%2 == 0){ //pickup
                    if(!picked[j/2] && (remainingWeight-deliveries[j/2].itemWeight > 0)){
                        isCurrentPickUp = true;
                    } else {
                        continue;
                    }
                } else { //dropoff = 1
                    if(!dropped[j/2] && picked[j/2]){
                        isCurrentPickUp = false;
                    } else {
                        continue;
                    }
                }
                
                minTime = pathTimes[bestInter1][j].time;
                bestInter2 = j; //nextStop
            }
        }

        bestInts.push_back(bestInter2);
     //   std::cout << bestInter2 << " at " << minTime << std::endl;
        
        if(isCurrentPickUp){
            remainingWeight = remainingWeight - deliveries[bestInter2/2].itemWeight;
            intTypes.push_back(PICKUP);
            picked[bestInter2/2] = true;
            numPicked++;
        } else {
            remainingWeight = remainingWeight + deliveries[bestInter2/2].itemWeight;
            intTypes.push_back(DROPOFF);
            dropped[bestInter2/2] = true;
            numDropped++;
        }
    }

    // find the closest end depot ===========================================================
    
    bestInter1 = bestInter2;
    minTime = 99999999999999999;
    
    for(unsigned j=pathTimes.size() ; j<pathTimes[bestInter1].size(); j++){
        if(pathTimes[bestInter1][j].time < minTime && pathTimes[bestInter1][j].time != 0){
            minTime = pathTimes[bestInter1][j].time;
            bestInter2 = j; //depot
        }
    }
    
    bestInts.push_back(bestInter2);
    intTypes.push_back(DEPOT);

    // get the courier path now
    
   // std::cout << "---------------------------------------------------\n";
    //std::cout << numDeliveries << " deliveries and depots: " << numDepots << std::endl;
    
    for(unsigned i=0; i<bestInts.size()-1; i++){
        CourierSubpath tempSubpath;
        tempSubpath.pickUp_indices.clear();
         
        if(intTypes[i] == DEPOT){
         //   std::cout << "1depot #" << bestInts[i] << std::endl;
            tempSubpath.start_intersection = depots[bestInts[i]];
            tempSubpath.subpath = depotTimes[bestInts[i]][bestInts[i+1]/2].path;
            
        } else if(intTypes[i] == PICKUP){
           // std::cout << "1pickup #" << bestInts[i] << std::endl;
            tempSubpath.start_intersection = deliveries[bestInts[i]/2].pickUp;
            
            tempSubpath.pickUp_indices.push_back(bestInts[i]/2);
//            unsigned next = i+1;
//            unsigned jumpBy = 0;
//            while(bestInts[i] == bestInts[next]){
//                tempSubpath.pickUp_indices.push_back(bestInts[next]/2);
//                jumpBy++;
//                next += jumpBy;
//            }
//            i = i+jumpBy;
            
            tempSubpath.subpath = pathTimes[bestInts[i]][bestInts[i+1]].path;
            
        } else { //dropoff
         //   std::cout << "1dropoff #" << bestInts[i] << std::endl;
            tempSubpath.start_intersection = deliveries[bestInts[i]/2].dropOff;
            tempSubpath.subpath = pathTimes[bestInts[i]][bestInts[i+1]].path;

        }
        
        // set the end intersection locations 
        
        if(intTypes[i+1] == DEPOT){
         //   std::cout << "2depot #" << bestInts[i+1] << std::endl;
            tempSubpath.end_intersection = depots[bestInts[i+1]-2*numDeliveries];
        } else if(intTypes[i+1] == PICKUP){
         //   std::cout << "2pickup #" << bestInts[i+1] << std::endl;
            tempSubpath.end_intersection = deliveries[bestInts[i+1]/2].pickUp;
        } else {
          //  std::cout << "2dropoff #" << bestInts[i+1] << std::endl;
            tempSubpath.end_intersection = deliveries[bestInts[i+1]/2].dropOff;
        }
        
      //  std::cout << tempSubpath.start_intersection << " to " << tempSubpath.end_intersection << "with size " << tempSubpath.subpath.size() << std::endl;
        
        courierPath.push_back(tempSubpath);
    }
    
    bestInts.clear();
    intTypes.clear();
    pathTimes.clear();
    depotTimes.clear();
    picked.clear();
    dropped.clear();
    
    return courierPath;
}


    
void fillAllPathTimes(std::vector<std::vector<pathTime>>& pathTimes,
                      std::vector<std::vector<pathTime>>& depotTimes,
                      const std::vector<DeliveryInfo>& deliveries,
                      const std::vector<unsigned>& depots,
                      const double right_turn_penalty,
                      const double left_turn_penalty){
    
    
    std::vector<unsigned> allDest;
    std::vector<unsigned> allStart;
    
    for(unsigned i=0; i<deliveries.size() ; i++){
        allStart.push_back(deliveries[i].pickUp);
        allDest.push_back(deliveries[i].pickUp);
        allDest.push_back(deliveries[i].dropOff);
    }
    
    for(unsigned i=0; i<depots.size() ; i++){
        allDest.push_back(depots[i]);
    }
    
    #pragma omp parallel for
    for(unsigned i=0; i<(deliveries.size()*2) ; i++){       
        pathTimes[i][i].path.clear();
        pathTimes[i][i].time = 0;
        Node start = Dir.Nodes[allDest[i]];
        multiDestPath(&start, allDest, pathTimes[i], right_turn_penalty,left_turn_penalty);
    }
    
    #pragma omp parallel for
    for(unsigned i=0; i<depots.size() ; i++){
        Node start = Dir.Nodes[depots[i]];
        multiDestPath(&start, allStart, depotTimes[i], right_turn_penalty, left_turn_penalty);
    }
}

// structure contains compare operator for sorting priority queue
// sorts queue using scores in waveElem such that lowest score is placed at front of queue
struct compareTime {
    bool operator()(const waveElem4 &a, const waveElem4 &b){
        return a.travelTime > b.travelTime;
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

void multiDestPath(Node *sourceNode, const std::vector<unsigned>& allDest, 
        std::vector<pathTime>& pathTimes, const double rtPen, const double ltPen) {
    
    std::priority_queue< waveElem4 , std::vector<waveElem4 >, compareTime> wavefront;
    wavefront.push(waveElem4(NONODE, sourceNode, NOEDGE, 0));
     
    int numMatched = 0;
    std::vector<unsigned> reachingEdge;
    std::vector<double> bestTime;
    std::vector<Node *> reachingNode;
    
    reachingEdge.resize(Dir.Nodes.size());
    reachingNode.resize(Dir.Nodes.size());
    bestTime.resize(Dir.Nodes.size());
    std::fill(bestTime.begin(), bestTime.end(), NOTIME);
    
   // std::cout << "dest size  " << allDest.size() << std::endl;
      
    // repeat until all possible paths are checked or path is found
    while(!wavefront.empty()){
        // get next intersection node
        waveElem4 wave = wavefront.top();
        wavefront.pop();
        
        Node *currNode = wave.node;

        // if best path from this wave
        if(wave.travelTime < bestTime[currNode->id]){
            
            // if this was better path to node, update
            reachingEdge[currNode->id] = wave.edgeID;
            bestTime[currNode->id] = wave.travelTime;
            if(static_cast<int> (wave.reachingNode) != NONODE){
                reachingNode[currNode->id] = &(Dir.Nodes[wave.reachingNode]);
            } else {
                reachingNode[currNode->id] = NULL;
            }
            
            // if at destination, get the path and exit loop
            for(unsigned i=0; i<allDest.size(); i++){
                if(currNode->id == allDest[i]){
                    //extract the path from the nodes
                    pathTimes[i].path = getFinalPath4(currNode,sourceNode->id, reachingEdge, reachingNode);
                    pathTimes[i].time = bestTime[currNode->id];
                    numMatched = numMatched + 1;
                }
            }
            
            if(numMatched == allDest.size()){
                break;
            }
        
            // add all nodes that can be reached to wavefront
            for(unsigned i=0 ; i < currNode->outEdges.size(); i++){
                Node* toNode = currNode->toNodes[i];
                unsigned toEdge = currNode->outEdges[i];
                
                // if same segment as current
                if(wave.edgeID == toEdge){
                    continue;
                }

                // calculate score and time for the waveElem
                double toNodeTime;
                toNodeTime = bestTime[currNode->id] + travelTimeAdd(wave.edgeID,toEdge,rtPen,ltPen);
                
                // add node to wavefront
                wavefront.push(waveElem4(currNode->id, toNode, toEdge, toNodeTime));
            }
        }
    }
    reachingEdge.clear();
    bestTime.clear();
    reachingNode.clear();
}