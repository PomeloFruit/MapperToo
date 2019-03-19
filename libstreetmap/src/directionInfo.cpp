#include "directionInfo.h"

#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "LatLon.h"
#include "m3.h"
#include "m3Helpers.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <sstream>
#include <iomanip>


#define SLIGHTTURNANGLE M_PI/4 //45 degrees in radians
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
    if(path.size()>0){
        InfoStreetSegment prevInfo=getInfoStreetSegment(path[0]);
        InfoStreetSegment curInfo;
        unsigned prevID=path[0];
        int numStreetsOnPath=1;
        std::vector<std::pair<unsigned, unsigned>> changedStreetIDSegs;
        for(int i=1;i<path.size();i++){
            curInfo=getInfoStreetSegment(path[i]);
            if(prevInfo.streetID!=curInfo.streetID){
                if(getStreetName(getInfoStreetSegment(prevID).streetID)!=getStreetName(curInfo.streetID)){
                    std::cout<< getStreetName(getInfoStreetSegment(prevID).streetID)<<" "<< getStreetName(curInfo.streetID)<<" "<<prevID<<" "<<path[i]<<'\n';
                    changedStreetIDSegs.push_back(std::make_pair(prevID, path[i]));
                    numStreetsOnPath++;
                }
            }
            prevID=path[i];
        }
        
        clear();
        for(int i=0;i<numStreetsOnPath;i++){
            navInstruction filler;
            Hum.humanInstructions.push_back(filler);
        }
        fillDistance(path, changedStreetIDSegs);
        fillStreets(path, changedStreetIDSegs);
        fillTurn(path, changedStreetIDSegs);
    }else{
        clear();
    }
    
}

void HumanInfo::fillDistance(std::vector<unsigned> path, std::vector<std::pair<unsigned, unsigned>> changedStreetIDSegs){
    double distance=0;
    double totalDistance=0;
    double totalTime=0;
    double time=0;
    int intDistance=0;
    int numStreetsChanged=0;
    int curMarker=static_cast<int> (path[0]);
    int nextMarker=-1;
    if(changedStreetIDSegs.size()>0){
        nextMarker=changedStreetIDSegs[0].first;
    }
    if(path.size()>1){
        for(int i=0 ; i<path.size() ; i++){
            curMarker=static_cast<int> (path[i]);
            double tempDistance=find_street_segment_length(path[i]);
            distance=distance+tempDistance;
            if(changedStreetIDSegs.size()>0&&(curMarker==nextMarker)){
                totalDistance=totalDistance+distance;
                totalTime=totalTime+time;
                Hum.humanInstructions[numStreetsChanged].distance=distance;
                intDistance=static_cast<int> (distance);
                int rem = intDistance % 10;
                intDistance=rem >= 5 ? (intDistance - rem + 10) : (intDistance - rem);
                setDistanceTimeStreet(intDistance, numStreetsChanged);
                distance=0;
                time=0;
                numStreetsChanged++;
                if(numStreetsChanged<=changedStreetIDSegs.size()){
                    if((numStreetsChanged)!=changedStreetIDSegs.size()){
                        nextMarker=static_cast<int> (changedStreetIDSegs[numStreetsChanged].first);
                    }
                    else{
                        nextMarker=static_cast<int> (path[path.size()-1]);
                    }
                }
            }
        }
    }
    else if(path.size()==1){
        totalDistance=find_street_segment_length(path[0]);
        totalTime=find_street_segment_travel_time(path[0]);
    }
    if(path.size()!=1&&numStreetsChanged==0){
        for(int i=0;i<path.size();i++){
            totalDistance=totalDistance+find_street_segment_length(path[i]);
        }
        intDistance=static_cast<int> (totalDistance);
        setDistanceTimeStreet(intDistance, numStreetsChanged);
    }
    intDistance=static_cast<int> (totalDistance);
    int rem = intDistance % 10;
    intDistance=rem >= 5 ? (intDistance - rem + 10) : (intDistance - rem);
    totalTime=compute_path_travel_time(path, 0, 0);
    setDistanceTime(intDistance, totalTime);
}

void HumanInfo::fillStreets(std::vector<unsigned> path, std::vector<std::pair<unsigned, unsigned>> changedStreetIDSegs){
    
    for(int i=0;i<changedStreetIDSegs.size();i++){
        InfoStreetSegment prevInfo=getInfoStreetSegment(changedStreetIDSegs[i].first);
        InfoStreetSegment curInfo=getInfoStreetSegment(changedStreetIDSegs[i].second);
        std::string prevName=getStreetName(prevInfo.streetID);
        std::string curName=getStreetName(curInfo.streetID);
        Hum.humanInstructions[i].onStreet=prevName;
        Hum.humanInstructions[i].nextStreet=curName;
    }
    if(changedStreetIDSegs.size()==0){
        InfoStreetSegment curInfo=getInfoStreetSegment(path[0]);
        std::string curName=getStreetName(curInfo.streetID);
        Hum.humanInstructions[0].onStreet=curName;
        Hum.humanInstructions[0].nextStreet=curName;
    } else{
        InfoStreetSegment curInfo=getInfoStreetSegment(changedStreetIDSegs[changedStreetIDSegs.size()-1].second);
        std::string curName=getStreetName(curInfo.streetID);
        Hum.humanInstructions[changedStreetIDSegs.size()].onStreet=curName;
        Hum.humanInstructions[changedStreetIDSegs.size()].nextStreet=curName;
    }
}


