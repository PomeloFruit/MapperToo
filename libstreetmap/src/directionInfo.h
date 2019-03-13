#pragma once
#include <vector>
#include <string>
#include "LatLon.h"

#define NONODE -1
#define NOEDGE -1
#define NOTIME 9999999999
#define NOSCORE 9999999999

struct Node {
    std::vector< Node* > toNodes;
    std::vector< unsigned > outEdges;
    Node* reachingNode;
    
    unsigned id;
    unsigned reachingEdge;

    double bestTime;
    double bestScore;
};

struct waveElem {
    unsigned reachingNode;
    Node *node;
    unsigned edgeID;
    double travelTime;
    double score;
    
    waveElem(unsigned from, Node* source, unsigned id, double time, double score);
};

class DirectionInfo {
public:
    std::vector< Node > Nodes;
    std::vector< LatLon > intersectionPos;
    
    void fillNodes();
    void connectNodes();
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

extern DirectionInfo Dir;
extern HumanInfo Hum;