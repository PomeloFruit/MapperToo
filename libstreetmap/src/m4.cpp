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
#include "m4.h"

//struct DeliveryInfo {
//    //Specifies a delivery order (input to your algorithm).
//    //
//    //To satisfy the order the item-to-be-delivered must have been picked-up 
//    //from the pickUp intersection before visiting the dropOff intersection.
//
//    DeliveryInfo(unsigned pick_up, unsigned drop_off, float weight)
//        : pickUp(pick_up), dropOff(drop_off), itemWeight(weight) {}
//
//    //The intersection id where the item-to-be-delivered is picked-up.
//    unsigned pickUp;
//
//    //The intersection id where the item-to-be-delivered is dropped-off.
//    unsigned dropOff;
//
//    // Weight of the item in pounds (lb)
//    float itemWeight;
//};
//
//
//struct CourierSubpath {
//    // Specifies one subpath of the courier truck route
//
//    // The intersection id where a start depot, pick-up intersection or drop-off intersection 
//    // is located
//    unsigned start_intersection;
//
//    // The intersection id where this subpath ends. This must be the 
//    // start_intersection of the next subpath or the intersection of an end depot
//    unsigned end_intersection;
//
//    // Street segment ids of the path between start_intersection and end_intersection 
//    // They form a connected path (see m3.h)
//    std::vector<unsigned> subpath;
//
//    // Specifies the indices from the deliveries vector of the picked up 
//    // delivery items at the start_intersection (if a pick up is to be made)
//    // Will be length zero if no delivery item is picked up at the start intersection
//    std::vector<unsigned> pickUp_indices;
//};

// This routine takes in a vector of N deliveries (pickUp, dropOff
// intersection pairs), another vector of M intersections that
// are legal start and end points for the path (depots), right and left turn 
// penalties in seconds (see m3.h for details on turn penalties), 
// and the truck_capacity in pounds.
//
// The first vector 'deliveries' gives the delivery information.  Each delivery
// in this vector has pickUp and dropOff intersection ids and the weight (also
// in pounds) of the delivery item. A delivery can only be dropped-off after
// the associated item has been picked-up. 
// 
// The second vector 'depots' gives the intersection ids of courier company
// depots containing trucks; you start at any one of these depots and end at
// any one of the depots.
//
// This routine returns a vector of CourierSubpath objects that form a delivery route.
// The CourierSubpath is as defined above. The first street segment id in the
// first subpath is connected to a depot intersection, and the last street
// segment id of the last subpath also connects to a depot intersection.  The
// route must traverse all the delivery intersections in an order that allows
// all deliveries to be made with the given truck capacity. Addionally, a package
// should not be dropped off if you haven't picked it up yet.
//
// The start_intersection of each subpath in the returned vector should be 
// at least one of the following (a pick-up and/or drop-off can only happen at 
// the start_intersection of a CourierSubpath object):
//      1- A start depot.
//      2- A pick-up location (and you must specify the indices of the picked 
//                              up orders in pickup_indices)
//      3- A drop-off location. 
//
// You can assume that N is always at least one, M is always at least one
// (i.e. both input vectors are non-empty), and truck_capacity is always greater
// or equal to zero.
//
// It is legal for the same intersection to appear multiple times in the pickUp
// or dropOff list (e.g. you might have two deliveries with a pickUp
// intersection id of #50). The same intersection can also appear as both a
// pickUp location and a dropOff location.
//        
// If you have two pickUps to make at an intersection, traversing the
// intersection once is sufficient to pick up both packages, as long as the
// truck_capcity fits both of them and you properly set your pickup_indices in
// your courierSubpath.  One traversal of an intersection is sufficient to
// drop off all the (already picked up) packages that need to be dropped off at
// that intersection.
//
// Depots will never appear as pickUp or dropOff locations for deliveries.
//  
// If no valid route to make *all* the deliveries exists, this routine must
// return an empty (size == 0) vector.


