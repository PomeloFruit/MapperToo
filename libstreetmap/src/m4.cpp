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
#include <chrono>

struct pathTime {
    std::vector<unsigned> path;
    double time;
};

struct multiStruct {
    double courierTime;
    std::vector<double> timePerSub;
    std::vector<unsigned> bestInts;
    std::vector<unsigned> intTypes; //0 for pickup, 1 for dropoff, 2 for depot
    std::vector<unsigned> pickUpIndex;
    std::vector<unsigned> dropOffIndex; // in what part of the route do we drop off the package
    std::vector<double> remWeightHere;
};

#define TIME_LIMIT 45
#define CHICKEN 0.99
#define SCARED 0.7

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

void multiDestPath(Node *sourceNode, 
                   const std::vector<unsigned>& allDest, 
                   std::vector<pathTime>& pathTimes,
                   const double rtPen,
                   const double ltPen);

void opt_k_Swap(multiStruct &temp, 
                const unsigned size,
                const unsigned len,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<DeliveryInfo>& deliveries);

void swap(unsigned &a, unsigned &b);

void opt_k_Rotate(multiStruct &temp, 
                const unsigned size,
                const unsigned len,
                const unsigned ror,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<DeliveryInfo>& deliveries);

void opt_k_Checks(multiStruct &temp,
                  multiStruct &testNew,  
                  const unsigned start,
                  const unsigned len, 
                  const std::vector<std::vector<pathTime>>& pathTimes,
                  const std::vector<DeliveryInfo>& deliveries);

multiStruct multiStart(const unsigned numDeliveries, 
                       const unsigned startDepot,
                       const std::vector<std::vector<pathTime>>& pathTimes, 
                       const std::vector<std::vector<pathTime>>& depotTimes,
                       const std::vector<DeliveryInfo>& deliveries,
                       const double truckCap);

double addSubPathTimes(std::vector<double> times);

// ==================================================================================

