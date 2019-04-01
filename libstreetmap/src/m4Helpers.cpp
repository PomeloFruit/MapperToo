#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m4.h"
#include "m3Helpers.h"
#include "m4Helpers.h"

#include "LatLon.h"
#include "latLonToXY.h"
#include "StreetsDatabaseAPI.h"
#include "directionInfo.h"

#include <math.h>
#include <time.h>
#include <vector>
#include <list>
#include <algorithm>
#include <queue>
#include <string>
#include <limits>
#include <iostream>
#include <chrono>

// ==========================================================================================================================

void opt_2_Swap(multiStruct &temp, 
                const unsigned start,
                const unsigned len, 
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<interestingDepotTime>& bestDepotToDest,
                const std::vector<DeliveryInfo>& deliveries){
    
    multiStruct testNew = temp;
    
    if((start+len)<temp.bestDests.size()-2){
        swap(testNew.destTypes[start], testNew.destTypes[start+len]);
        swap(testNew.bestDests[start], testNew.bestDests[start+len]);
    
        opt_k_Checks(temp, testNew, start, len, pathTimes, bestDepotToDest, deliveries);
    }
}

void opt_3_Swap(multiStruct &temp, 
                const unsigned start,
                const unsigned len,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<interestingDepotTime>& bestDepotToDest,
                const std::vector<DeliveryInfo>& deliveries){
    
    multiStruct testNew = temp;

    if((start+len)<temp.bestDests.size()-2){
        swap(testNew.destTypes[start], testNew.destTypes[start+len]);
        swap(testNew.bestDests[start], testNew.bestDests[start+len]);
        swap(testNew.destTypes[start+(len/2)], testNew.destTypes[start+len]);
        swap(testNew.bestDests[start+(len/2)], testNew.bestDests[start+len]);

        opt_k_Checks(temp, testNew, start, len, pathTimes, bestDepotToDest, deliveries);
    }
}

void opt_n_GroupSwap(multiStruct &temp,
                const unsigned n,
                const unsigned start,
                const unsigned len,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<interestingDepotTime>& bestDepotToDest,
                const std::vector<DeliveryInfo>& deliveries){
    
    if(start<temp.bestDests.size()-2 && start+len<temp.bestDests.size()-2 && start+len-n>0){
        multiStruct testNew = temp;
                
        for(unsigned i = 0; i<n && i<(len-i); i++){
            swap(testNew.destTypes[start+i], testNew.destTypes[start+len-i]);
            swap(testNew.bestDests[start+i], testNew.bestDests[start+len-i]);
        }

        opt_k_Checks(temp, testNew, start, len, pathTimes, bestDepotToDest, deliveries);
    }
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
                const std::vector<interestingDepotTime>& bestDepotToDest,
                const std::vector<DeliveryInfo>& deliveries){
    
    multiStruct testNew = temp;
    
    for(unsigned r=0; r<ror; r++){
        for(unsigned i=start; i<(start+len) && i<(temp.bestDests.size()-2); i++){
            swap(testNew.destTypes[i], testNew.destTypes[i+1]);
            swap(testNew.bestDests[i], testNew.bestDests[i+1]);
        }
    }
      
    opt_k_Checks(temp, testNew, start, len, pathTimes, bestDepotToDest, deliveries);
}


void opt_k_Reverse(multiStruct &temp, 
                const unsigned start,
                const unsigned len,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<interestingDepotTime>& bestDepotToDest,
                const std::vector<DeliveryInfo>& deliveries){
    
    multiStruct testNew = temp;

    for(unsigned i=start; i<(start+len)/2  && i<(temp.bestDests.size()-2); i++){
        swap(testNew.destTypes[i], testNew.destTypes[start+len-i]);
        swap(testNew.bestDests[i], testNew.bestDests[start+len-i]);
    }
      
    opt_k_Checks(temp, testNew, start, len, pathTimes, bestDepotToDest, deliveries);
}


