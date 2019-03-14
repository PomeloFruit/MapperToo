#pragma once

#include <vector>
#include <string>
#include "LatLon.h"

//============================== Constants ===================================

#define NONODE -1
#define NOEDGE -1
#define NOTIME 9999999999
#define NOSCORE 9999999999

//============================== Structures ===================================

struct Node {
    std::vector< Node* > toNodes;
    std::vector< unsigned > outEdges;
    Node* reachingNode;
    
    unsigned id;
    unsigned reachingEdge;

    double bestTime;
};

struct waveElem {
    unsigned reachingNode;
    Node *node;
    unsigned edgeID;
    double travelTime;
    double score;
    
    // instanties the waveElem variables
    waveElem(unsigned from, Node* source, unsigned id, double time, double score);
};

enum class HumanTurnType {
    STRAIGHT, // going straight
    RIGHT, // turning right
    LEFT, // turning left
    SLIGHTRIGHT,
    SLIGHTLEFT,
    NONE // no turn detected
};

struct navInstruction{
    int distance;
    HumanTurnType turnType;
    std::string distancePrint;
    std::string nextStreet;
    std::string onStreet;
    std::string turnPrint;
};

//============================== Class ===================================

// direction info holds all the node data
class DirectionInfo {
public:
    // contains all the nodes (intersections) in the city with all needed info
    std::vector< Node > Nodes;
    
    // contains all the latlon positions of intersections
    std::vector< LatLon > intersectionPos;
    
    // the minimum seconds needed to travel a meter in the city
    double secPerMeter;
    
    // fills all nodes in Nodes    
    void fillNodes();
    
    // connects all nodes to each other, for the outEdges and toNodes
    void connectNodes();
    
    // finds the fastest speed limit in the city, and computes secPerMeter
    void findFastestStreet();
};

class HumanInfo {
public:
    std::vector< navInstruction > humanInstructions;
    std::string startIntersection;
    std::string endIntersection;
    std::string totDistancePrint;
    std::string totTimePrint;
    
    void fillInfo();
    void fillDistance();
    void fillStreets();
    void fillTurn();
    
    void setStartStop();
    void setDistanceTime();    
};

//=========================== Global Variables ================================

// first declared in m1.cpp load map
extern DirectionInfo Dir;


extern HumanInfo Hum;