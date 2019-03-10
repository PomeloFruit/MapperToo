#pragma once
#include <vector>

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

class directionInfo {
public:
    std::vector< Node > Nodes;
    
    void fillNodes();
    void connectNodes();
};