std::vector<CourierSubpath> traveling_courier(
		const std::vector<DeliveryInfo>& deliveries,
	       	const std::vector<unsigned>& depots, 
		const float right_turn_penalty, 
		const float left_turn_penalty, 
		const float truck_capacity){
    
    auto startTime = std::chrono::high_resolution_clock::now();
   
    std::vector<CourierSubpath> courierPath;

    unsigned numDeliveries = deliveries.size();
    unsigned numDepots = depots.size();
    unsigned totDest = 2*numDeliveries + numDepots;
        
    std::vector<std::vector<pathTime>> pathTimes; // from pickup/dropoff to pickup/dropoff/depot
    std::vector<std::vector<pathTime>> depotTimes; // from all depots to all pickups
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
    
    // do multistart and save the best one ========================================================

    multiStruct bestCI;
    double bestCourier = NOTIME;
    std::vector< multiStruct > tempStarts;
    tempStarts.resize(numDepots);
    
    // this is slow so lets do all this computation parallel
    #pragma omp parallel for
    for(unsigned i=0; i<numDepots; i++){
        tempStarts[i] = multiStart(numDeliveries, i, pathTimes, depotTimes, deliveries, truck_capacity);
    }
    
    // do k-opt stuff ================================================================================
    // this is a multi start k-opt kinda?
    
    unsigned bestIndex = 0;
    // find best result
    for(unsigned i=0; i<numDepots; i++){
        if(tempStarts[i].courierTime < bestCourier){
            bestIndex = i;
            bestCourier = tempStarts[i].courierTime;
        }
    }
        
    multiStruct betterPath = tempStarts[bestIndex];
    double kOpt = betterPath.bestInts.size();
    double maxIter = 50;

    bool timeOut = false;
    bool scared = false;
    bool somethingChanged = true;
    unsigned numIter = 5;
    auto prevTime = std::chrono::high_resolution_clock::now();

    for(unsigned z = 0; z < maxIter && !timeOut; z++){
        if(!somethingChanged && !scared){
            numIter += numIter;
        } else if(!somethingChanged){
            numIter += 1;
        }

        if(numIter>betterPath.bestInts.size()){
            break;
        }

        ///////////////////////////////////////////////////////////////////// TODO: make it so u can change the depot->pickup route
        
        somethingChanged = false;
        //std::cout << z << " " << std::endl;
        for(unsigned k = 1; k < kOpt && !timeOut; k++){
            for(unsigned i=2; betterPath.bestInts.size() > (k+1) && i< betterPath.bestInts.size()-k-1 && !timeOut; i++){

                std::vector<multiStruct> pathToTry;
                pathToTry.resize(numIter);

                #pragma omp parallel for
                for(unsigned d=0; d<numIter; d++){
                    pathToTry[d] = betterPath;
                }

                #pragma omp parallel for
                for(unsigned d=0; d<numIter; d++){
                    multiStruct temp = pathToTry[d];
                    opt_k_Rotate(temp, i, k, d, pathTimes, deliveries);
                    if(temp.courierTime < pathToTry[d].courierTime){
                        pathToTry[d] = temp;
                    }

                    multiStruct beforeSwap = pathToTry[d];

                    #pragma omp parallel for
                    for(unsigned f=1; f<k; f++){
                        multiStruct temp1 = beforeSwap;
                        opt_k_Swap(temp1, i, f, pathTimes, deliveries);
                        if(temp1.courierTime < pathToTry[d].courierTime){
                            pathToTry[d] = temp1;
                        }
                    }
                }

                for(unsigned d=0; d<numIter; d++){
                    if(pathToTry[d].courierTime < betterPath.courierTime){
                        betterPath = pathToTry[d];
                        somethingChanged = true;
                    }
                }               

                auto currentTime = std::chrono::high_resolution_clock::now();
                auto diffClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-prevTime);
                auto wallClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-startTime);
                double timeElapsed = wallClock.count();
                double timeForLast = diffClock.count();

                prevTime = currentTime;

                if((TIME_LIMIT*CHICKEN - timeElapsed) < timeForLast){
                    std::cout << "exit @ try 1-" << z << " try 2-" << k << " try 3-" << i << " time " << timeForLast << " " << timeElapsed << " " << TIME_LIMIT - timeElapsed << std::endl;
                    timeOut = true;
                } else if(timeElapsed > SCARED*TIME_LIMIT){
                    scared = true;
                }
            }
        }
    }

    bestCI = betterPath;
    
    tempStarts.clear();
    
    for(unsigned i=0; i<bestCI.bestInts.size()-1 && !bestCI.bestInts.empty(); i++){
        CourierSubpath tempSubpath;
        tempSubpath.pickUp_indices.clear();
         
        if(bestCI.intTypes[i] == DEPOT){
            tempSubpath.start_intersection = depots[bestCI.bestInts[i]];
            tempSubpath.subpath = depotTimes[bestCI.bestInts[i]][bestCI.bestInts[i+1]/2].path;
            
        } else if(bestCI.intTypes[i] == PICKUP){
            tempSubpath.start_intersection = deliveries[bestCI.bestInts[i]/2].pickUp;       
            tempSubpath.pickUp_indices.push_back(bestCI.bestInts[i]/2);
            tempSubpath.subpath = pathTimes[bestCI.bestInts[i]][bestCI.bestInts[i+1]].path;
            
        } else { //dropoff
            tempSubpath.start_intersection = deliveries[bestCI.bestInts[i]/2].dropOff;
            tempSubpath.subpath = pathTimes[bestCI.bestInts[i]][bestCI.bestInts[i+1]].path;
        }
        
        // set the end intersection locations 
        
        if(bestCI.intTypes[i+1] == DEPOT){
            tempSubpath.end_intersection = depots[bestCI.bestInts[i+1]-2*numDeliveries];
        } else if(bestCI.intTypes[i+1] == PICKUP){
            tempSubpath.end_intersection = deliveries[bestCI.bestInts[i+1]/2].pickUp;
        } else {
            tempSubpath.end_intersection = deliveries[bestCI.bestInts[i+1]/2].dropOff;
        }
        
//        std::cout << i << " " << tempSubpath.start_intersection << " " << tempSubpath.end_intersection << std::endl;
                
        courierPath.push_back(tempSubpath);
    }
    
    bestCI.bestInts.clear();
    bestCI.intTypes.clear();
    pathTimes.clear();
    depotTimes.clear();
    
    return courierPath;
}

