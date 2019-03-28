#pragma once

#include <vector>
#include <string>
#include "LatLon.h"

//============================== Constants ===================================

#define NONODE -1
#define NOEDGE -1
#define NOTIME 9999999999
#define NOSCORE 9999999999
#define SLIGHTTURNANGLE M_PI/4 //45 degrees in radians
#define NOTURNANGLE 0.261799
#define NOINTERSECTION -100
#define SAMESTREET -99
#define NOTURN 0
#define SLIGHTTURN 1
#define REGULARTURN 2

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

struct waveElem4 {
    unsigned reachingNode;
    Node* node;
    unsigned edgeID;
    double travelTime;
    
    // instanties the waveElem4 variables
    waveElem4(unsigned from, Node* source, unsigned id, double time);
};


enum class HumanTurnType {
    STRAIGHT, // going straight
    RIGHT, // turning right
    LEFT, // turning left
    SLIGHTRIGHT, //turning slightly right
    SLIGHTLEFT, //turning slightly left
    NONE // no turn detected
};

struct navInstruction{
    int distance;//The total distance spent on the street
    HumanTurnType turnType;//The turn that has to be made at the end of the time on the street
    std::string distancePrint;//The distance spent on street but processed into a string
    std::string nextStreet;//The street that will be turned onto
    std::string onStreet;//The street that the distance is covered on
    std::string turnPrint;//The turn type but processed
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
    
    std::vector< navInstruction > humanInstructions;//A vector containing navInstructions, each index is an instruction to be printed
    std::string startIntersection;//The intersection the user starts at
    std::string endIntersection;//The intersection the user ends at
    std::string totDistancePrint;//The total distance of the path but processed
    std::string totTimePrint;//The total time of the path but processed
    
    //creates changedStreetIDSegs and calls other functions
    void fillInfo(std::vector<unsigned> path);
    
    //Processes information required to fill all distance fields in the Hum object
    void fillDistance(std::vector<unsigned> path, std::vector<std::pair<unsigned, unsigned>> changedStreetIDSegs);
    
    //Processes information required to fill all street fields in the Hum object
    void fillStreets(std::vector<unsigned> path, std::vector<std::pair<unsigned, unsigned>> changedStreetIDSegs);
    
    //Processes information required to fill all turn fields in the Hum object
    void fillTurn(std::vector<std::pair<unsigned, unsigned>> changedStreetIDSegs);
    
    //Takes processed information and further processes it into a readable string
    void setDistanceTimeStreet(int distance, int index);
    
    //Sets the intersection fields in the Hum object
    void setStartStop(std::string start, std::string stop);
    
    //Takes processed information and further processes it into a readable string
    void setDistanceTime(int distance, double time);
    
    //clears every field of the Hum object
    void clear();
};

//=========================== Global Variables ================================

// first declared in m1.cpp load map
extern DirectionInfo Dir;

//first declared in m2.cpp just after searchOnMap
extern HumanInfo Hum;