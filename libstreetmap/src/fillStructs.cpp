#include "fillStructs.h"

#include "globals.h"
#include "latLonToXY.h"

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "OSMID.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"

void populateData::initialize(infoStrucs info, mapBoundary xy){
    populateOSMWayInfo(info);
    populateStreetSegInfo(info);
    populateIntersectionInfo(info);
    populateFeatureInfo(info, xy);
    populatePOIInfo(info);
}


void populateData::populateOSMWayInfo(infoStrucs info){
    info.WayMap.clear();
    const OSMWay* currentWayPtr;
    OSMID currentID;
    
    for(unsigned i=0 ; i<getNumberOfWays() ; i++){
        currentWayPtr = getWayByIndex(i);
        currentID = currentWayPtr->id();
        info.WayMap.insert({currentID,currentWayPtr});
    }
}

//void populateData::populateOSMSubwayInfo(infoStrucs info){
//    SubwayInfo.clear();
//    subwayData newStop;
//    bool isSubway = false;
//    const OSMWay* currentPtr;
//
//    for(unsigned i=0 ; i< getNumberOfNodes(); i++){
//        currentPtr = getNodeByIndex(i);
//        isSubway = checkIfSubway(currentPtr);
//        
//        if(isSubway){
//            newStop.name = getOSMSubwayName(currentPtr);
//            SubwayInfo.insert(newStop);
//        }
//    }
//}

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
            } else if (value == "secondary" || value == "tertiary"){
                return SECONDARY;
            } else if (value == "residential"){
                return RESIDENTIAL;
            } else {
                return SERVICE;
            }
        }
    }
    return SERVICE;
}

void populateData::populateStreetSegInfo(infoStrucs info){
    int numStreetSegments = getNumStreetSegments();
    info.StreetSegInfo.resize(numStreetSegments);
    
    for(int i=0;i<numStreetSegments;i++){
        info.StreetSegInfo[i].fromIntersection = getInfoStreetSegment(i).from;
        info.StreetSegInfo[i].toIntersection = getInfoStreetSegment(i).to;
        info.StreetSegInfo[i].numCurvePoints = getInfoStreetSegment(i).curvePointCount;
        info.StreetSegInfo[i].id = getInfoStreetSegment(i).wayOSMID;
        info.StreetSegInfo[i].wayPtr = info.WayMap[info.StreetSegInfo[i].id];
        info.StreetSegInfo[i].type = getRoadType(info.StreetSegInfo[i].wayPtr);
    }
}

void populateData::populateIntersectionInfo(infoStrucs info){
    int numOfIntersections = getNumIntersections();
    info.IntersectionInfo.resize(numOfIntersections);
    
    for(int i=0;i<numOfIntersections;++i){
        info.IntersectionInfo[i].position = getIntersectionPosition(i);
        info.IntersectionInfo[i].name = getIntersectionName(i);
    }
}

void populateData::populateFeatureInfo(infoStrucs info, mapBoundary xy){
    int numFeatures = getNumFeatures();
    int numPoints;
    LatLon newPoint;
    double xNew, yNew;
    info.FeatureInfo.resize(numFeatures);
    info.FeaturePointVec.resize(numFeatures);
    
    for(int i=0;i<numFeatures;i++){
        info.FeatureInfo[i].name = getFeatureName(i);
        info.FeatureInfo[i].featureType = getFeatureType(i);
        
        numPoints = getFeaturePointCount(i);
        info.FeaturePointVec[i].clear();
        
        for(int p=0 ; p<numPoints; p++){
            newPoint = getFeaturePoint(p, i);
            
            xNew = xy.xFromLon(newPoint.lon());
            yNew = xy.yFromLat(newPoint.lat());
            
            info.FeaturePointVec[i].push_back(ezgl::point2d(xNew,yNew));
        }
        //std::cout << "n - " << info.FeatureInfo[i].name << " t - " << info.FeatureInfo[i].featureType << std::endl;
    }
}

void populateData::populatePOIInfo(infoStrucs info){
    int numPOI = getNumPointsOfInterest();
   
    info.POIInfo.resize(numPOI);
        
    for(int i=0 ; i<numPOI ; i++){
        
        
        info.POIInfo[i].name = getPointOfInterestName(i);
        info.POIInfo[i].type = getPointOfInterestType(i);
        
      //  std::cout << "n - " << POIInfo[i].name << " t - " << POIInfo[i].type << std::endl;
    }
}