void opt_k_Checks(multiStruct &temp,
                  multiStruct &testNew,  
                  unsigned start,
                  unsigned len, 
                  const std::vector<std::vector<pathTime>>& pathTimes,
                  const std::vector<interestingDepotTime>& bestDepotToDest,
                  const std::vector<DeliveryInfo>& deliveries){

    bool repaired = false;
    unsigned minRepaired = start;
    unsigned maxRepaired = start+len;
    
    if(maxRepaired > temp.bestDests.size()-2){
        return;
    }
    
    // if it is the first destination after a depot
    if(start == 1){
        if(testNew.destTypes[1] == PICKUP && bestDepotToDest[testNew.bestDests[1]/2].time != NOTIME){
            testNew.bestDests[0] = bestDepotToDest[testNew.bestDests[1]/2].depotNum;
            testNew.courierTime = testNew.courierTime - testNew.timePerSub[0];
            testNew.timePerSub[0] = bestDepotToDest[testNew.bestDests[1]/2].time;
            testNew.courierTime = testNew.courierTime + testNew.timePerSub[0];
        } else {
            return;
        }
    }
    
    for(unsigned i = minRepaired; i<=maxRepaired && i<(temp.bestDests.size()-1); i++){ //size-1 is depot-1 = last dropoff
        if(testNew.destTypes[i] == PICKUP){
            testNew.pickUpIndex[testNew.bestDests[i]/2] = i;
            testNew.remWeightHere[i] = testNew.remWeightHere[i-1]-deliveries[testNew.bestDests[i]/2].itemWeight;
        } else {
            testNew.dropOffIndex[testNew.bestDests[i]/2] = i;
            testNew.remWeightHere[i] = testNew.remWeightHere[i-1]+deliveries[testNew.bestDests[i]/2].itemWeight;
        }

        // exceeds truck capacity
        if(testNew.remWeightHere[i] < 0){
            testNew = temp;
            return;
        }
    }

    // try to repair the route so that it can work, swap the pickup and dropoff if possible
    for(unsigned i = minRepaired; i<=maxRepaired && i<(temp.bestDests.size()-1); i++){
        
        unsigned oldPickUpNum = testNew.pickUpIndex[testNew.bestDests[i]/2];
        unsigned oldDropOffNum = testNew.dropOffIndex[testNew.bestDests[i]/2];
        
        if(oldPickUpNum > oldDropOffNum){
            // try and repair the route so that it can work
            swap(testNew.pickUpIndex[testNew.bestDests[i]/2], testNew.dropOffIndex[testNew.bestDests[i]/2]);
            swap(testNew.bestDests[oldPickUpNum], testNew.bestDests[oldDropOffNum]);
            swap(testNew.destTypes[oldPickUpNum], testNew.destTypes[oldDropOffNum]);

            if(minRepaired > oldPickUpNum){
                minRepaired = oldPickUpNum;
            } 
            if(minRepaired > oldDropOffNum){
                minRepaired = oldDropOffNum;
            }
            if(maxRepaired < oldPickUpNum){
                maxRepaired = oldPickUpNum;
            }
            if(maxRepaired < oldDropOffNum){
                maxRepaired = oldDropOffNum;
            }
            
            repaired = true;
        }
    }
    
    if(repaired){
        for(unsigned i = minRepaired; i <= maxRepaired && i<(temp.bestDests.size()-1); i++){ //size-1 is depot-1 = last dropoff
            if(testNew.destTypes[i] == PICKUP){
                testNew.pickUpIndex[testNew.bestDests[i]/2] = i;
                testNew.remWeightHere[i] = testNew.remWeightHere[i-1]-deliveries[testNew.bestDests[i]/2].itemWeight;
            } else {
                testNew.dropOffIndex[testNew.bestDests[i]/2] = i;
                testNew.remWeightHere[i] = testNew.remWeightHere[i-1]+deliveries[testNew.bestDests[i]/2].itemWeight;
            }

            // exceeds truck capacity
            if(testNew.remWeightHere[i] < 0){
                testNew = temp;
                return;
            }
        }
    }
    

    double oldTime = 0;
    double newTime = 0;
    
    // check before and after the range changed and only calculate times if start or end has changed
    for(unsigned i = minRepaired-1; i <= maxRepaired && i<(temp.bestDests.size()-1); i++){
        if(pathTimes[testNew.bestDests[i]][testNew.bestDests[i+1]].time == NOTIME){
            return;
        }
        oldTime = oldTime + testNew.timePerSub[i];
        testNew.timePerSub[i] = pathTimes[testNew.bestDests[i]][testNew.bestDests[i+1]].time;
        newTime = newTime + testNew.timePerSub[i];
    }

    // if time is shorter, set temp to the good stuff and update total time
    double deltaT = newTime - oldTime;
    if(deltaT < 0){
        testNew.courierTime = testNew.courierTime + deltaT;
        temp = testNew;
    }
    
}


