#include "directionInfo.h"

#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "LatLon.h"
#include <vector>


/* waveElem constructor
 * - fills waveElem variables with parameters
 * 
 * @param none
 * 
 * @return void
 */

waveElem::waveElem(unsigned from, Node* source, unsigned id, double time, double scoreIn){
    node = source;
    edgeID = id;
    travelTime = time;
    reachingNode = from;
    score = scoreIn;
}


/* fillNodes function
 * - creates a node for every intersection on the map
 * - fills all nodes with pre determined default values
 * - gets the latlon position of the intersection for quick access later
 * 
 * @param none
 * 
 * @return void
 */

void DirectionInfo::fillNodes(){
    int numOfIntersections = getNumIntersections();
    Nodes.resize(numOfIntersections);
    intersectionPos.resize(numOfIntersections);
    
    for(int i=0 ; i<numOfIntersections ; i++){
        Nodes[i].reachingNode = NULL;
        Nodes[i].reachingEdge = NOEDGE;
        Nodes[i].bestTime = NOTIME;
        Nodes[i].id = i;
        intersectionPos[i] = getIntersectionPosition(i);
    }
    
    connectNodes();
    findFastestStreet();
}


/* connectNodes function
 * - adds pointers to outEdges and toNodes for connected nodes
 * - goes through all intersection and adds all reachable to nodes from every node
 * 
 * @param none
 * 
 * @return void
 */

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
            
            if(segTo == i){ // if connected segment reaches intersection at 'to' point
                if(!oneWay){ // make sure that it can travel to->from on segment
                    Nodes[i].toNodes.push_back(&Nodes[segFrom]);
                    Nodes[i].outEdges.push_back(tempSegs[j]);
                }
            } else if(segFrom == i){ // if connected segment reaches intersection at 'from' point
                Nodes[i].toNodes.push_back(&Nodes[segTo]);
                Nodes[i].outEdges.push_back(tempSegs[j]);
            }
        }
    }
}


/* findFastestStreet function
 * - goes through all the street segments and finds the maximum speed limit
 * - computes the minimum seconds to travel a meter in the map
 *  (very important for A* success to underestimate travel times)
 * 
 * @param none
 * 
 * @return void
 */

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