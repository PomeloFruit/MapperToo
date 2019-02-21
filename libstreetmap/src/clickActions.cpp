#include "clickActions.h"

#include "ezgl/application.hpp"
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
    displayName += " | Nearest Intersection: ";
    displayName += info.IntersectionInfo[nearestIntID].name;
    
    highlightPOI(info, clickedID);
    
    return displayName;
}


std::string clickActions::clickedOnIntersection(double x, double y, mapBoundary &xy, infoStrucs &info){
    LatLon clickPos;
    unsigned clickedID = 0;
    std::string displayName = "Intersection Clicked: ";
    
    clickPos = xy.LatLonFromXY(x,y);
    clickedID = find_closest_intersection(clickPos);
    displayName += info.IntersectionInfo[clickedID].name;
    
    highlightIntersection(info, clickedID);
    
    return displayName;
}

std::string clickActions::clickedOnSubway(double x, double y, mapBoundary &xy, infoStrucs &info){
    LatLon clickPos;
    unsigned clickedID = 0;
    std::string displayName = "Subway Station Clicked: ";
    
    clickPos = xy.LatLonFromXY(x,y);
    clickedID = findNearestSubway(info, clickPos);
    displayName += info.SubwayInfo[clickedID].name;
            
    return displayName;
}

unsigned clickActions::findNearestSubway(infoStrucs &info, LatLon pt){
    double min = EARTH_RADIUS_IN_METERS;
    unsigned nearestIndex = 0; 

    for(unsigned i = 0; i < info.SubwayInfo.size(); i++){
        double temp = find_distance_between_two_points(info.SubwayInfo[i].point, pt);
        if(temp <= min){
            min = temp;
            nearestIndex = i;
        }
    }
    return nearestIndex; 
}


std::string clickActions::searchOnMap(infoStrucs &info){
    std::string displayMessage;
    std::vector<unsigned> street1ID, street2ID, resultID;
    int match1, match2;

    street1ID.clear();
    street2ID.clear();
    
    match1 = findMatches(street1ID, info.textInput1);
    match2 = findMatches(street2ID, info.textInput2);

    displayMessage = getMessagesFromMatches(match1, match2);
    
    if((match1 == 1) && (match2 == 1)){
        resultID = find_intersection_ids_from_street_ids(street1ID[0], street2ID[0]);
        highlightIntersection(info, resultID);
        displayMessage = "Intersection(s) Found: ";
        displayMessage += getIntersectionName(resultID[0]);
    } else if(match1 == 1) {
        resultID = street1ID;
        displayMessage = "Street Found: ";
        displayMessage += getStreetName(resultID[0]);
        highlightStreet(info, resultID[0]);
    } else if(match2 == 1) {
        resultID = street2ID;
        displayMessage = "Street Found: ";
        displayMessage += getStreetName(street2ID[0]);
        highlightStreet(info, resultID[0]);
    }

    return displayMessage;
}



void clickActions::highlightStreet(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highSegs;
    
    clearPreviousHighlights(info);
    
    highSegs = find_street_street_segments(highID);

    for(unsigned i=0 ; i<highSegs.size() ; i++){
        info.StreetSegInfo[highSegs[i]].clicked = true;
    }
    info.lastSeg = highSegs;
}

void clickActions::highlightPOI(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highIDinVec;
    highIDinVec.clear();
    highIDinVec.push_back(highID);
    highlightPOI(info, highIDinVec);
}

void clickActions::highlightPOI(infoStrucs &info, std::vector<unsigned> &highID){
    clearPreviousHighlights(info);
    
    for(unsigned i=0 ; i<highID.size() ; i++){
        info.POIInfo[highID[i]].clicked = true;
    }
    info.lastPOI = highID;
}

void clickActions::highlightIntersection(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highIDinVec;
    highIDinVec.clear();
    highIDinVec.push_back(highID);
    highlightIntersection(info, highIDinVec);
}

void clickActions::highlightIntersection(infoStrucs &info, std::vector<unsigned> &highID){
    clearPreviousHighlights(info);
    
    for(unsigned i=0 ; i<highID.size() ; i++){
        info.IntersectionInfo[highID[i]].clicked = true;
    }
    info.lastIntersection = highID;
}

void clickActions::clearPreviousHighlights(infoStrucs &info){
    unsigned currentIndex;
    
    for(unsigned i=0 ; i<info.lastPOI.size() ; i++){
        currentIndex = info.lastPOI[i];
        info.POIInfo[currentIndex].clicked = false;
    }
    info.lastPOI.clear();
    
    for(unsigned i=0 ; i<info.lastIntersection.size() ; i++){
        currentIndex = info.lastIntersection[i];
        info.IntersectionInfo[currentIndex].clicked = false;
    }
    info.lastIntersection.clear();
    
    for(unsigned i=0 ; i<info.lastSeg.size() ; i++){
        currentIndex = info.lastSeg[i];
        info.StreetSegInfo[currentIndex].clicked = false;
    }
    info.lastSeg.clear();
}

// -1 for no input, 0 for no match, 1 for match street, 2 for not unique enough
int clickActions::findMatches(std::vector<unsigned> &streetID, std::string userInput){
    int match = -1, numMatches = 0;
    
    if(userInput != ""){
        streetID = find_street_ids_from_partial_street_name(userInput);
        
        numMatches = streetID.size();
        
        if(numMatches == 0){
            match = 0;
            
            //add stuff when implementing other search functions
            
        } else if(numMatches == 1){
            match = 1;
        } else { //if(numMatches > 1)
            match = numMatches+5;
        }
    }
    return match;
}

std::string clickActions::getMessagesFromMatches(int match1, int match2){
    std::string displayMessage = "";
    int numMatches1 = match1-5;
    int numMatches2 = match2-5;
    
    if(match1 == -1){ //no input in field 1
        displayMessage = "Please try again <input 1 - no names detected>!";
    } else if(match1 == 0 || match1 >= 5){ //no match, too many matches found for field 1
        if(match2 == 0 || match2 >= 5){
            displayMessage = "Please try again | ";
            displayMessage += std::to_string(numMatches1) + " matches found for name 1 | ";
            displayMessage += std::to_string(numMatches2) + " matches found for name 2";
        } else if((match2 == 1) || (match2 == -1)) {
            displayMessage = "Please try again | ";
            displayMessage += std::to_string(numMatches1) + " matches found for name 1.";
        }
    } else if(match1 == 1){  //unique match for field 1
        if(match2 == 0 || match2 >= 5){ // no match or too many matches
            displayMessage = "Please try again | ";
            displayMessage += std::to_string(numMatches2) + " matches found for name 2.";
        }
    }
    return displayMessage;
}