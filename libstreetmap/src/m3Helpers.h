#pragma once

#include "directionInfo.h"

// this header contains all the helper functions needed for providing navigation
// and direction calcultions in m3

//=============================== Constants ===============================

#define NOINTERSECTION -100
#define SAMESTREET -99

//=========================== Function Prototypes ===========================

// finds the path between sourceNode and node with intersection Id of destID
std::vector<unsigned> getPath(Node *sourceNode, const unsigned destID, const double rtPen, const double ltPen);

// adds the turn time between existingSeg and newSeg and the travel time of newSeg
double travelTimeAdd(unsigned existingSeg, unsigned newSeg, const double rt_penalty, const double lt_penalty);

// calculates the angle between street_segment1 and street_segment2, relative to the x axis
double findAngleBetweenSegs(unsigned street_segment1, unsigned street_segment2);

// finds the angle between ptFrom->ptCommon and ptCommon->ptTo
double findAngleBetweenThreePoints(LatLon ptFrom, LatLon ptCommon, LatLon ptTo);

// calculates the score of the newPoint in the wavefront
// score = (time from start to newPoint) + ((smallest distance from newPoint->end)/fastest speed limit) 
double getNewScore(unsigned newPoint, LatLon end, double time);

// back-tracks the node information to get segment ids from start->end
std::vector<unsigned> getFinalPath(Node *currNode, unsigned start);
std::vector<unsigned> getFinalPath4(Node *currNode, unsigned start, std::vector<unsigned>& reachingEdge, std::vector<Node *>& reachingNode);