std::vector<CourierSubpath> traveling_courier(
		const std::vector<DeliveryInfo>& deliveries,
	       	const std::vector<unsigned>& depots, 
		const float right_turn_penalty, 
		const float left_turn_penalty, 
		const float truck_capacity){
    
    std::vector<CourierSubpath> courierPath;
    double minDistance = 999999999999;
    unsigned bestDepot = depots.at(0);
    unsigned bestStartDelivery = 0;
    
//    for(unsigned i=0 ; i<deliveries.size() ; i++){
        LatLon deliveryLL = getIntersectionPosition(deliveries.at(0).pickUp);
        
        for(unsigned j=0 ; j<depots.size(); j++){
            
            LatLon depotLL = getIntersectionPosition(depots.at(j));
            double temp = find_distance_between_two_points(depotLL, deliveryLL);
            if(temp<minDistance){
                minDistance = temp;
                bestDepot = depots.at(j);
                //bestStartDelivery = i;
            }
        }
//    }
    
        CourierSubpath startSubpath;
        startSubpath.subpath = find_path_between_intersections(bestDepot, deliveries.at(0).pickUp, right_turn_penalty, left_turn_penalty);
        startSubpath.start_intersection = bestDepot;
        startSubpath.end_intersection = deliveries.at(0).pickUp;
        startSubpath.pickUp_indices.clear();
        
        courierPath.push_back(startSubpath);
        
        for(unsigned i=0 ; i<deliveries.size() ; i++){
            CourierSubpath newSubpath;
            newSubpath.subpath = find_path_between_intersections(deliveries.at(i).pickUp, deliveries.at(i).dropOff, right_turn_penalty, left_turn_penalty);
            newSubpath.start_intersection = deliveries.at(i).pickUp;
            newSubpath.end_intersection = deliveries.at(i).dropOff;
            newSubpath.pickUp_indices.push_back(i);
            courierPath.push_back(newSubpath);
            
            if(i < deliveries.size()-1){
                CourierSubpath newSubpath1;
                newSubpath1.subpath = find_path_between_intersections(deliveries.at(i).dropOff, deliveries.at(i+1).pickUp, right_turn_penalty, left_turn_penalty);
                newSubpath1.start_intersection = deliveries.at(i).dropOff;
                newSubpath1.end_intersection = deliveries.at(i+1).pickUp;
                newSubpath1.pickUp_indices.clear();
                courierPath.push_back(newSubpath1);
            }
        }
        
        
        LatLon finalLL = getIntersectionPosition(deliveries.at(deliveries.size()-1).dropOff);
        minDistance = 99999999999999999;
        
        for(unsigned j=0 ; j<depots.size(); j++){
            
            LatLon depotLL = getIntersectionPosition(depots.at(j));
            double temp = find_distance_between_two_points(depotLL, finalLL);
            if(temp<minDistance){
                minDistance = temp;
                bestDepot = depots.at(j);
            }
        }
        
        CourierSubpath finalSubpath;
        finalSubpath.subpath = find_path_between_intersections(deliveries.at(deliveries.size()-1).dropOff, bestDepot, right_turn_penalty, left_turn_penalty);
        finalSubpath.start_intersection = deliveries.at(deliveries.size()-1).dropOff;
        finalSubpath.end_intersection = bestDepot;
        finalSubpath.pickUp_indices.clear();
        
        courierPath.push_back(finalSubpath);     
        
        return courierPath;
}
    
    //struct CourierSubpath {
//    // Specifies one subpath of the courier truck route
//
//    // The intersection id where a start depot, pick-up intersection or drop-off intersection 
//    // is located
//    unsigned start_intersection;
//
//    // The intersection id where this subpath ends. This must be the 
//    // start_intersection of the next subpath or the intersection of an end depot
//    unsigned end_intersection;
//
//    // Street segment ids of the path between start_intersection and end_intersection 
//    // They form a connected path (see m3.h)
//    std::vector<unsigned> subpath;
//
//    // Specifies the indices from the deliveries vector of the picked up 
//    // delivery items at the start_intersection (if a pick up is to be made)
//    // Will be length zero if no delivery item is picked up at the start intersection
//    std::vector<unsigned> pickUp_indices;
//};