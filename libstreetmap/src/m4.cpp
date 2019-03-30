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
#include <time.h>
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

struct interestingDepotTime {
    double time;
    unsigned depotNum;
};

struct multiStruct {
    double courierTime; // total time for the entire route
    std::vector<double> timePerSub; // time per subpath
    std::vector<unsigned> bestDests; // the pickup/dropoff/depot number to travel to/from
    std::vector<unsigned> destTypes; //0 for pickup, 1 for dropoff, 2 for depot
    std::vector<unsigned> pickUpIndex; // in what subpath do we pick up the package
    std::vector<unsigned> dropOffIndex; // in what part of the route do we drop off the package
    std::vector<double> remWeightHere; // the remaining weight left in the truck at this subpath
};

#define TIME_LIMIT 45
#define CHICKEN 0.99
#define SCARED 0.7

#define PICKUP 0
#define DROPOFF 1
#define DEPOT 2

std::vector<unsigned> PrimeNumbers = {13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 
                                      79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163,
                                      167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 
                                      257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
                                      353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443,
                                      449, 457, 461, 463, 467, 479, 487, 491, 499, 503};

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

void opt_2_Swap(multiStruct &temp, 
                const unsigned size,
                const unsigned len,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<interestingDepotTime>& bestDepotToDest,
                const std::vector<DeliveryInfo>& deliveries);

void opt_3_Swap(multiStruct &temp, 
                const unsigned dest,
                const unsigned len,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<interestingDepotTime>& bestDepotToDest,
                const std::vector<DeliveryInfo>& deliveries);

void swap(unsigned &a, unsigned &b);

void opt_k_Rotate(multiStruct &temp, 
                const unsigned size,
                const unsigned len,
                const unsigned ror,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<interestingDepotTime>& bestDepotToDest,
                const std::vector<DeliveryInfo>& deliveries);

void opt_k_Reverse(multiStruct &temp, 
                const unsigned start,
                const unsigned len,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<interestingDepotTime>& bestDepotToDest,
                const std::vector<DeliveryInfo>& deliveries);

void opt_k_Checks(multiStruct &temp,
                  multiStruct &testNew,  
                  const unsigned start,
                  const unsigned len, 
                  const std::vector<std::vector<pathTime>>& pathTimes,
                  const std::vector<interestingDepotTime>& bestDepotToDest,
                  const std::vector<DeliveryInfo>& deliveries);

multiStruct multiStart(const unsigned numDeliveries, 
                       const unsigned startDepot,
                       const std::vector<std::vector<pathTime>>& pathTimes, 
                       const std::vector<std::vector<pathTime>>& depotTimes,
                       const std::vector<DeliveryInfo>& deliveries,
                       const double truckCap);

double addSubPathTimes(std::vector<double> times);

std::vector<CourierSubpath> anneal (std::vector<CourierSubpath> courierPath, 
             multiStruct &best, const float truck_capacity, 
             const std::vector<std::vector<pathTime>>& pathTimes,
             const std::vector<std::vector<pathTime>>& depotTimes,
             const std::vector<DeliveryInfo>& deliveries);

// ==================================================================================

