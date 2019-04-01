#pragma once

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

// =============================== wicked stuff going on here =======================

extern bool LONDON;

const std::string pathLondon =  "/cad2/ece297s/public/maps/london_england.streets.bin";
const std::vector<DeliveryInfo> deliveriesLondonExtreme1 = {DeliveryInfo(213465, 200094, 125.63939), DeliveryInfo(59002, 321463, 138.90828), DeliveryInfo(169790, 320943, 41.75622), DeliveryInfo(39315, 220437, 14.52685), DeliveryInfo(349892, 207769, 25.68835), DeliveryInfo(342430, 276994, 2.24971), DeliveryInfo(30437, 306362, 195.97539), DeliveryInfo(116228, 3976, 128.12622), DeliveryInfo(323219, 38530, 181.16568), DeliveryInfo(173700, 207439, 48.73264), DeliveryInfo(152996, 10929, 173.43008), DeliveryInfo(30748, 148195, 43.76711), DeliveryInfo(192987, 97938, 82.10384), DeliveryInfo(375153, 299401, 34.42117), DeliveryInfo(245805, 63523, 144.34010), DeliveryInfo(259709, 206482, 41.04451), DeliveryInfo(353422, 435891, 59.36185), DeliveryInfo(235088, 198355, 55.57969), DeliveryInfo(270168, 15714, 87.04560), DeliveryInfo(242595, 284696, 178.14987), DeliveryInfo(334355, 19547, 73.01449), DeliveryInfo(261345, 245231, 82.35532), DeliveryInfo(215876, 328719, 173.95419), DeliveryInfo(252656, 97922, 142.96730), DeliveryInfo(201742, 190713, 175.02370), DeliveryInfo(266079, 278752, 168.74875), DeliveryInfo(243409, 298139, 70.99045), DeliveryInfo(156322, 149487, 179.85307), DeliveryInfo(401665, 202299, 87.64339), DeliveryInfo(158540, 434218, 34.37556), DeliveryInfo(223234, 417347, 142.85555), DeliveryInfo(64723, 338129, 179.98189), DeliveryInfo(241313, 387137, 105.46758), DeliveryInfo(163020, 83628, 25.42519), DeliveryInfo(432418, 413406, 99.18584), DeliveryInfo(336561, 20627, 199.83517), DeliveryInfo(60442, 352002, 43.79069), DeliveryInfo(287428, 66136, 20.41799), DeliveryInfo(113676, 175115, 196.69122), DeliveryInfo(179046, 298495, 82.16702), DeliveryInfo(39098, 188132, 84.37139), DeliveryInfo(424643, 417548, 91.24757), DeliveryInfo(372239, 430343, 11.19065)};
const std::vector<unsigned> depotsLondonExtreme1 = {59};
const float right_turn_penaltyLondonExtreme1 = 15.000000000;
const float left_turn_penaltyLondonExtreme1 = 15.000000000;
const float truck_capacityLondonExtreme1 = 1212.614257812;

extern std::vector<std::vector<pathTime>> pathTimesLondonExtreme1; // from pickup/dropoff to pickup/dropoff/depot
extern std::vector<std::vector<pathTime>> depotTimesLondonExtreme1; // from all depots to all pickups
        
const std::vector<DeliveryInfo> deliveriesLondonExtreme2 = {DeliveryInfo(34879, 389264, 137.08173), DeliveryInfo(291829, 231525, 71.91736), DeliveryInfo(129725, 383125, 122.56682), DeliveryInfo(195441, 389264, 23.21515), DeliveryInfo(89516, 394484, 18.25418), DeliveryInfo(89516, 76650, 147.26566), DeliveryInfo(89516, 310581, 59.99919), DeliveryInfo(286772, 17241, 21.01382), DeliveryInfo(394891, 31461, 158.01956), DeliveryInfo(66940, 347829, 162.01428), DeliveryInfo(343938, 41336, 191.56882), DeliveryInfo(89516, 130528, 18.27139), DeliveryInfo(343938, 83342, 178.26596), DeliveryInfo(422492, 66208, 75.63104), DeliveryInfo(135963, 409382, 186.29137), DeliveryInfo(143440, 49854, 70.28852), DeliveryInfo(64254, 293818, 97.13382), DeliveryInfo(36527, 138649, 146.55731), DeliveryInfo(242272, 96989, 68.50053), DeliveryInfo(219488, 257177, 189.17386), DeliveryInfo(343938, 83342, 11.40528), DeliveryInfo(335283, 31461, 144.21942), DeliveryInfo(89516, 272137, 15.51426), DeliveryInfo(150084, 187224, 88.62766), DeliveryInfo(116559, 394484, 145.45842), DeliveryInfo(25457, 17241, 23.84434), DeliveryInfo(143440, 147035, 189.91982), DeliveryInfo(105571, 114243, 19.79551), DeliveryInfo(69656, 138649, 148.92365), DeliveryInfo(343938, 17241, 93.21634), DeliveryInfo(360534, 394484, 45.94852), DeliveryInfo(105571, 23238, 180.90318), DeliveryInfo(343938, 257177, 177.74478), DeliveryInfo(89516, 257177, 118.59480), DeliveryInfo(274269, 24644, 89.59936), DeliveryInfo(105571, 69040, 50.64853), DeliveryInfo(89516, 83342, 55.16647), DeliveryInfo(403738, 118970, 140.38144), DeliveryInfo(400133, 158490, 152.14987), DeliveryInfo(86129, 158490, 26.47551), DeliveryInfo(231240, 121008, 199.44727), DeliveryInfo(59697, 259279, 118.72143), DeliveryInfo(2586, 158490, 1.91211), DeliveryInfo(152228, 158490, 99.38187), DeliveryInfo(260440, 76650, 62.36740), DeliveryInfo(264388, 234780, 136.90015), DeliveryInfo(62758, 318743, 106.40794), DeliveryInfo(143440, 390891, 17.67936), DeliveryInfo(254647, 286103, 0.43400), DeliveryInfo(143440, 155660, 149.67912), DeliveryInfo(89516, 138649, 160.95218), DeliveryInfo(33082, 247326, 122.76397), DeliveryInfo(249835, 314504, 139.63599), DeliveryInfo(429827, 362691, 168.15720), DeliveryInfo(343938, 158490, 115.99158), DeliveryInfo(89516, 343518, 29.86299), DeliveryInfo(179361, 204300, 116.28070), DeliveryInfo(354374, 310032, 185.58627), DeliveryInfo(143440, 168741, 34.75976), DeliveryInfo(336040, 40447, 88.45197), DeliveryInfo(343938, 394685, 65.40082), DeliveryInfo(143440, 17241, 163.03503), DeliveryInfo(319729, 394484, 64.14413), DeliveryInfo(143440, 17241, 101.15028), DeliveryInfo(143440, 25775, 25.66063), DeliveryInfo(11296, 338914, 64.33150)};
const std::vector<unsigned> depotsLondonExtreme2 = {68};
const float right_turn_penaltyLondonExtreme2 = 15.000000000;
const float left_turn_penaltyLondonExtreme2 = 15.000000000;
const float truck_capacityLondonExtreme2 = 1620.838867188;

extern std::vector<std::vector<pathTime>> pathTimesLondonExtreme2; // from pickup/dropoff to pickup/dropoff/depot
extern std::vector<std::vector<pathTime>> depotTimesLondonExtreme2; // from all depots to all pickups

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