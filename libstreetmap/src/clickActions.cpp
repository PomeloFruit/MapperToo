#include "clickActions.h"

#include "m1.h"
#include "LatLon.h"
#include "globals.h"
#include "latLonToXY.h"
#include <string>

std::string clickActions::clickedOnPOI(double x, double y, mapBoundary &xy, infoStrucs &info){
    LatLon clickPos;
    unsigned clickedID, nearestIntID;
    std::string displayName = "Point of Interest Clicked: ";
    
    clickPos = xy.LatLonFromXY(x,y);
    clickedID = find_closest_point_of_interest(clickPos);
    displayName += info.POIInfo[clickedID].name;
    
    nearestIntID = find_closest_intersection(clickPos);
    displayName += " | Nearest Intersection: " + info.IntersectionInfo[nearestIntID].name;
    
    info.IntersectionInfo[info.lastIntersection].clicked = false;
    info.POIInfo[info.lastPOI].clicked = false;
    info.POIInfo[clickedID].clicked = true;
    info.lastPOI = clickedID;
    
    return displayName;
}


std::string clickActions::clickedOnIntersection(double x, double y, mapBoundary &xy, infoStrucs &info){
    LatLon clickPos;
    unsigned clickedID;
    std::string displayName = "Intersection Clicked: ";
    
    clickPos = xy.LatLonFromXY(x,y);
    clickedID = find_closest_intersection(clickPos);
    displayName += info.IntersectionInfo[clickedID].name;
    
    info.POIInfo[info.lastPOI].clicked = false;
    info.IntersectionInfo[info.lastIntersection].clicked = false;
    info.IntersectionInfo[clickedID].clicked = true;
    info.lastIntersection = clickedID;
    
    return displayName;
}