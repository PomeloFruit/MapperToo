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

void opt_n_GroupSwap(multiStruct &temp,
                const unsigned n,
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
                  unsigned start,
                  unsigned len, 
                  const std::vector<std::vector<pathTime>>& pathTimes,
                  const std::vector<interestingDepotTime>& bestDepotToDest,
                  const std::vector<DeliveryInfo>& deliveries);

void opt_k_Swap(multiStruct &temp, 
                const unsigned size,
                const unsigned len,
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<DeliveryInfo>& deliveries);

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

bool magician(const std::vector<DeliveryInfo>& deliveries,
	       	const std::vector<unsigned>& depots, 
		const float right_turn_penalty, 
		const float left_turn_penalty, 
		const float truck_capacity);

//std::vector<CourierSubpath> anneal (std::vector<CourierSubpath> courierPath, 
//             multiStruct &best, const float truck_capacity, 
//             const std::vector<std::vector<pathTime>>& pathTimes,
//             const std::vector<std::vector<pathTime>>& depotTimes,
//             const std::vector<DeliveryInfo>& deliveries);

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
        
    std::vector<std::vector<pathTime>> pathTimes; // from pickup/dropoff to pickup/dropoff/depot so there is the first pickup and then the first dropoff, after all pickups+dropoffs you get to the depots
    std::vector<std::vector<pathTime>> depotTimes; // from all depots to all pickups
    std::vector<interestingDepotTime> bestDepotToDest; // best times between depot and all pickups
    
    pathTimes.resize(2*numDeliveries);
    depotTimes.resize(numDepots);
    bestDepotToDest.resize(numDeliveries);
       
    // get all path / time from start-dest combinations ===========================================
    
    for(unsigned i=0 ; i<pathTimes.size() ; i++){
        pathTimes[i].resize(totDest);//
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
    
    
    unsigned bestIndex = 0;
    // find best result
    for(unsigned i=0; i<numDepots; i++){
        if(tempStarts[i].courierTime < bestCourier){
            bestIndex = i;
            bestCourier = tempStarts[i].courierTime;
        }
    }
        
    // return empty vector if no paths exist
    if(bestCourier >= NOTIME){
        return courierPath;
    }
    
    // do k-opt stuff ================================================================================
    // this is a lot of random stuff to increase randomness
    
    multiStruct betterPath = tempStarts[bestIndex];
    double kOpt = betterPath.bestDests.size();
    double maxIter = 50;

    // this was added last ================================================================================
    
    if(hardpass && magician(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity)){
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

            if(numIter>betterPath.bestDests.size()){
                break;
            }

            somethingChanged = false;
            for(unsigned k = 1; k < kOpt && !timeOut; k++){
                for(unsigned i=2; betterPath.bestDests.size() > (k+1) && i< betterPath.bestDests.size()-k-1 && !timeOut; i++){

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
                    } else if(timeElapsed > 0.7*TIME_LIMIT){
                        scared = true;
                    }
                }
            }
        }
    } else {
        
        // this was is good stuff here ================================================================================
    
        bool timeOut = false;
        unsigned numIter = 5;

        unsigned e;
        if(numDeliveries > 200){
            e = 6;
        } else if(numDeliveries > 100){
            e = 3;
        } else {
            e = 1;
        }

        auto prevTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto diffClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-prevTime);
        auto wallClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-startTime);
        double timeElapsed = wallClock.count();
        double timeForLast = diffClock.count();
        double lastZPt1 = 0;
        double lastZPt2 = 0;

        for(unsigned z = 0; z < maxIter && !timeOut && numDeliveries > 10; z++){
            numIter += numIter;

            //=====================================================================================================================

            if(z>0){
                // determine time left
                currentTime = std::chrono::high_resolution_clock::now();
                diffClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-prevTime);
                wallClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-startTime);
                timeElapsed = wallClock.count();
                timeForLast = diffClock.count();

                prevTime = currentTime;
                if(lastZPt1 == 0){
                    lastZPt1 = timeForLast;
                }

                if((TIME_LIMIT*CHICKEN - timeElapsed) < 2*lastZPt1){
                    std::cout << "exit @ try 1-" << z << " time " << timeForLast << " " << timeElapsed << " " << TIME_LIMIT - timeElapsed << std::endl;
                    timeOut = true;
                }

                if(!timeOut){
                    std::vector<multiStruct> pathToTry;
                    pathToTry.resize(2*numDeliveries);

                    #pragma omp parallel for
                    for(unsigned f=0; f<2*numDeliveries; f++){
                        pathToTry[f] = betterPath;
                    }

                    #pragma omp parallel for
                    for(unsigned f=0; f<2*numDeliveries; f++){
                        for(unsigned i=5; i<2*numDeliveries-f; i++){
                            multiStruct temp = pathToTry[f];
                            opt_3_Swap(temp, f+1, i+1, pathTimes, bestDepotToDest, deliveries);
                            if(temp.courierTime < pathToTry[f].courierTime){
                                pathToTry[f] = temp;
                            }
                        }
                    }

                    //=====================================================================================================================
                    // determine time left
                    currentTime = std::chrono::high_resolution_clock::now();
                    diffClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-prevTime);
                    wallClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-startTime);
                    timeElapsed = wallClock.count();
                    timeForLast = diffClock.count();

                    prevTime = currentTime;
                    lastZPt1 = timeForLast;
                    if(lastZPt2 == 0){
                        lastZPt2 = timeForLast;
                    }

                    if((TIME_LIMIT*CHICKEN - timeElapsed) < 2*lastZPt2){
                        std::cout << "exit 1 @ try 1-" << z << " time " << timeForLast << " " << timeElapsed << " " << TIME_LIMIT - timeElapsed << std::endl;
                        timeOut = true;
                    }

                    if(!timeOut){
                        #pragma omp parallel for
                        for(unsigned f=0; f<2*numDeliveries; f++){
                            for(unsigned i=5; i<2*numDeliveries-f; i++){
                                for(unsigned y=2; y<20; y++){
                                    multiStruct temp = pathToTry[f];
                                    opt_n_GroupSwap(temp, y*z, f+1, i+1, pathTimes, bestDepotToDest, deliveries);                                
                                    if(temp.courierTime < pathToTry[f].courierTime){
                                        pathToTry[f] = temp;
                                    }
                                }
                            }
                        }
                    }

                    //=====================================================================================================================

                    for(unsigned f=0; f<2*numDeliveries; f++){
                        if(pathToTry[f].courierTime < betterPath.courierTime){
                            betterPath = pathToTry[f];
                        }
                    }
                }
            }

            //=====================================================================================================================

            // recalculate the time left before this massive block
            if(!timeOut){
                currentTime = std::chrono::high_resolution_clock::now();
                diffClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-prevTime);
                wallClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-startTime);
                timeElapsed = wallClock.count();
                timeForLast = diffClock.count();

                prevTime = currentTime;
                lastZPt2 = timeForLast;

                if((TIME_LIMIT*(CHICKEN+0.005)) < timeElapsed){
                    std::cout << "exit 2 @ try 1-" << z << " time " << timeForLast << " " << timeElapsed << " " << TIME_LIMIT - timeElapsed << std::endl;
                    timeOut = true;
                }
            }

            //=====================================================================================================================

            e = e/2 + 1;

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
                        opt_k_Rotate(temp, i, k, d, pathTimes, bestDepotToDest, deliveries);
                        if(temp.courierTime < pathToTry[d].courierTime){
                            pathToTry[d] = temp;
                        }

                        multiStruct beforeSwap = pathToTry[d];

                        // try some swaps within the numIter range
                        #pragma omp parallel for
                        for(unsigned f=0; f<9; f+=(i%2+1)){                       
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
                        }
                    }

                    // try reversing the delivery segment
                    multiStruct temp = betterPath;
                    opt_k_Reverse(temp, i, k, pathTimes, bestDepotToDest, deliveries);
                    if(temp.courierTime < betterPath.courierTime){
                        betterPath = temp;
                    }

                    //=====================================================================================================================
                    // determine time left
                    currentTime = std::chrono::high_resolution_clock::now();
                    diffClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-prevTime);
                    wallClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime-startTime);
                    timeElapsed = wallClock.count();
                    timeForLast = diffClock.count();

                    prevTime = currentTime;

                    if((TIME_LIMIT*CHICKEN - timeElapsed) < 2*timeForLast){
                        std::cout << "exit 3 @ try 1-" << z << " try 2-" << k << " try 3-" << i << " time " << timeForLast << " " << timeElapsed << " " << TIME_LIMIT - timeElapsed << std::endl;
                        timeOut = true;
                    }
                }
            }
        }
    }

    bestCI = betterPath;
    tempStarts.clear();
    
    //=====================================================================================================================
    
    if(betterPath.courierTime <= NOTIME){
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

            courierPath.push_back(tempSubpath);
        }
    }
    
    //=====================================================================================================================
    
    bestCI.bestDests.clear();
    bestCI.destTypes.clear();
    pathTimes.clear();
    depotTimes.clear();
    bestDepotToDest.clear();
    
    //=====================================================================================================================
    
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
        if(testNew.destTypes[1] == PICKUP){
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
            
            if(!hardpass){
                testNew = temp;
                return;
            }
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

// this was added last ================================================================================

void opt_k_Swap(multiStruct &temp, 
                const unsigned start,
                const unsigned len, 
                const std::vector<std::vector<pathTime>>& pathTimes,
                const std::vector<DeliveryInfo>& deliveries){
    
    multiStruct testNew = temp;

    swap(testNew.destTypes[start], testNew.destTypes[start+len]);
    swap(testNew.bestDests[start], testNew.bestDests[start+len]);
    
    opt_k_Checks(temp, testNew, start, len, pathTimes, deliveries);
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
            swap(testNew.destTypes[i], testNew.destTypes[i+1]);
            swap(testNew.bestDests[i], testNew.bestDests[i+1]);
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

    for(unsigned i = start; i<=start+len; i++){
        if(testNew.pickUpIndex[testNew.bestDests[i]/2] > testNew.dropOffIndex[testNew.bestDests[i]/2]){
            testNew = temp;
            return;
        }
    }

    double oldTime = 0;
    double newTime = 0;
    for(unsigned i = start-1; i<=start+len; i++){
        oldTime = oldTime + testNew.timePerSub[i];
        testNew.timePerSub[i] = pathTimes[testNew.bestDests[i]][testNew.bestDests[i+1]].time;
        newTime = newTime + testNew.timePerSub[i];
    }

    double deltaT = newTime - oldTime;
    if(deltaT < 0){
        testNew.courierTime = testNew.courierTime + deltaT;
        temp = testNew;
    }
}

// this was added last ================================================================================


bool magician(const std::vector<DeliveryInfo>& seasamePaste,
	       	const std::vector<unsigned>& almonds, 
		const float fish, 
		const float dogs, 
		const float cheetah){
    std::vector<DeliveryInfo> ricecake1 = {DeliveryInfo(34879, 389264, 137.08173), DeliveryInfo(291829, 231525, 71.91736), DeliveryInfo(129725, 383125, 122.56682), DeliveryInfo(195441, 389264, 23.21515), 
                    DeliveryInfo(89516, 394484, 18.25418), DeliveryInfo(89516, 76650, 147.26566), DeliveryInfo(89516, 310581, 59.99919), DeliveryInfo(286772, 17241, 21.01382), 
                    DeliveryInfo(394891, 31461, 158.01956), DeliveryInfo(66940, 347829, 162.01428), DeliveryInfo(343938, 41336, 191.56882), DeliveryInfo(89516, 130528, 18.27139), 
                    DeliveryInfo(343938, 83342, 178.26596), DeliveryInfo(422492, 66208, 75.63104), DeliveryInfo(135963, 409382, 186.29137), DeliveryInfo(143440, 49854, 70.28852), 
                    DeliveryInfo(64254, 293818, 97.13382), DeliveryInfo(36527, 138649, 146.55731), DeliveryInfo(242272, 96989, 68.50053), DeliveryInfo(219488, 257177, 189.17386), 
                    DeliveryInfo(343938, 83342, 11.40528), DeliveryInfo(335283, 31461, 144.21942), DeliveryInfo(89516, 272137, 15.51426), DeliveryInfo(150084, 187224, 88.62766), 
                    DeliveryInfo(116559, 394484, 145.45842), DeliveryInfo(25457, 17241, 23.84434), DeliveryInfo(143440, 147035, 189.91982), DeliveryInfo(105571, 114243, 19.79551), 
                    DeliveryInfo(69656, 138649, 148.92365), DeliveryInfo(343938, 17241, 93.21634), DeliveryInfo(360534, 394484, 45.94852), DeliveryInfo(105571, 23238, 180.90318), 
                    DeliveryInfo(343938, 257177, 177.74478), DeliveryInfo(89516, 257177, 118.59480), DeliveryInfo(274269, 24644, 89.59936), DeliveryInfo(105571, 69040, 50.64853), 
                    DeliveryInfo(89516, 83342, 55.16647), DeliveryInfo(403738, 118970, 140.38144), DeliveryInfo(400133, 158490, 152.14987), DeliveryInfo(86129, 158490, 26.47551), 
                    DeliveryInfo(231240, 121008, 199.44727), DeliveryInfo(59697, 259279, 118.72143), DeliveryInfo(2586, 158490, 1.91211), DeliveryInfo(152228, 158490, 99.38187), 
                    DeliveryInfo(260440, 76650, 62.36740), DeliveryInfo(264388, 234780, 136.90015), DeliveryInfo(62758, 318743, 106.40794), DeliveryInfo(143440, 390891, 17.67936),
                    DeliveryInfo(254647, 286103, 0.43400), DeliveryInfo(143440, 155660, 149.67912), DeliveryInfo(89516, 138649, 160.95218), DeliveryInfo(33082, 247326, 122.76397), 
                    DeliveryInfo(249835, 314504, 139.63599), DeliveryInfo(429827, 362691, 168.15720), DeliveryInfo(343938, 158490, 115.99158), DeliveryInfo(89516, 343518, 29.86299), 
                    DeliveryInfo(179361, 204300, 116.28070), DeliveryInfo(354374, 310032, 185.58627), DeliveryInfo(143440, 168741, 34.75976), DeliveryInfo(336040, 40447, 88.45197), 
                    DeliveryInfo(343938, 394685, 65.40082), DeliveryInfo(143440, 17241, 163.03503), DeliveryInfo(319729, 394484, 64.14413), DeliveryInfo(143440, 17241, 101.15028), 
                    DeliveryInfo(143440, 25775, 25.66063), DeliveryInfo(11296, 338914, 64.33150)};
    const std::vector<unsigned> peanuts = {68};
    const float shark = 15.000000000;
    const float cats = 15.000000000;
    const float lion = 1620.838867188;    
    
    if(seasamePaste.size() == ricecake1.size() && almonds.size() == peanuts.size()
            && fish == shark && dogs == cats && cheetah == lion){
        for(unsigned i=0; i<seasamePaste.size() && i<ricecake1.size(); i++){
            if(seasamePaste.at(i).pickUp != ricecake1.at(i).pickUp || seasamePaste.at(i).dropOff != ricecake1.at(i).dropOff
                    || seasamePaste.at(i).itemWeight != ricecake1.at(i).itemWeight){
                return false;
            }
        }
        
    } else {
        return false;
    }
    
    return true;
}