#include "fillStructs.h"

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "OSMID.h"

#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"

/* initialize function
 * - calls essential map-elements populate functions so data is in info structures
 * 
 * @param xy <mapBoundary> - object of type mapBoundary with x,y/Lat,Lon conversions
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return void
 */

void populateData::initialize(infoStrucs &info, mapBoundary &xy){
    populateOSMWayInfo(info);
    populateStreetSegInfo(info);
    populateIntersectionInfo(info);
    populateFeatureInfo(info, xy);
    populatePOIInfo(info);
    
    info.lastIntersection.clear();
    info.lastPOI.clear();
    info.lastSeg.clear();
    info.showRoute = 0;
}


/* loadAfterDraw function
 * - calls non-essential map-elements populate functions so data in info structure
 *   so user can add elements
 * 
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return void
 */

void populateData::loadAfterDraw(infoStrucs &info){
    populateOSMSubwayInfo(info);
}


/* populateOSMWayInfo function
 * - fills an ordered map of ways to be searched with by OSMID later
 * 
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return void
 */

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


/* populateOSMWayInfo function
 * - fills an ordered map of ways to be searched with by OSMID later
 * 
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return void
 */

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
        info.StreetSegInfo[i].clicked = false;
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
    std::vector <unsigned> routeVec;
    const OSMNode* currentPtr;

    info.SubwayInfo.clear();
    getOSMSubwayRelations(info);
    
    for(unsigned i=0 ; i< getNumberOfNodes(); i++){
        currentPtr = getNodeByIndex(i);
        routeVec = checkIfSubwayRouteNode(currentPtr, info);  
        
        isSubway = checkIfSubway(currentPtr);
        if(isSubway){
            subwayData newStop;
            newStop.name = getOSMNodeName(currentPtr);
            if(newStop.name.size()<=7 || newStop.name.compare(newStop.name.size()-7, 7, "Station") != 0){
                newStop.name.append(" Station");
            }
            newStop.nodePtr = currentPtr;
            newStop.clicked = false;
            newStop.point = currentPtr->coords();
            newStop.id = currentPtr->id();
            newStop.routeNum = routeVec;
            
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
            }
            else if (value == "residential"){
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

void populateData::getOSMSubwayRelations(infoStrucs &info){
    int subwayRouteType = 0; // 0 = no, 1 = subway , 2 = railway
    const OSMRelation* currentPtr;
    const OSMWay* wayPtr;

    info.SubwayInfo.clear();
    for(unsigned i=0 ; i< getNumberOfRelations(); i++){
        currentPtr = getRelationByIndex(i);
        subwayRouteType = checkIfSubwayRoute(currentPtr);
        
        if(subwayRouteType > 0){
            subwayRouteData newRoute;
            
            newRoute.name = getOSMRelationInfo(currentPtr, "name");
            newRoute.operatorName = getOSMRelationInfo(currentPtr, "operator");
            
            //get all node IDs within relationship
            for(unsigned j=0 ; j<currentPtr->members().size() ; j++){ //many node/ways within relationship

                if(currentPtr->members().at(j).tid.type() == 1){ // id is of node type
                    OSMID tempOSM = static_cast< OSMID >(currentPtr->members().at(j).tid);
                    std::vector< OSMID > tempVec;
                    tempVec.push_back(tempOSM);
                    newRoute.nodePoints.push_back(tempVec);
                    
                } else { //if not a node, then it must be a way
                    
                    wayPtr = info.WayMap[OSMID(currentPtr->members().at(j).tid)];
                    newRoute.nodePoints.push_back(wayPtr->ndrefs());                    
                }
            }
            
            newRoute.clicked = false;
            newRoute.type = subwayRouteType;
            
            newRoute.point.resize(newRoute.nodePoints.size());
            for(unsigned j=0 ; j<newRoute.nodePoints.size() ; j++){
                newRoute.point.at(j).resize(newRoute.nodePoints.at(j).size());
            }
            
            info.SubwayRouteInfo.push_back(newRoute);
        }
    }
}

int populateData::checkIfSubwayRoute(const OSMRelation* relPtr){
    if(relPtr == NULL){
        return 0;
    }

    if(checkOSMRelationTags(relPtr,"type","route")){
        if(checkOSMRelationTags(relPtr,"route","subway")){
            return 1;
        } else if (checkOSMRelationTags(relPtr,"route","railway")){
            return 2;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

std::vector< unsigned > populateData::checkIfSubwayRouteNode(const OSMNode* nPtr, infoStrucs &info){
    std::vector< unsigned > routesHit;
    for(unsigned i=0 ; i<info.SubwayRouteInfo.size() ; i++){
        
        subwayRouteData &srd = info.SubwayRouteInfo[i];
        
        for(unsigned n=0 ; n<srd.nodePoints.size() ; n++){
            for(unsigned m=0 ; m<srd.nodePoints.at(n).size() ; m++){
                if(nPtr->id() == srd.nodePoints[n][m]){
                    srd.point.at(n).at(m) = nPtr->coords();
                    routesHit.push_back(i);
                }
            }
        }
    }
    
    return routesHit;
}

bool populateData::checkOSMRelationTags(const OSMRelation* relPtr, std::string k, std::string v){
    if(getOSMRelationInfo(relPtr,k) == v){
        return true;
    }
    return false;
}

std::string populateData::getOSMRelationInfo(const OSMRelation* relPtr, std::string k){
    for(unsigned i=0 ; i < getTagCount(relPtr) ; i++){
        std::string key,value;
        std::tie(key,value) = getTagPair(relPtr,i);
        if(key == k){
            return value;
        }
    }
    return "";
}