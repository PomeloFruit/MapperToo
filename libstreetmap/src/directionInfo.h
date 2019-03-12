#pragma once
#include <vector>
#include <string>

#define NONODE -1
#define NOEDGE -1
#define NOTIME 9999999999

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
    
    waveElem(unsigned from, Node* source, unsigned id, double time);
};

class DirectionInfo {
public:
    std::vector< Node > Nodes;
    
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