#include "fillStructs.h"

#include "globals.h"
#include "latLonToXY.h"
#include "LatLon.h"
#include "grid.h"

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
    
    if(getNumberOfNodes() < MAXLOADNODES){
        populateOSMSubwayInfo(info);
    }
    
    info.findDirections = false;
    info.lastIntersection.clear();
    info.lastPOI.clear();
    info.lastSeg.clear();
    info.showRoute = 0;
}


void populateData::clear(infoStrucs &info){
    info.FeatureInfo.clear();
    info.FeaturePointVec.clear();
    info.IntersectionInfo.clear();
    info.POIInfo.clear();
    info.StreetSegInfo.clear(); 
    info.SubwayInfo.clear(); 
    info.SubwayRouteInfo.clear();
    info.WayMap.clear();
    info.lastFeature.clear();
    info.lastIntersection.clear();
    info.lastPOI.clear();
    info.lastSeg.clear();
    info.lastSubway.clear();
    info.streetType.clear();
    for(int i = 0; i < 6; i++){
        info.numStreetType[i] = 0;
    }
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


/* populateStreetSegInfo function
 * - fills the street segment vector in info 
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
        classifyStreetType(i, info);
        info.StreetSegInfo[i].name = getStreetName((getInfoStreetSegment(i).streetID));//gives the name of the street segment (for use in putting the names))
        info.StreetSegInfo[i].streetID=getInfoStreetSegment(i).streetID;
        info.StreetSegInfo[i].clicked = false;
    }
    streetTypeArray(info);
}


/* classifyStreetType function
 * - stores all unique streets and their street types 
 * 
 * @param i <int> - type of street
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return void
 */

void populateData::classifyStreetType(int i, infoStrucs &info){
    bool found = false;
    unsigned ID = getInfoStreetSegment(i).streetID;
    int type = getRoadType(info.StreetSegInfo[i].wayPtr);
    
    for(auto it = info.streetType.begin(); it != info.streetType.end(); it++){
        if(it->first == ID){
            found = true;
        }
    }
    if(!found){
        info.streetType.push_back(std::make_pair(ID, type));
    }
}


/* streetTypeArray function
 * - stores the number of street types 
 * 
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return void
 */

void populateData::streetTypeArray(infoStrucs &info){
    for(auto it = info.streetType.begin(); it != info.streetType.end(); it++){
        if(it->second == HIGHWAY){
            info.numStreetType[0]++;
        }else if (it->second == PRIMARY){
            info.numStreetType[1]++;
        }else if (it->second == SECONDARY){
            info.numStreetType[2]++;
        }else if (it->second == RESIDENTIAL){
            info.numStreetType[3]++;
        }else if (it->second == SERVICE){
            info.numStreetType[4]++;
        }else if (it->second == TRUNK){
            info.numStreetType[5]++;
        }
    }
}

/* populateIntersectionInfo function
 * - fills the intersection vector in info 
 * 
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return void
 */

void populateData::populateIntersectionInfo(infoStrucs &info){
    int numOfIntersections = getNumIntersections();
    info.IntersectionInfo.resize(numOfIntersections);
    
    for(int i=0;i<numOfIntersections;i++){
        info.IntersectionInfo[i].position = getIntersectionPosition(i);
        info.IntersectionInfo[i].name = getIntersectionName(i);
        info.IntersectionInfo[i].clicked = false;
    }
}


/* populateFeatureInfo function
 * - fills the feature vector in info 
 * - also fills feature point vec with converted xy coordinates
 * 
 * @param info <infoStrucs> - object containing all essential map information
 * @param xy <mapBoundary> - object of type mapBoundary with x,y/Lat,Lon conversions
 * 
 * @return void
 */

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
        
        // classify features by # of points

        if(info.FeatureInfo[i].featureType==3){
            info.FeatureInfo[i].priorityNum=1;
        }
        else if(info.FeatureInfo[i].isOpen) {
            info.FeatureInfo[i].priorityNum = 5;
        }else if(numPoints < 50){
            info.FeatureInfo[i].priorityNum = 4; 
        }else if (numPoints < 100){
            info.FeatureInfo[i].priorityNum = 3;
        } else {
            info.FeatureInfo[i].priorityNum = 2;
        }

        info.FeaturePointVec[i].clear();
        
        // convert and store feature points
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


/* populatePOIInfo function
 * - fills the POI info structure in info
 * 
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return void
 */

void populateData::populatePOIInfo(infoStrucs &info){
    int numPOI = getNumPointsOfInterest();
   
    info.POIInfo.resize(numPOI);
        
    for(int i=0 ; i<numPOI ; i++){
        info.POIInfo[i].name = getPointOfInterestName(i);
        info.POIInfo[i].type = getPointOfInterestType(i);
        info.POIInfo[i].clicked = false;
        info.POIInfo[i].poiNum = classifyPOI(info.POIInfo[i].type);
    }
}


/* populateOSMSubwayInfo function
 * - fills the subway info structure in info
 * 
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return void
 */

void populateData::populateOSMSubwayInfo(infoStrucs &info){
    bool isSubway = false;
    std::vector <unsigned> routeVec;
    const OSMNode* currentPtr;

    info.SubwayInfo.clear();
    
    // fill the subway routes structure
    getOSMSubwayRelations(info);
    
    // if too much information, we are not displaying routes
    if(getNumberOfNodes() > MAXNODES){
        info.SubwayRouteInfo.clear();
    }
    
    for(unsigned i=0 ; i< getNumberOfNodes() && i < MAXNODES ; i++){
                

        currentPtr = getNodeByIndex(i);
        routeVec = checkIfSubwayRouteNode(currentPtr, info);  

        isSubway = checkIfSubway(currentPtr);
        if(isSubway){

            subwayData newStop;
            
            // adds "station" to end of name if it does not have it
            newStop.name = getOSMNodeName(currentPtr);
            if(newStop.name.size()<=7 || newStop.name.compare(newStop.name.size()-7, 7, "Station") != 0){
                newStop.name.append(" Station");
            }
            
            newStop.nodePtr = currentPtr;
            newStop.clicked = false;
            newStop.point = currentPtr->coords();
            newStop.id = currentPtr->id();
            
            //associate subway station with route
            newStop.routeNum = routeVec;
            
            info.SubwayInfo.push_back(newStop);
        }            
    }    
}


/* checkIfSubway function
 * - determines if node matches the key value pair of a subway
 * 
 * @param nodePtr <OSMNode*> - contains OSM node information with lots of tags 
 * 
 * @return <bool> - true if node is a subway station, false else
 */

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


/* getOSMNodeName function
 * - gets the name of the node
 * 
 * @param nodePtr <OSMNode*> - contains OSM node information with lots of tags 
 * 
 * @return name<string> - name of osm node nodePtr
 */

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


/* getRoadType function
 * - goes through the tags of the way, looking for "highway"
 * - categorizes the type based on predifined constants
 * 
 * @param wayPtr <OSMWay*> - pointer to way with a lot of tags
 * 
 * @return <int> - road type based on predefined constants
 */

int populateData::getRoadType(const OSMWay* wayPtr){
    if(wayPtr == NULL){
        return SERVICE;
    }
    
    for(unsigned i=0 ; i < getTagCount(wayPtr) ; i++){
        std::string key,value;
        std::tie(key,value) = getTagPair(wayPtr,i);
        
        //compare only lower cases
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        
        if(key == "highway"){
            if(value == "motorway"){
                return HIGHWAY;
            } else if (value == "primary"){
                return PRIMARY;
            } else if (value == "secondary" || value == "tertiary"){
                return SECONDARY;
            } else if (value == "residential"){
                return RESIDENTIAL;
            } else if (value == "trunk" || value == "trunk_link"){
                return TRUNK; 
            } else if (value == "motorway_link") {
                return HIGHWAYRAMP;
            } else {
                return SERVICE;
            }
        }
    }
    return SECONDARY;
}


/* isFeatureOpen function
 * - features are closed is first and last point are the same
 * - determines if pt1(first) and pt2(last) are same
 * 
 * @param pt1 <LatLon> - first point of feature
 * @param pt2 <LatLon> - last point of feature
 * 
 * @return <bool> - true is feature is open, false if closed
 */

bool populateData::isFeatureOpen(LatLon pt1, LatLon pt2){
    if((pt1.lat() == pt2.lat()) && (pt1.lon() == pt2.lon())){
        return false;
    } else {
        return true;
    }
}


/* getOSMSubwayRelations function
 * - fills the subway route info in info
 * 
 * @param info <infoStrucs> - object containing all needed map element information
 * 
 * @return void
 */

void populateData::getOSMSubwayRelations(infoStrucs &info){
    int subwayRouteType = 0; // 0 = no, 1 = subway , 2 = railway
    const OSMRelation* currentPtr;
    const OSMWay* wayPtr;

    info.SubwayInfo.clear();
        
    //search through all relations looking for subways and trains
    for(unsigned i=0 ; i< getNumberOfRelations(); i++){
        currentPtr = getRelationByIndex(i);
        subwayRouteType = checkIfSubwayRoute(currentPtr);
        
        // if it is a train or subway
        if(subwayRouteType > 0){
            subwayRouteData newRoute;
            
            // get useful message information
            newRoute.name = getOSMRelationInfo(currentPtr, "name");
            newRoute.operatorName = getOSMRelationInfo(currentPtr, "operator");
                     
            //get all node IDs within relationship
            for(unsigned j=0 ; j<currentPtr->members().size() ; j++){ //many node/ways within relationship

                if(currentPtr->members().at(j).tid.type() == 1){ // id is of node type

                    OSMID tempOSM = static_cast< OSMID >(currentPtr->members().at(j).tid);
                    std::vector< OSMID > tempVec;
                    tempVec.push_back(tempOSM);
                    newRoute.nodePoints.push_back(tempVec);
                    
                } else if(currentPtr->members().at(j).tid.type() == 2) { // only want ways, no relations
                    
                    wayPtr = info.WayMap[OSMID(currentPtr->members().at(j).tid)];
                    newRoute.nodePoints.push_back(wayPtr->ndrefs());
                    
                }
            }
            
            newRoute.type = subwayRouteType;
            
            // dont highlight on intial draw
            newRoute.clicked = false;
            
            // prepare internal structures for filling
            newRoute.point.resize(newRoute.nodePoints.size());
            for(unsigned j=0 ; j<newRoute.nodePoints.size() ; j++){
                newRoute.point.at(j).resize(newRoute.nodePoints.at(j).size());
            }
            
            info.SubwayRouteInfo.push_back(newRoute);
        }
    }
}


/* checkIfSubwayRoute function
 * - looks through all relations looking for tags that id a train or subway
 * - returns 0 for no match, 1 for subway, 2 for train
 * 
 * @param relPtr <OSMRelation*> - pointer to relation with a lot of tags
 * 
 * @return <int> - type of route of relation relPtr
 */

int populateData::checkIfSubwayRoute(const OSMRelation* relPtr){
    if(relPtr == NULL){
        return 0;
    }

    if(checkOSMRelationTags(relPtr,"type","route")){
        
        if(checkOSMRelationTags(relPtr,"route","subway")){ //subway
            return 1;
        } else if (checkOSMRelationTags(relPtr,"route","railway")){ //train
            return 2;
        } else {
            return 0;
        }
        
    } else {
        return 0;
    }
}


/* checkIfSubwayRouteNode function
 * - goes through all subway route info nodes and looks for nPtr node
 * - adds all matches to routesHit and returns it
 * 
 * @param nPtr <OSMNode*> - pointer to node with a lot of tags
 * @param info <infoStrucs> - global variable storage with all map information
 * 
 * @return routesHit <std::vector< unsigned >> - all routes that contain nPtr node
 */

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


/* checkOSMRelationTags function
 * - goes through all tags of relation and determines if k,v pair exists
 * 
 * @param relPtr <OSMRelation*> - pointer to relation with a lot of tags
 * @param k <std::string> - key of tag
 * @param v <std::string> - value of tag
 * 
 * @return <bool> - true if relPtr relation contains tag with k key and v value
 */

bool populateData::checkOSMRelationTags(const OSMRelation* relPtr, std::string k, std::string v){
    if(getOSMRelationInfo(relPtr,k) == v){
        return true;
    }
    return false;
}


/* getOSMRelationInfo function
 * - goes through all tags of relation and returns value with key k
 * 
 * @param relPtr <OSMRelation*> - pointer to relation with a lot of tags
 * @param k <std::string> - key of tag
 * 
 * @return value<std::string> - value within key,value pair in tag with key k 
 */

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


/* classifyPOI function
 * - classifies POI based using the POI Type 
 * 
 * @param type <std::string> - POI Type from the Layer 1 API
 * 
 * @return poiType <int> - returns the POI Type number defined in Globals
 */

int populateData::classifyPOI(std::string type){
    int poiType = POIUNDEF; 
    
    if(std::find(tourist.begin(), tourist.end(), type) != tourist.end()){
        poiType = POITOURIST; 
    } else if(std::find(foodDrink.begin(), foodDrink.end(), type) != foodDrink.end()){
        poiType = POIFOOD;
    } else if(std::find(shops.begin(), shops.end(), type) != shops.end()){
        poiType = POISHOPS; 
    } else {
        poiType = POIUNDEF; 
    }
    
    return poiType; 
}