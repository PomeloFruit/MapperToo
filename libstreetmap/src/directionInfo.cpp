#include "directionInfo.h"

#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "LatLon.h"
#include "m3.h"
#include "m3Helpers.h"
#include <vector>
#include <math.h>
#include <iostream>

#define SLIGHTTURNANGLE 0.523599 //30 degrees in radians
#define NOTURNANGLE 0.261799
#define NOINTERSECTION -100
#define SAMESTREET -99
#define NOTURN 0
#define SLIGHTTURN 1
#define REGULARTURN 2


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


void HumanInfo::fillInfo(std::vector<unsigned> path){
    
    for(int i=0 ;i<path.size();i++){
        navInstruction filler;
        Hum.humanInstructions.push_back(filler);
    }
    clear();
    fillDistance(path);
    fillStreets(path);
    fillTurn(path);    
    
}

void HumanInfo::fillDistance(std::vector<unsigned> path){
    double distance=0;
    double time=0;

    for(int i=0 ; i<path.size() ; i++){

        double tempDistance=find_street_segment_length(path[i]);

        distance=distance+tempDistance;
        Hum.humanInstructions[i].distance=tempDistance;

        time=time+tempDistance/find_street_segment_travel_time(path[i]);
    }

        int intDistance=static_cast<int> (distance);
    int rem = intDistance % 10;
    intDistance=rem >= 5 ? (intDistance - rem + 10) : (intDistance - rem);
    setDistanceTime(intDistance, time);
}

void HumanInfo::fillStreets(std::vector<unsigned> path){
    if(path.size()>0){
        InfoStreetSegment prevInfo=getInfoStreetSegment(path[0]);
        std::string prevName=getStreetName(prevInfo.streetID);
        for(int i=1;i<path.size();i++){
            InfoStreetSegment curInfo=getInfoStreetSegment(path[i]);
            std::string curName=getStreetName(curInfo.streetID);
            Hum.humanInstructions[i-1].onStreet=prevName;
            Hum.humanInstructions[i-1].nextStreet=curName;
            prevName=curName;
        }
        if(path.size()==1){
            Hum.humanInstructions[0].onStreet=prevName;
            Hum.humanInstructions[0].nextStreet=prevName;

        }
    }
}


void HumanInfo::fillTurn(std::vector<unsigned> path){
    if(path.size()>1){
        for(int i=1;i<path.size();i++){
            double angle=findAngleBetweenSegs(path[i-1], path[i]);
            double angleSeverity=abs(angle);
            int turnSeverity;
            if(angleSeverity>M_PI){
                angleSeverity=angleSeverity-M_PI;
            }
            if(angleSeverity<SLIGHTTURNANGLE && angleSeverity>NOTURNANGLE){
                turnSeverity=SLIGHTTURN;
            }
            else if(angleSeverity<NOTURNANGLE){
                turnSeverity=NOTURN;
            }
            else{
                turnSeverity=REGULARTURN;
            }
            // if turn angle -PI < dAngle < 0 or if dAngle > PI
            if(angle == SAMESTREET || NOTURN){ // same id
                Hum.humanInstructions[i-1].turnType=HumanTurnType::STRAIGHT;     
            } else if(angle == NOINTERSECTION){ // segments dont intersect
                Hum.humanInstructions[i-1].turnType=HumanTurnType::NONE;     
            } else if(((angle < 0 && angle > -M_PI)|| angle > M_PI)&&(turnSeverity==REGULARTURN)){ //negative angle
                Hum.humanInstructions[i-1].turnType=HumanTurnType::LEFT;
            } else if(turnSeverity==REGULARTURN) {
                Hum.humanInstructions[i-1].turnType=HumanTurnType::RIGHT;
            } else if((angle < 0 && angle > -M_PI)|| angle > M_PI){
                Hum.humanInstructions[i-1].turnType=HumanTurnType::SLIGHTLEFT;
            } else{
                Hum.humanInstructions[i-1].turnType=HumanTurnType::SLIGHTRIGHT;
            }

        }
    }
    else if(path.size()==1){
        Hum.humanInstructions[0].turnType=HumanTurnType::STRAIGHT;
    }
}

void HumanInfo::setStartStop(std::string start, std::string stop){
    Hum.startIntersection = start;
    Hum.endIntersection = stop;
}


void HumanInfo::setDistanceTime(int distance, double time){
    time=ceil(time);
    if(distance>=1000){
        Hum.totDistancePrint=std::to_string(distance)+" KM";
    }
    else{
        Hum.totDistancePrint=std::to_string(distance)+" M";
    }
    
    int intTime=static_cast<int> (time);
    int hours=intTime/3600;
    intTime=intTime-hours*3600;
    int minutes=intTime/60;
    intTime=intTime-minutes*60;
    int seconds=intTime/60;
    
    if(hours==0&&minutes!=0){
        Hum.totTimePrint=std::to_string(minutes)+" min";
    }
    else if(hours==0&&minutes==0){
        Hum.totTimePrint=std::to_string(seconds)+" sec";
    }
    else{
        Hum.totTimePrint=std::to_string(hours)+" h "+std::to_string(minutes)+" min";
    }
    
}


void HumanInfo::clear(){
    Hum.endIntersection.clear();
    Hum.startIntersection.clear();
    Hum.totDistancePrint.clear();
    Hum.totTimePrint.clear();
    for(int i=0;i<static_cast<int>(Hum.humanInstructions.size());i++){
        Hum.humanInstructions[i].distance=0;
        Hum.humanInstructions[i].distancePrint.clear();
        Hum.humanInstructions[i].nextStreet.clear();
        Hum.humanInstructions[i].onStreet.clear();
        Hum.humanInstructions[i].turnPrint.clear();
    }
}