void HumanInfo::fillTurn(std::vector<unsigned> path, std::vector<std::pair<unsigned, unsigned>> changedStreetIDSegs){
    if(changedStreetIDSegs.size()>0){
        for(int i=0;i<changedStreetIDSegs.size();i++){
            double angle=findAngleBetweenSegs(changedStreetIDSegs[i].first, changedStreetIDSegs[i].second);
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
                std::string insert="straight";
                Hum.humanInstructions[i].turnType=HumanTurnType::STRAIGHT;
                Hum.humanInstructions[i].turnPrint=insert;
            } else if(angle == NOINTERSECTION){ // segments dont intersect
                std::string insert="AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH";
                Hum.humanInstructions[i].turnType=HumanTurnType::NONE;
                Hum.humanInstructions[i].turnPrint=insert;
            } else if(((angle < 0 && angle > -M_PI)|| angle > M_PI)&&(turnSeverity==REGULARTURN)){ //negative angle
                std::string insert="left";
                Hum.humanInstructions[i].turnType=HumanTurnType::LEFT;
                Hum.humanInstructions[i].turnPrint=insert;
            } else if(turnSeverity==REGULARTURN) {
                std::string insert="right";
                Hum.humanInstructions[i].turnType=HumanTurnType::RIGHT;
                Hum.humanInstructions[i].turnPrint=insert;
            } else if((angle < 0 && angle > -M_PI)|| angle > M_PI){
                std::string insert="slightly left";
                Hum.humanInstructions[i].turnType=HumanTurnType::SLIGHTLEFT;
                Hum.humanInstructions[i].turnPrint=insert;
            } else{std::string insert="slightly right";
                Hum.humanInstructions[i].turnType=HumanTurnType::SLIGHTRIGHT;
                Hum.humanInstructions[i].turnPrint=insert;
            }

        }
    }
    
    //the last turn
    Hum.humanInstructions[changedStreetIDSegs.size()].turnType=HumanTurnType::STRAIGHT;
    std::string insert="straight";
    Hum.humanInstructions[changedStreetIDSegs.size()].turnPrint=insert;
}

void HumanInfo::setStartStop(std::string start, std::string stop){
    Hum.startIntersection = start;
    Hum.endIntersection = stop;
}


void HumanInfo::setDistanceTime(int distance, double time){
    const double SEC_PER_MIN = 60.0;
    const double MIN_PER_HOUR = 60.0; 
    //std::cout<<distance<<'\n';
    if(distance >= 1000){
        double tempDistance=static_cast<double> (distance);
        tempDistance=tempDistance/100;
        tempDistance=round(tempDistance);
        tempDistance=tempDistance/10;
        std::stringstream editor;
        editor<<std::fixed<<std::setprecision(1)<<tempDistance;
        std::string insert=editor.str();
        Hum.totDistancePrint=insert+" km";
    } else {
        Hum.totDistancePrint=std::to_string(distance)+" m";
    }

    
    int hour = time/(SEC_PER_MIN*MIN_PER_HOUR); 
    time = time - hour*(SEC_PER_MIN*MIN_PER_HOUR); 
    
    int min = time/(SEC_PER_MIN); 

    if(hour == 0 && min != 0){
        min = ceil(time/(SEC_PER_MIN));
        Hum.totTimePrint=std::to_string(min)+" min";
    }
    else if(hour == 0 && min == 0){
        Hum.totTimePrint = "< 1 min";
    }
    else{
        Hum.totTimePrint=std::to_string(hour)+" h "+std::to_string(min)+" min";
    }
    
}

void HumanInfo::setDistanceTimeStreet(int distance, int index){
    if(distance>=1000){
        double tempDistance=static_cast<double> (distance);
        tempDistance=tempDistance/100;
        tempDistance=round(tempDistance);
        tempDistance=tempDistance/10;
        std::stringstream editor;
        editor<<std::fixed<<std::setprecision(1)<<tempDistance;
        std::string insert=editor.str();
        Hum.humanInstructions[index].distancePrint=insert+" km";
    }
    else{
        Hum.humanInstructions[index].distancePrint=std::to_string(distance)+" m";
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
    Hum.humanInstructions.clear();
}