// ==========================================================================================================================

void opt_k_Swap(multiStruct &temp, 
                const unsigned start,
                const unsigned len, 
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<DeliveryInfo>& deliveries){
    
    multiStruct testNew = temp;

    swap(testNew.intTypes[start], testNew.intTypes[start+len]);
    swap(testNew.bestInts[start], testNew.bestInts[start+len]);
    
    opt_k_Checks(temp, testNew, start, len, pathTimes, deliveries);
}

void swap(unsigned &a, unsigned &b){
    unsigned temp = a;
    a = b;
    b = temp;
}

void opt_k_Rotate(multiStruct &temp, 
                const unsigned start,
                const unsigned len,
                const unsigned ror,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<DeliveryInfo>& deliveries){
    
    multiStruct testNew = temp;

    for(unsigned r=0; r<ror; r++){
        for(unsigned i=start; i<(start+len); i++){
            swap(testNew.intTypes[i], testNew.intTypes[i+1]);
            swap(testNew.bestInts[i], testNew.bestInts[i+1]);
        }
    }
      
    opt_k_Checks(temp, testNew, start, len, pathTimes, deliveries);
}


void opt_k_Checks(multiStruct &temp,
                  multiStruct &testNew,  
                  const unsigned start,
                  const unsigned len, 
                  const std::vector<std::vector<pathTime>>& pathTimes,
                  const std::vector<DeliveryInfo>& deliveries){
    
    for(unsigned i = start; i<=start+len; i++){
        if(testNew.intTypes[i] == PICKUP){
            testNew.pickUpIndex[testNew.bestInts[i]/2] = i;
            testNew.remWeightHere[i] = testNew.remWeightHere[i-1]-deliveries[testNew.bestInts[i]/2].itemWeight;
        } else {
            testNew.dropOffIndex[testNew.bestInts[i]/2] = i;
            testNew.remWeightHere[i] = testNew.remWeightHere[i-1]+deliveries[testNew.bestInts[i]/2].itemWeight;
        }

        // exceeds truck capacity
        if(testNew.remWeightHere[i] < 0){
            testNew = temp;
            return;
        }
    }

    for(unsigned i = start; i<=start+len; i++){
        if(testNew.pickUpIndex[testNew.bestInts[i]/2] > testNew.dropOffIndex[testNew.bestInts[i]/2]){
            testNew = temp;
            return;
        }
    }

    double oldTime = 0;
    double newTime = 0;
    for(unsigned i = start-1; i<=start+len; i++){
        oldTime = oldTime + testNew.timePerSub[i];
        testNew.timePerSub[i] = pathTimes[testNew.bestInts[i]][testNew.bestInts[i+1]].time;
        newTime = newTime + testNew.timePerSub[i];
    }

    double deltaT = newTime - oldTime;
    if(deltaT < 0){
        testNew.courierTime = testNew.courierTime + deltaT;
        temp = testNew;
    }
    
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


multiStruct multiStart(const unsigned numDeliveries, 
                       const unsigned startDepot,
                       const std::vector<std::vector<pathTime>>& pathTimes, 
                       const std::vector<std::vector<pathTime>>& depotTimes,
                       const std::vector<DeliveryInfo>& deliveries,
                       const double truckCap){
    
    multiStruct route;
    
    unsigned bestInter1;
    unsigned bestInter2;
    unsigned subPathNum = 0;
    
    route.bestInts.clear();
    route.intTypes.clear();
    route.timePerSub.clear();
    route.remWeightHere.clear();
    route.pickUpIndex.resize(numDeliveries);
    route.dropOffIndex.resize(numDeliveries);
    
    
    double remainingWeight = truckCap;
    double minTime;
    
    std::vector<bool> picked;
    std::vector<bool> dropped;
    
    picked.resize(numDeliveries);
    dropped.resize(numDeliveries);
    
    unsigned numPicked = 0;
    unsigned numDropped = 0;
    
    // get best depot to delivery pickup ===========================================================
    
    minTime = NOTIME;
    
    bestInter1 = startDepot;
    
    for(unsigned j=0 ; j<depotTimes[startDepot].size(); j++){
        if(depotTimes[startDepot][j].time < minTime){
            minTime = depotTimes[startDepot][j].time;
            bestInter2 = j; //delivery pickup
        }
    }
    
    // no path from depot to any pickup
    if(minTime == NOTIME){
        route.courierTime = NOTIME;
        return route;
    }
    
    // account for pickup dropoff 2x
    bestInter2 = bestInter2 * 2;
    route.bestInts.push_back(bestInter1);
    route.intTypes.push_back(DEPOT);
    route.bestInts.push_back(bestInter2);
    route.intTypes.push_back(PICKUP);
    route.remWeightHere.push_back(remainingWeight);
    
    picked[bestInter2/2] = true;
    remainingWeight =  remainingWeight - deliveries[bestInter2/2].itemWeight;
    route.timePerSub.push_back(minTime);
    route.pickUpIndex[bestInter2/2] = subPathNum;
    numPicked++;
    subPathNum++;
        
    // do all the deliveries ===========================================================
    
    while(numPicked < numDeliveries || numDropped < numDeliveries){
        bool isCurrentPickUp = false;
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

        route.bestInts.push_back(bestInter2);
        route.timePerSub.push_back(minTime);
        route.remWeightHere.push_back(remainingWeight);
        
        if(isCurrentPickUp){
            remainingWeight = remainingWeight - deliveries[bestInter2/2].itemWeight;
            route.intTypes.push_back(PICKUP);
            route.pickUpIndex[bestInter2/2] = subPathNum;
            picked[bestInter2/2] = true;
            numPicked++;
        } else {
            remainingWeight = remainingWeight + deliveries[bestInter2/2].itemWeight;
            route.intTypes.push_back(DROPOFF);
            route.dropOffIndex[bestInter2/2] = subPathNum; // save pathNum for respective dropoff
            dropped[bestInter2/2] = true;
            numDropped++;
        }
        
        subPathNum++;
    }

    // find the closest end depot ===========================================================
    
    bestInter1 = bestInter2;
    minTime = NOTIME;
    
    for(unsigned j=pathTimes.size() ; j<pathTimes[bestInter1].size(); j++){
        if(pathTimes[bestInter1][j].time < minTime){
            minTime = pathTimes[bestInter1][j].time;
            bestInter2 = j; //depot
        }
    }
    
    route.timePerSub.push_back(minTime);
    route.bestInts.push_back(bestInter2);
    route.intTypes.push_back(DEPOT);
    route.remWeightHere.push_back(remainingWeight);
    
    picked.clear();
    dropped.clear();
    
    route.courierTime = addSubPathTimes(route.timePerSub);

    return route;
}

double addSubPathTimes(std::vector<double> times){
    double total = 0;
    
    for(unsigned i=0; i<times.size(); i++){
        total += times[i];
    }
    
    return total;
}
    
    
// structure contains compare operator for sorting priority queue
// sorts queue using scores in waveElem such that lowest score is placed at front of queue
struct compareTime {
    bool operator()(const waveElem4 &a, const waveElem4 &b){
        return a.travelTime > b.travelTime;
    }
};


void multiDestPath(Node *sourceNode, 
                   const std::vector<unsigned>& allDest, 
                   std::vector<pathTime>& pathTimes,
                   const double rtPen,
                   const double ltPen){
    
    std::priority_queue< waveElem4 , std::vector<waveElem4 >, compareTime> wavefront;
    wavefront.push(waveElem4(NONODE, sourceNode, NOEDGE, 0));
     
    unsigned numMatched = 0;
    std::vector<unsigned> reachingEdge;
    std::vector<double> bestTime;
    std::vector<Node *> reachingNode;
    
    reachingEdge.resize(Dir.Nodes.size());
    reachingNode.resize(Dir.Nodes.size());
    bestTime.resize(Dir.Nodes.size());
    std::fill(bestTime.begin(), bestTime.end(), NOTIME);
    
    for(unsigned i=0 ; i<pathTimes.size(); i++){
        pathTimes[i].time = NOTIME;
    }
          
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
            #pragma omp parallel for
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
            #pragma omp parallel for
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