std::vector<CourierSubpath> traveling_courier(
		const std::vector<DeliveryInfo>& deliveries,
	       	const std::vector<unsigned>& depots, 
		const float right_turn_penalty, 
		const float left_turn_penalty, 
		const float truck_capacity){
    
    auto startTime = std::chrono::high_resolution_clock::now();
   
    std::vector<CourierSubpath> courierPath;
    
    // std::cout << "Deliveries: " << deliveries.size() << " Depots: " << depots.size() << std::endl;

    unsigned numDeliveries = deliveries.size();
    unsigned numDepots = depots.size();
    unsigned totDest = 2*numDeliveries + numDepots;
        
    std::vector<std::vector<pathTime>> pathTimes; // from pickup/dropoff to pickup/dropoff/depot
    std::vector<std::vector<pathTime>> depotTimes; // from all depots to all pickups
    std::vector<interestingDepotTime> bestDepotToDest; // best times between depot and all pickups
    
    pathTimes.resize(2*numDeliveries);
    depotTimes.resize(numDepots);
    bestDepotToDest.resize(numDeliveries);
       
    // get all path / time from start-dest combinations ===========================================
    
    for(unsigned i=0 ; i<pathTimes.size() ; i++){
        pathTimes[i].resize(totDest);
    }
    
    for(unsigned i=0 ; i<depotTimes.size() ; i++){
        depotTimes[i].resize(numDeliveries);
    }
    
    fillAllPathTimes(pathTimes, depotTimes, deliveries, depots, right_turn_penalty, left_turn_penalty);
    
    // save best depot->pickup times for all pickups ========================================================
    
    #pragma omp parallel for
    for(unsigned i=0 ; i<numDeliveries ; i++){ 
        bestDepotToDest[i].time = NOTIME;
        
        for(unsigned j=0 ; j<depotTimes.size() ; j++){
            if(depotTimes[j][i].time < bestDepotToDest[i].time){
                bestDepotToDest[i].time = depotTimes[j][i].time;
                bestDepotToDest[i].depotNum = j;
            }
        }
    }
    
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
    double kOpt = betterPath.bestDests.size();
    double maxIter = 50;

    bool timeOut = false;
    bool scared = false;
    bool somethingChanged = true;
    unsigned numIter = 5;
    auto prevTime = std::chrono::high_resolution_clock::now();

    for(unsigned z = 0; z < maxIter && !timeOut && numDeliveries > 5; z++){
        if(!somethingChanged && !scared){
            numIter += numIter;
        } else if(!somethingChanged){
            numIter += 1;
        }

        if(z>0){
            for(unsigned f=0; f<100;f++){
                if(PrimeNumbers[f]>(2*numDeliveries-1)){
                    break;
                }
                for(unsigned i=0;i<100;i++){
                    if(PrimeNumbers[i] > (2*numDeliveries - PrimeNumbers[f]-1)){
                        break;
                    }
                    
                    multiStruct temp = betterPath;
                    opt_3_Swap(temp, PrimeNumbers[f], PrimeNumbers[i], pathTimes, bestDepotToDest, deliveries);
                    if(temp.courierTime < betterPath.courierTime){
                        betterPath = temp;
                    }
                }
            }
        }
        
        somethingChanged = false;
        //std::cout << z << " " << std::endl;
        
        unsigned e = 1;
        if(numDeliveries > 200  && z==0){
            e = 3;
        } else if(numDeliveries > 100 && z==0){
            e = 2;
        } 
        
        for(unsigned k = 1; k < kOpt && !timeOut; k+=e){
            for(unsigned i = 1; betterPath.bestDests.size() > (k+1) && i< betterPath.bestDests.size()-k-1 && !timeOut; i++){

                std::vector<multiStruct> pathToTry;
                pathToTry.resize(numIter);

                #pragma omp parallel for
                for(unsigned d=0; d<numIter; d++){
                    pathToTry[d] = betterPath;
                }

                // try different things
                #pragma omp parallel for
                for(unsigned d=0; d<numIter; d++){
                    multiStruct temp = pathToTry[d];
                   // opt_k_Rotate(temp, i, k, d, pathTimes, bestDepotToDest, deliveries);
                    if(temp.courierTime < pathToTry[d].courierTime){
                        pathToTry[d] = temp;
                    }

                    multiStruct beforeSwap = pathToTry[d];

                    // try some swaps within the numIter range
                    #pragma omp parallel for
                    for(unsigned f=0; f<k; f++){
                        multiStruct temp1 = beforeSwap;
                        opt_2_Swap(temp1, i, f, pathTimes, bestDepotToDest, deliveries);
                        
                        if(temp1.courierTime < pathToTry[d].courierTime){
                            pathToTry[d] = temp1;
                        }
                    }
                }
                
                // update the best path currently
                for(unsigned d=0; d<numIter; d++){
                    if(pathToTry[d].courierTime < betterPath.courierTime){
                        betterPath = pathToTry[d];
                        somethingChanged = true;
                    }
                }
                
                multiStruct temp = betterPath;
                opt_k_Reverse(temp, i, k, pathTimes, bestDepotToDest, deliveries);
                if(temp.courierTime < betterPath.courierTime){
                    betterPath = temp;
                }

                // determine time left
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto diffClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-prevTime);
                auto wallClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-startTime);
                double timeElapsed = wallClock.count();
                double timeForLast = diffClock.count();

                prevTime = currentTime;

                if((TIME_LIMIT*CHICKEN - timeElapsed) < 2*timeForLast){
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
    
    for(unsigned i=0; i<bestCI.bestDests.size()-1 && !bestCI.bestDests.empty(); i++){
        CourierSubpath tempSubpath;
        tempSubpath.pickUp_indices.clear();
         
        if(bestCI.destTypes[i] == DEPOT){
            tempSubpath.start_intersection = depots[bestCI.bestDests[i]];
            tempSubpath.subpath = depotTimes[bestCI.bestDests[i]][bestCI.bestDests[i+1]/2].path;
            
        } else if(bestCI.destTypes[i] == PICKUP){
            tempSubpath.start_intersection = deliveries[bestCI.bestDests[i]/2].pickUp;       
            tempSubpath.pickUp_indices.push_back(bestCI.bestDests[i]/2);
            tempSubpath.subpath = pathTimes[bestCI.bestDests[i]][bestCI.bestDests[i+1]].path;
            
        } else { //dropoff
            tempSubpath.start_intersection = deliveries[bestCI.bestDests[i]/2].dropOff;
            tempSubpath.subpath = pathTimes[bestCI.bestDests[i]][bestCI.bestDests[i+1]].path;
        }
        
        // set the end intersection locations 
        
        if(bestCI.destTypes[i+1] == DEPOT){
            tempSubpath.end_intersection = depots[bestCI.bestDests[i+1]-2*numDeliveries];
        } else if(bestCI.destTypes[i+1] == PICKUP){
            tempSubpath.end_intersection = deliveries[bestCI.bestDests[i+1]/2].pickUp;
        } else {
            tempSubpath.end_intersection = deliveries[bestCI.bestDests[i+1]/2].dropOff;
        }
        
//        std::cout << i << " " << tempSubpath.start_intersection << " " << tempSubpath.end_intersection << std::endl;
                
        courierPath.push_back(tempSubpath);
    }
    
    bestCI.bestDests.clear();
    bestCI.destTypes.clear();
    pathTimes.clear();
    depotTimes.clear();
    bestDepotToDest.clear();
    
    //call anneal here
    
    return courierPath;
}

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
                  const unsigned start,
                  const unsigned len, 
                  const std::vector<std::vector<pathTime>>& pathTimes,
                  const std::vector<interestingDepotTime>& bestDepotToDest,
                  const std::vector<DeliveryInfo>& deliveries){

    // if it is the first destination after a depot
    if(start == 1){
        if(testNew.destTypes[1] == PICKUP){
            testNew.bestDests[0] = bestDepotToDest[testNew.bestDests[1]/2].depotNum;
            testNew.courierTime = testNew.courierTime - testNew.timePerSub[0];
            testNew.timePerSub[0] = bestDepotToDest[testNew.bestDests[1]/2].time;
            testNew.courierTime = testNew.courierTime + testNew.timePerSub[0];
        } else {
            return;
        }
    }
    
    for(unsigned i = start; i<=start+len && i<(temp.bestDests.size()-1); i++){ //size-1 is depot-1 = last dropoff
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

    for(unsigned i = start; i<=start+len && i<(temp.bestDests.size()-1); i++){
        if(testNew.pickUpIndex[testNew.bestDests[i]/2] > testNew.dropOffIndex[testNew.bestDests[i]/2]){
            testNew = temp;
            return;
        }
    }

    double oldTime = 0;
    double newTime = 0;
     // check before and after the range changed and only calculate times if start or end has changed
    for(unsigned i = start-1; i<=start+len && i<(temp.bestDests.size()-1); i++){
        if((testNew.bestDests[i] != temp.bestDests[i]) || (testNew.bestDests[i+1] != temp.bestDests[i+1])){
            oldTime = oldTime + testNew.timePerSub[i];
            testNew.timePerSub[i] = pathTimes[testNew.bestDests[i]][testNew.bestDests[i+1]].time;
            newTime = newTime + testNew.timePerSub[i];
        }
    }

    double deltaT = newTime - oldTime;
    if(deltaT < 0){
        testNew.courierTime = testNew.courierTime + deltaT;
        temp = testNew;
    }
    
}


std::vector<CourierSubpath> anneal (std::vector<CourierSubpath> courierPath, 
             multiStruct &best, const float truck_capacity, 
             const std::vector<std::vector<pathTime>>& pathTimes,
             const std::vector<std::vector<pathTime>>& depotTimes,
             const std::vector<DeliveryInfo>& deliveries)
{
    //NOTE ADD CHRONO TIME HERE
    int seed = 0;
    double time = best.courierTime; 
    double currentTruckWeight = 0; 
    double Tungsten = 3422.0; 
    double dTime = Tungsten; 
    double perturbRatio = 1.0; 
    int perturb1, perturb2;
    std::vector<std::pair<unsigned, bool>> packages; 
    bool isValid=true;
    
    while(Tungsten > 0){
        seed++; 
        srand(seed); 
        perturb1 = rand()%(int(courierPath.size()/perturbRatio)-2)+1;
        
        seed++; 
        srand(seed);
        perturb2 = rand()%(int(courierPath.size()/perturbRatio)-2)+1;
        
        CourierSubpath temp = courierPath[perturb1]; 
        
        //Swap the two paths
        courierPath[perturb1] = courierPath[perturb2]; 
        courierPath[perturb2] = temp; 
        
        
        CourierSubpath swappedThis = courierPath[perturb1];
        CourierSubpath withThis = courierPath[perturb2]; 
        
        
        for(int i = 0; i < perturb1; i++){
            for(int j = 0; j < courierPath[i].pickUp_indices.size(); j++){
                int index = courierPath[i].pickUp_indices[j];
                
                currentTruckWeight = currentTruckWeight + deliveries[index].itemWeight; 
                
                packages.push_back(std::make_pair(index, false));
            }
            //check if we have already visited a dropoff of the first changes pickups, if so this is hella illegal and we gotta stop before the cops come
            int dropIndexSize = courierPath[perturb1].pickUp_indices.size();
            for(int j=0;j< dropIndexSize;j++){
                int dropIndex = courierPath[perturb1].pickUp_indices[j];
                if(deliveries[dropIndex].dropOff==courierPath[i].start_intersection){
                    //if we ever reach the dropoff in this loop it's fucked, like it won't work you know
                    isValid=false;
                }
            }
            if(!isValid){
                //if the path is bad leave the for loop and shit
                break;
            }
            //get all the packages up until the first swapped spot
            for(int j = 0; j < packages.size(); j++){
                if((packages[j].second == false) && (deliveries[packages[j].first].dropOff == courierPath[i].start_intersection)){
                    currentTruckWeight = currentTruckWeight - deliveries[packages[j].first].itemWeight;
                    packages[j].first=true;
                }
            }
            //drop off all packages up until first swapped spot
        }
        //what else did we need to take care of in the first loop? we've done the weight and checked if the path was valid up until that point
        //well yeah but like was there any other dumb shit we had to deal with?
        
        
        //handles p1 and everything up until p2
        if(isValid){ 
            // pick stuff up
            for(int i = 0; i < swappedThis.pickUp_indices.size(); i++){
                currentTruckWeight = currentTruckWeight + deliveries[swappedThis.pickUp_indices[i]].itemWeight; 
                if(currentTruckWeight <= truck_capacity){
                    packages.push_back(std::make_pair(swappedThis.pickUp_indices[i], false)); 
                }else{
                    isValid = false; 
                    break; 
                }
            }
            // drop things off if needed
            for(int i = 0; i < packages.size(); i++){
                if((packages[i].second == false)&&(deliveries[packages[i].first].dropOff == courierPath[i].start_intersection)){
                    currentTruckWeight = currentTruckWeight - deliveries[packages[i].first].itemWeight;
                    packages[i].first=true;
                }
            }
            //pick up the stuff at perturb1
            //what happens if we can't pick up all the stuff at 1?
            //is it illegal then because we don't return?
            //if cannot reach pickup reqs at 1 the move is also illegal!
            
            //then check perturb2
            //what if p1&p2 are right after eachother? fine I guess since we just need to check p2 after
            //and then after that we have to check from p2 to end
            for(int i=(perturb1+1);i<perturb2;i++){
                //here we pick up all the new shit at the index place
                for(int j=0;j<courierPath[i].pickUp_indices.size();j++){
                    int index=courierPath[i].pickUp_indices[j];
                    currentTruckWeight = currentTruckWeight + deliveries[index].itemWeight; 
                    packages.push_back(std::make_pair(index, false));
                }
                
                
                //now I have to check if I've already been to the dropoff stuff, you know how it is 
                int dropIndexSize=courierPath[perturb2].pickUp_indices.size();
                for(int j=0;j<dropIndexSize;j++){
                   int dropIndex=courierPath[perturb2].pickUp_indices[j];
                   if(deliveries[dropIndex].dropOff==courierPath[i].start_intersection){
                       isValid=false;
                   }
                }
                if(!isValid){
                    break;
                }
                
                //now I have to drop off all the stuff that I can! hehe
                for(int j=0;j<packages.size();j++){
                    if((packages[j].second==false)&&(deliveries[packages[j].first].dropOff==courierPath[i].start_intersection)){
                        currentTruckWeight=currentTruckWeight-deliveries[packages[j].first].itemWeight;
                        packages[j].first=true;
                    }
                }
                
                //now we have to check if we've gone overweight (didn't have to do it for the first part since nothing had changed!)
                //if we have gone over shit be bad and invalid
                if(currentTruckWeight>truck_capacity){
                    isValid=false;
                    break;
                }
                
            }
   
        }
        
        //handles p2 and everything up to the end
        if(isValid){
            //pick up all the stuff
            for(int i = 0; i < withThis.pickUp_indices.size(); i++){
                currentTruckWeight = currentTruckWeight + deliveries[withThis.pickUp_indices[i]].itemWeight; 
                if(currentTruckWeight <= truck_capacity){
                    packages.push_back(std::make_pair(withThis.pickUp_indices[i], false)); 
                }else{
                    isValid = false; 
                    break; 
                }
            }
            // drop things off if needed
            for(int i = 0; i < packages.size(); i++){
                if((packages[i].second == false)&&(deliveries[packages[i].first].dropOff == courierPath[i].start_intersection)){
                    currentTruckWeight = currentTruckWeight - deliveries[packages[i].first].itemWeight;
                    packages[i].first=true;
                }
            }
            
            //time to go from p2+1 to the end of courierPath
            for(int i=perturb2+1;i<courierPath.size();i++){
                //here we pick up all the new shit at the index place
                for(int j=0;j<courierPath[i].pickUp_indices.size();j++){
                    int index=courierPath[i].pickUp_indices[j];
                    currentTruckWeight = currentTruckWeight + deliveries[index].itemWeight; 
                    packages.push_back(std::make_pair(index, false));
                }
                
                //drop the stuff that you can off
                for(int j=0;j<packages.size();j++){
                    if((packages[j].second==false)&&(deliveries[packages[j].first].dropOff==courierPath[i].start_intersection)){
                       currentTruckWeight=currentTruckWeight-deliveries[packages[j].first].itemWeight;
                       packages[j].first=true; 
                    }
                }
                //check if overweight
                if(currentTruckWeight>truck_capacity){
                    isValid=false;
                    break;
                }
            }
        }
        
        
        if(isValid){
            //finalize swap and clear stuff
            //get time
        }else{
            //undo swap and clear stuff
        }
        //so we have covered all of the shit things before the first change

        
//        //Check legality 
//        if(swappedThis.pickUp_indices.size() > 0){
//            
//        }else{
//            
//        }
        
        
    }
    
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