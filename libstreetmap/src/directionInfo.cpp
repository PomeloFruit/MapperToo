#include "directionInfo.h"

#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "LatLon.h"
#include <vector>

waveElem::waveElem(unsigned from, Node* source, unsigned id, double time, double scoreIn){
    node = source;
    edgeID = id;
    travelTime = time;
    reachingNode = from;
    score = scoreIn;
}

void DirectionInfo::fillNodes(){
    int numOfIntersections = getNumIntersections();
    Nodes.resize(numOfIntersections);
    intersectionPos.resize(numOfIntersections);
    
    for(int i=0 ; i<numOfIntersections ; i++){
        Nodes[i].reachingNode = NULL;
        Nodes[i].reachingEdge = NOEDGE;
        Nodes[i].bestTime = NOTIME;
        Nodes[i].id = i;
        Nodes[i].bestScore = NOSCORE;
        intersectionPos[i] = getIntersectionPosition(i);
    }
    
    connectNodes();
    findFastestStreet();
}

void DirectionInfo::connectNodes(){
    int numOfIntersections = getNumIntersections();
    
    for(int i=0 ; i<numOfIntersections ; i++){
        std::vector<unsigned> tempSegs;
        tempSegs = find_intersection_street_segments(i); 
        Nodes[i].toNodes.clear();
        Nodes[i].outEdges.clear();
        
        for(int j=0 ; j<static_cast<int>(tempSegs.size()) ; j++){
            
            int segTo, segFrom;
            segTo = getInfoStreetSegment(tempSegs[j]).to;
            segFrom = getInfoStreetSegment(tempSegs[j]).from;
            bool oneWay = getInfoStreetSegment(tempSegs[j]).oneWay;
            
            if(segTo == i){
                if(!oneWay){
                    Nodes[i].toNodes.push_back(&Nodes[segFrom]);
                    Nodes[i].outEdges.push_back(tempSegs[j]);
                }
            } else if(segFrom == i){
                Nodes[i].toNodes.push_back(&Nodes[segTo]);
                Nodes[i].outEdges.push_back(tempSegs[j]);
            }
        }
    }
}



void DirectionInfo::findFastestStreet(){
    double fastestKmH = 0;
    
    // search through all segments for fastest speed limit
    for(int i=0 ; i<getNumStreetSegments() ; i++){
        if(getInfoStreetSegment(i).speedLimit > fastestKmH){
            fastestKmH = getInfoStreetSegment(i).speedLimit;
        }
    }
    
    // 3600 (s/h) / (Km/h) = (s/km) ->> (s/km) * 0.001 (km/m) = (s/m)
    secPerMeter = (3600 / fastestKmH) * 0.001;
}