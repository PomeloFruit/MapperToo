#include "clickActions.h"

#include "ezgl/application.hpp"
#include "m1.h"
#include "LatLon.h"
#include "globals.h"
#include "latLonToXY.h"
#include <string>
#include <iostream>

std::string clickActions::clickedOnPOI(double x, double y, mapBoundary &xy, infoStrucs &info){
    LatLon clickPos;
    unsigned clickedID, nearestIntID;
    std::string displayName = "Point of Interest Clicked: ";
    
    clickPos = xy.LatLonFromXY(x,y);
    clickedID = find_closest_point_of_interest(clickPos);
    displayName += info.POIInfo[clickedID].name;
    
    nearestIntID = find_closest_intersection(clickPos);
    displayName += " | Nearest Intersection: " + info.IntersectionInfo[nearestIntID].name;
    
    highlightOnePOI(info, clickedID);
    
    return displayName;
}


std::string clickActions::clickedOnIntersection(double x, double y, mapBoundary &xy, infoStrucs &info){
    LatLon clickPos;
    unsigned clickedID;
    std::string displayName = "Intersection Clicked: ";
    
    clickPos = xy.LatLonFromXY(x,y);
    clickedID = find_closest_intersection(clickPos);
    displayName += info.IntersectionInfo[clickedID].name;
    
    highlightOneIntersection(info, clickedID);
    
    return displayName;
}

std::string clickActions::searchOnMap(infoStrucs &info){
    std::string displayMessage;
    std::vector<unsigned> street1ID, street2ID, resultID;
    int match1, match2;

    street1ID.clear();
    street2ID.clear();
    
    match1 = findMatches(info, street1ID, info.textInput1);
    match2 = findMatches(info, street2ID, info.textInput2);

    displayMessage = getMessagesFromMatches(match1, match2);
    
    if((match1 == 1) && (match2 == 1)){
        resultID = find_intersection_ids_from_street_ids(street1ID[0], street2ID[0]);
        highlightOneIntersection(info, resultID[0]);
        displayMessage = "Intersection(s) Found: ";
        displayMessage += getIntersectionName(resultID[0]);
    } else if(match1 == 1) {
        resultID = street1ID;
    } else if(match2 == 1) {
        resultID = street2ID;
    }
    
        
    return displayMessage;
}


    
    
void clickActions::highlightOnePOI(infoStrucs &info, unsigned highID){
    info.IntersectionInfo[info.lastIntersection].clicked = false;
    info.POIInfo[info.lastPOI].clicked = false;
    info.POIInfo[highID].clicked = true;
    info.lastPOI = highID;
}

void clickActions::highlightOneIntersection(infoStrucs &info, unsigned highID){
    info.POIInfo[info.lastPOI].clicked = false;
    info.IntersectionInfo[info.lastIntersection].clicked = false;
    info.IntersectionInfo[highID].clicked = true;
    info.lastIntersection = highID;
}

// -1 for no input, 0 for no match, 1 for match street, 2 for not unique enough
int clickActions::findMatches(infoStrucs &info, std::vector<unsigned> &streetID, std::string userInput){
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
            match = 2;
        }
    }
    return match;
}

std::string clickActions::getMessagesFromMatches(int match1, int match2){
    std::string displayMessage = "";
    
    switch(match1){
        case -1: //no input in field 1
            displayMessage = "Please try again <input 1 - no names detected>!";
            break;
            
        case 0: case 2: //no match, too many matches found for field 1
            if(match2 == 0 || match2 == 2){
                displayMessage = "Please try again <input 1 & 2 - no unique matches found>!";
            } else { //if(match2 == 1) or (match2 == -1) 
                displayMessage = "Please try again <input 1 - no unique matches found>!";
            }           
            break;
            
        case 1: //unique match for field 1
            if(match2 == 0 || match2 == 2){
                displayMessage = "Please try again <input 2 - no unique matches found>!";
            }           
            break;
        default:
            break;
    }
    return displayMessage;
}