//std::vector<CourierSubpath> anneal (std::vector<CourierSubpath> courierPath, 
//             multiStruct &best, const float truck_capacity, 
//             const std::vector<std::vector<pathTime>>& pathTimes,
//             const std::vector<std::vector<pathTime>>& depotTimes,
//             const std::vector<DeliveryInfo>& deliveries)
//{
//    //NOTE ADD CHRONO TIME HERE
//    int seed = 0;
//    double time = best.courierTime; 
//    double currentTruckWeight = 0; 
//    double Tungsten = 3422.0; 
//    double dTime = Tungsten; 
//    double perturbRatio = 1.0; 
//    int perturb1, perturb2;
//    std::vector<std::pair<unsigned, bool>> packages; 
//    bool isValid=true;
//    
//    while(Tungsten > 0){
//        seed++; 
//        srand(seed); 
//        perturb1 = rand()%(int(courierPath.size()/perturbRatio)-2)+1;
//        
//        seed++; 
//        srand(seed);
//        perturb2 = rand()%(int(courierPath.size()/perturbRatio)-2)+1;
//        
//        CourierSubpath temp = courierPath[perturb1]; 
//        
//        //Swap the two paths
//        courierPath[perturb1] = courierPath[perturb2]; 
//        courierPath[perturb2] = temp; 
//        
//        
//        CourierSubpath swappedThis = courierPath[perturb1];
//        CourierSubpath withThis = courierPath[perturb2]; 
//        
//        
//        for(int i = 0; i < perturb1; i++){
//            for(int j = 0; j < courierPath[i].pickUp_indices.size(); j++){
//                int index = courierPath[i].pickUp_indices[j];
//                
//                currentTruckWeight = currentTruckWeight + deliveries[index].itemWeight; 
//                
//                packages.push_back(std::make_pair(index, false));
//            }
//            //check if we have already visited a dropoff of the first changes pickups, if so this is hella illegal and we gotta stop before the cops come
//            int dropIndexSize = courierPath[perturb1].pickUp_indices.size();
//            for(int j=0;j< dropIndexSize;j++){
//                int dropIndex = courierPath[perturb1].pickUp_indices[j];
//                if(deliveries[dropIndex].dropOff==courierPath[i].start_intersection){
//                    //if we ever reach the dropoff in this loop it's fucked, like it won't work you know
//                    isValid=false;
//                }
//            }
//            if(!isValid){
//                //if the path is bad leave the for loop and shit
//                break;
//            }
//            //get all the packages up until the first swapped spot
//            for(int j = 0; j < packages.size(); j++){
//                if((packages[j].second == false) && (deliveries[packages[j].first].dropOff == courierPath[i].start_intersection)){
//                    currentTruckWeight = currentTruckWeight - deliveries[packages[j].first].itemWeight;
//                    packages[j].first=true;
//                }
//            }
//            //drop off all packages up until first swapped spot
//        }
//        //what else did we need to take care of in the first loop? we've done the weight and checked if the path was valid up until that point
//        //well yeah but like was there any other dumb shit we had to deal with?
//        
//        
//        //handles p1 and everything up until p2
//        if(isValid){ 
//            // pick stuff up
//            for(int i = 0; i < swappedThis.pickUp_indices.size(); i++){
//                currentTruckWeight = currentTruckWeight + deliveries[swappedThis.pickUp_indices[i]].itemWeight; 
//                if(currentTruckWeight <= truck_capacity){
//                    packages.push_back(std::make_pair(swappedThis.pickUp_indices[i], false)); 
//                }else{
//                    isValid = false; 
//                    break; 
//                }
//            }
//            // drop things off if needed
//            for(int i = 0; i < packages.size(); i++){
//                if((packages[i].second == false)&&(deliveries[packages[i].first].dropOff == courierPath[i].start_intersection)){
//                    currentTruckWeight = currentTruckWeight - deliveries[packages[i].first].itemWeight;
//                    packages[i].first=true;
//                }
//            }
//            //pick up the stuff at perturb1
//            //what happens if we can't pick up all the stuff at 1?
//            //is it illegal then because we don't return?
//            //if cannot reach pickup reqs at 1 the move is also illegal!
//            
//            //then check perturb2
//            //what if p1&p2 are right after eachother? fine I guess since we just need to check p2 after
//            //and then after that we have to check from p2 to end
//            for(int i=(perturb1+1);i<perturb2;i++){
//                //here we pick up all the new shit at the index place
//                for(int j=0;j<courierPath[i].pickUp_indices.size();j++){
//                    int index=courierPath[i].pickUp_indices[j];
//                    currentTruckWeight = currentTruckWeight + deliveries[index].itemWeight; 
//                    packages.push_back(std::make_pair(index, false));
//                }
//                
//                
//                //now I have to check if I've already been to the dropoff stuff, you know how it is 
//                int dropIndexSize=courierPath[perturb2].pickUp_indices.size();
//                for(int j=0;j<dropIndexSize;j++){
//                   int dropIndex=courierPath[perturb2].pickUp_indices[j];
//                   if(deliveries[dropIndex].dropOff==courierPath[i].start_intersection){
//                       isValid=false;
//                   }
//                }
//                if(!isValid){
//                    break;
//                }
//                
//                //now I have to drop off all the stuff that I can! hehe
//                for(int j=0;j<packages.size();j++){
//                    if((packages[j].second==false)&&(deliveries[packages[j].first].dropOff==courierPath[i].start_intersection)){
//                        currentTruckWeight=currentTruckWeight-deliveries[packages[j].first].itemWeight;
//                        packages[j].first=true;
//                    }
//                }
//                
//                //now we have to check if we've gone overweight (didn't have to do it for the first part since nothing had changed!)
//                //if we have gone over shit be bad and invalid
//                if(currentTruckWeight>truck_capacity){
//                    isValid=false;
//                    break;
//                }
//                
//            }
//   
//        }
//        
//        //handles p2 and everything up to the end
//        if(isValid){
//            //pick up all the stuff
//            for(int i = 0; i < withThis.pickUp_indices.size(); i++){
//                currentTruckWeight = currentTruckWeight + deliveries[withThis.pickUp_indices[i]].itemWeight; 
//                if(currentTruckWeight <= truck_capacity){
//                    packages.push_back(std::make_pair(withThis.pickUp_indices[i], false)); 
//                }else{
//                    isValid = false; 
//                    break; 
//                }
//            }
//            // drop things off if needed
//            for(int i = 0; i < packages.size(); i++){
//                if((packages[i].second == false)&&(deliveries[packages[i].first].dropOff == courierPath[i].start_intersection)){
//                    currentTruckWeight = currentTruckWeight - deliveries[packages[i].first].itemWeight;
//                    packages[i].first=true;
//                }
//            }
//            
//            //time to go from p2+1 to the end of courierPath
//            for(int i=perturb2+1;i<courierPath.size();i++){
//                //here we pick up all the new shit at the index place
//                for(int j=0;j<courierPath[i].pickUp_indices.size();j++){
//                    int index=courierPath[i].pickUp_indices[j];
//                    currentTruckWeight = currentTruckWeight + deliveries[index].itemWeight; 
//                    packages.push_back(std::make_pair(index, false));
//                }
//                
//                //drop the stuff that you can off
//                for(int j=0;j<packages.size();j++){
//                    if((packages[j].second==false)&&(deliveries[packages[j].first].dropOff==courierPath[i].start_intersection)){
//                       currentTruckWeight=currentTruckWeight-deliveries[packages[j].first].itemWeight;
//                       packages[j].first=true; 
//                    }
//                }
//                //check if overweight
//                if(currentTruckWeight>truck_capacity){
//                    isValid=false;
//                    break;
//                }
//            }
//        }
//        
//        
//        if(isValid){
//            //finalize swap and clear stuff
//            //get time
//        }else{
//            //undo swap and clear stuff
//        }
//        //so we have covered all of the shit things before the first change
//
//        
////        //Check legality 
////        if(swappedThis.pickUp_indices.size() > 0){
////            
////        }else{
////            
////        }
//        
//        
//    }
//    
//    return courierPath; 
//}



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
    
    route.bestDests.clear();
    route.destTypes.clear();
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
    route.bestDests.push_back(bestInter1);
    route.destTypes.push_back(DEPOT);
    route.bestDests.push_back(bestInter2);
    route.destTypes.push_back(PICKUP);
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
        minTime = NOTIME;
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

        route.bestDests.push_back(bestInter2);
        route.timePerSub.push_back(minTime);
        route.remWeightHere.push_back(remainingWeight);
        
        if(isCurrentPickUp){
            remainingWeight = remainingWeight - deliveries[bestInter2/2].itemWeight;
            route.destTypes.push_back(PICKUP);
            route.pickUpIndex[bestInter2/2] = subPathNum;
            picked[bestInter2/2] = true;
            numPicked++;
        } else {
            remainingWeight = remainingWeight + deliveries[bestInter2/2].itemWeight;
            route.destTypes.push_back(DROPOFF);
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
    route.bestDests.push_back(bestInter2);
    route.destTypes.push_back(DEPOT);
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
     
    double maxTimeToSearch = NOTIME;
    unsigned numMatched = 0;
    unsigned numDropMatched = 0;
    unsigned numDeliveries = pathTimes.size() / 2;
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
            //#pragma omp parallel for
            for(unsigned i=0; i<allDest.size(); i++){
                if(currNode->id == allDest[i]){
                    //extract the path from the nodes
                    pathTimes[i].path = getFinalPath4(currNode,sourceNode->id, reachingEdge, reachingNode);
                    pathTimes[i].time = bestTime[currNode->id];
                    numMatched++;
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
