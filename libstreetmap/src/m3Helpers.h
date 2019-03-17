#pragma once

#include "directionInfo.h"

// this header contains all the helper functions needed for providing navigation
// and direction calcultions in m3

//=============================== Constants ===============================

#define NOINTERSECTION -100
#define SAMESTREET -99

//=========================== Function Prototypes ===========================

std::vector<unsigned> getPath(Node *sourceNode, const unsigned destID, const double rtPen, const double ltPen);

double travelTimeAdd(unsigned existingSeg, unsigned newSeg, const double rt_penalty, const double lt_penalty);

double findAngleBetweenSegs(unsigned street_segment1, unsigned street_segment2);

double findAngleBetweenThreePoints(LatLon ptFrom, LatLon ptCommon, LatLon ptTo);

double getNewScore(unsigned newPoint, LatLon end, double time);

std::vector<unsigned> getFinalPath(Node *currNode,unsigned start);