#include "fillStructs.h"

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "OSMID.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"

#include <iostream>

void populateData::initialize(infoStrucs &info, mapBoundary &xy){
    populateOSMWayInfo(info);
    populateStreetSegInfo(info);
    populateIntersectionInfo(info);
    populateFeatureInfo(info, xy);
    populatePOIInfo(info);
    
    info.lastIntersection.clear();
    info.lastPOI.clear();
    info.lastSeg.clear();
    info.showSubway = false;
}

void populateData::loadAfterDraw(infoStrucs &info){
    populateOSMSubwayInfo(info);
}

void populateData::populateOSMWayInfo(infoStrucs &info){
    info.WayMap.clear();
    const OSMWay* currentWayPtr;
    OSMID currentID;
    
    for(unsigned i=0 ; i<getNumberOfWays() ; i++){
        currentWayPtr = getWayByIndex(i);
        currentID = currentWayPtr->id();
        info.WayMap.insert({currentID,currentWayPtr});
    }
}

int populateData::getRoadType(const OSMWay* wayPtr){
    if(wayPtr == NULL){
        return SERVICE;
    }
    
    for(unsigned i=0 ; i < getTagCount(wayPtr) ; i++){
        std::string key,value;
        std::tie(key,value) = getTagPair(wayPtr,i);
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        
        if(key == "highway"){
            if(value == "motorway" || value == "motorway_link"){
                return HIGHWAY;
            } else if (value == "trunk" || value == "trunk_link" || value == "primary"){
                return PRIMARY;
            } else if (value == "secondary" || value == "tertiary" || value == "residential"){
                return RESIDENTIAL;
            } else {
                return SERVICE;
            }
        }
    }
    return SERVICE;
}

bool populateData::isFeatureOpen(LatLon pt1, LatLon pt2){
    if((pt1.lat() == pt2.lat()) && (pt1.lon() == pt2.lon())){
        return false;
    } else {
        return true;
    }
}

void populateData::populateStreetSegInfo(infoStrucs &info){
    int numStreetSegments = getNumStreetSegments();
    info.StreetSegInfo.resize(numStreetSegments);
    
    for(int i=0;i<numStreetSegments;i++){
        info.StreetSegInfo[i].fromIntersection = getInfoStreetSegment(i).from;
        info.StreetSegInfo[i].toIntersection = getInfoStreetSegment(i).to;
        info.StreetSegInfo[i].numCurvePoints = getInfoStreetSegment(i).curvePointCount;
        info.StreetSegInfo[i].id = getInfoStreetSegment(i).wayOSMID;//is the OSMID unique?
        info.StreetSegInfo[i].wayPtr = info.WayMap[info.StreetSegInfo[i].id];
        info.StreetSegInfo[i].type = getRoadType(info.StreetSegInfo[i].wayPtr);
        info.StreetSegInfo[i].name = getStreetName((getInfoStreetSegment(i).streetID));//gives the name of the street segment (for use in putting the names))
        info.StreetSegInfo[i].streetID=getInfoStreetSegment(i).streetID;
<<<<<<< HEAD
        info.StreetSegInfo[i].clicked = false;
=======
        
>>>>>>> Created functions to change the size of the POI's and the roads
    }
}

void populateData::populateIntersectionInfo(infoStrucs &info){
    int numOfIntersections = getNumIntersections();
    info.IntersectionInfo.resize(numOfIntersections);
    
    for(int i=0;i<numOfIntersections;++i){
        info.IntersectionInfo[i].position = getIntersectionPosition(i);
        info.IntersectionInfo[i].name = getIntersectionName(i);
        info.IntersectionInfo[i].clicked = false;
    }
}

void populateData::populateFeatureInfo(infoStrucs &info, mapBoundary &xy){
    int numFeatures = getNumFeatures();
    int numPoints;
    LatLon newPoint, pt1, pt2;
    double xNew, yNew;
    
    info.FeatureInfo.resize(numFeatures);
    info.FeaturePointVec.resize(numFeatures);
    
    for(int i=0;i<numFeatures;i++){
        info.FeatureInfo[i].name = getFeatureName(i);
        info.FeatureInfo[i].featureType = getFeatureType(i);
        info.FeatureInfo[i].id = getFeatureOSMID(i);
        
        numPoints = getFeaturePointCount(i);
        
        if(info.FeatureInfo[i].isOpen){
            info.FeatureInfo[i].priorityNum = 4;
        }else if(numPoints < 5){
            info.FeatureInfo[i].priorityNum = 3; 
        }else if (numPoints < 15){
            info.FeatureInfo[i].priorityNum = 2;
        }else{
            info.FeatureInfo[i].priorityNum = 1;
        }

        info.FeaturePointVec[i].clear();
        
        pt1 = getFeaturePoint(0, i);
        pt2 = getFeaturePoint(numPoints-1, i);
        info.FeatureInfo[i].isOpen = isFeatureOpen(pt1,pt2);
        
        for(int p=0 ; p<numPoints; p++){
            newPoint = getFeaturePoint(p, i);
            
            xNew = xy.xFromLon(newPoint.lon());
            yNew = xy.yFromLat(newPoint.lat());
            
            info.FeaturePointVec[i].push_back(ezgl::point2d(xNew,yNew));
        }
    }
}

void populateData::populatePOIInfo(infoStrucs &info){
    int numPOI = getNumPointsOfInterest();
   
    info.POIInfo.resize(numPOI);
        
    for(int i=0 ; i<numPOI ; i++){
        info.POIInfo[i].name = getPointOfInterestName(i);
        info.POIInfo[i].type = getPointOfInterestType(i);
        info.POIInfo[i].clicked = false;
    }
}

void populateData::populateOSMSubwayInfo(infoStrucs &info){
    bool isSubway = false;
    const OSMNode* currentPtr;

    info.SubwayInfo.clear();
    
    for(unsigned i=0 ; i< getNumberOfNodes(); i++){
        currentPtr = getNodeByIndex(i);
        isSubway = checkIfSubway(currentPtr);
        
        if(isSubway){
            subwayData newStop;
            newStop.name = getOSMNodeName(currentPtr);
            newStop.nodePtr = currentPtr;
            newStop.clicked = false;
            newStop.point = currentPtr->coords();
            newStop.id = currentPtr->id();
            
            info.SubwayInfo.push_back(newStop);
        }
    }    
}

bool populateData::checkIfSubway(const OSMNode* nodePtr){
    if(nodePtr == NULL){
        return false;
    }
    
    for(unsigned i=0 ; i < getTagCount(nodePtr) ; i++){
        std::string key,value;
        std::tie(key,value) = getTagPair(nodePtr,i);
        
        if(key == "railway" && value == "station"){
            return true;
        }
    }
    
    return false;
}

std::string populateData::getOSMNodeName(const OSMNode* nodePtr){
    std::string name = "";
    
    for(unsigned i=0 ; i < getTagCount(nodePtr) ; i++){
        std::string key,value;
        std::tie(key,value) = getTagPair(nodePtr,i);
        
        if(key == "name"){
            name = value;
        }
    }
    
    return name;
}

//void populateData::getOSMSubwayRelations(infoStrucs &info){
//    bool isSubwayRoute = false;
//    const OSMRelation* currentPtr;
//    const OSMWay* wayPtr;
//
//    info.SubwayInfo.clear();
//    for(unsigned i=0 ; i< getNumberOfRelations(); i++){
//        currentPtr = getRelationByIndex(i);
//        isSubwayRoute = checkIfSubwayRoute(currentPtr, info);
//        
//        if(isSubwayRoute){
//            subwayRouteData newRoute;
//            for(unsigned i=0 ; i < currentPtr->members().size() ; i++){ //many ways
//                wayPtr = info.WayMap[OSMID(currentPtr->members().at(i).tid)];
//                newRoute.wayPtr.push_back(wayPtr); 
//                for(unsigned i=0 ; i<wayPtr->ndrefs().size() ; i++){ //many nodes
//                    newRoute.nodePoints.push_back(wayPtr->ndrefs().at(i));
//                }
//            }
//            info.SubwayRouteInfo.push_back(newRoute);
//        }
//    }
//}
//
//bool populateData::checkIfSubwayRoute(const OSMRelation* relPtr){
//    if(relPtr == NULL){
//        return false;
//    }
//
//    if(checkOSMRelationTags(relPtr,"type","route") && checkOSMRelationTags(relPtr,"route","subway")){
//        return true;
//    } else {
//        return false;
//    }
//}
//    
//
//void populateData::checkOSMRelationTags(const OSMRelation* relPtr, std::string k, std::string v){
//    for(unsigned i=0 ; i < getTagCount(relPtr) ; i++){
//        std::string key,value;
//        std::tie(key,value) = getTagPair(relPtr,i);
//        
//        if(key == k && value == v){
//            return true;
//        }
//    }
//    return false;
//}