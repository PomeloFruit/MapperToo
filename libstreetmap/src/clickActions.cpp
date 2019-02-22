// class clickActions definition file
#include "clickActions.h"

#include "ezgl/application.hpp"
#include "m1.h"
#include "m2.h"
#include "LatLon.h"
#include "globals.h"
#include "latLonToXY.h"
#include <string>


/* clickedOnIntersection function
 * - determines which intersection was clicked
 * - calls for said intersection to be highlighted
 * - returns message about it
 * 
 * @param x <double> - x coordinate of click in screen coordinates
 * @param y <double> - y coordinate of click in screen coordinates
 * @param xy <mapBoundary> - object of type mapBoundary with x,y/Lat,Lon conversions
 * @param info <infoStrucs> - object containing all essential map information
 * @return displayName <std::string > - message with information about intersection
 */

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

std::string clickActions::clickedOnSubway(double x, double y, mapBoundary &xy, infoStrucs &info){
    LatLon clickPos;
    unsigned clickedID = 0;
    std::string displayName = "Subway Station Clicked: ";
    
    clickPos = xy.LatLonFromXY(x,y);
    clickedID = findNearestSubway(info, clickPos);
    displayName += info.SubwayInfo[clickedID].name;
    
    for(unsigned i=0 ; i<info.SubwayInfo[clickedID].routeNum.size() ; i++){
        displayName += " | ";
        displayName += info.SubwayRouteInfo[info.SubwayInfo[clickedID].routeNum[i]].name;
        displayName += " Metro Line by ";
        displayName += info.SubwayRouteInfo[info.SubwayInfo[clickedID].routeNum[i]].operatorName;
    }
    
    highlightSubway(info, clickedID);
            
    return displayName;
}

std::string clickActions::searchOnMap(infoStrucs &info, ezgl::application *&application){
  //  const std::string STREETEXT = ".street.bin";
    std::string displayMessage;
    std::vector<unsigned> street1ID, street2ID, resultID;
    int match1, match2;
    unsigned correct1 = 0;
    unsigned correct2 = 0;
    
//    if(info.textInput1.compare(info.textInput1.size()-(STREETEXT.size()+1), STREETEXT.size(), STREETEXT)){
//        application->quit();
//        close_map();
//        std::cout << "here1\n";
//        load_map(info.textInput1);
//        std::cout << "here2\n";
//        application->quit();
//        //draw_map_helper(application);
//        displayMessage = "Successfully loaded map at " + info.textInput1;
//        return displayMessage;
//    }

    street1ID.clear();
    street2ID.clear();
    
    match1 = findMatches(street1ID, info.textInput1, info);
    match2 = findMatches(street2ID, info.textInput2, info);
    
    info.corInput1 = "";
    info.corInput2 = "";
    
    displayMessage = getMessagesFromMatches(match1, match2);
    
    if((match1 > 0) && (match2 > 0)){  // find intersections
        for(unsigned i=0 ; i<street1ID.size() ; i++){
            for(unsigned j=0 ; j<street2ID.size() ; j++){
                std::vector<unsigned> temp;
                temp = find_intersection_ids_from_street_ids(street1ID[i], street2ID[j]);
                for(unsigned k=0 ; k<temp.size(        ) ; k++){        
                    bool found = false;
                    for(unsigned l=0 ; l<resultID.size() ; l++){
                        if(resultID[l] == temp[k]){
                            found = true;
                        }
                    }
                    if(!found){
                        correct1 = i;
                        correct2 = j;
                        resultID.push_back(temp[k]);
                    }
                }
            }
        }
        
        info.corInput1=getStreetName(street1ID[correct1]);
        info.corInput2=getStreetName(street2ID[correct2]);
        
        highlightIntersection(info, resultID);
        displayMessage = "Intersection(s) Found: ";
        for(unsigned i=0 ; i<resultID.size()-1 ; i++){
            displayMessage += getIntersectionName(resultID[i]) + " | ";
        }        
        displayMessage += getIntersectionName(resultID[resultID.size()-1]);
        
    } else if(match1 == 1 || match1 > 5) { // find street from input 1
        
        resultID = street1ID;
        displayMessage = "Street Found: ";
        displayMessage += getStreetName(resultID[0]);
        info.corInput1 = getStreetName(resultID[0]);
        if(resultID.size()>1){
            displayMessage += " (1 of " + std::to_string(resultID.size()) + ")"; 
        }
        
        
    } else if(match2 == 1 || match2 > 5) { // find street from input 2
        
        resultID = street2ID;
        displayMessage = "Street Found: ";
        displayMessage += getStreetName(resultID[0]);
        info.corInput2 = getStreetName(resultID[0]);
        if(resultID.size()>1){
            displayMessage += " (1 of " + std::to_string(resultID.size()) + ")"; 
        }
        highlightStreet(info, resultID[0]);
        
    } else if(match1 == -2) { // find POI
        
        resultID = street1ID;
        displayMessage = "Point of Interest Found: ";
        displayMessage += info.POIInfo[resultID[0]].name;
        info.corInput1 = info.POIInfo[resultID[0]].name;
        if(resultID.size()>1){
            displayMessage += " (1 of " + std::to_string(resultID.size()) + ")"; 
        }
        highlightPOI(info, resultID);
        
    } else if(match1 == -3) { // find subways
        
        resultID = street1ID;
        displayMessage = "Subway Found: ";
        displayMessage += info.SubwayInfo[resultID[0]].name;
        info.corInput1 = info.SubwayInfo[resultID[0]].name;
        if(resultID.size()>1){
            displayMessage += " (" + std::to_string(resultID.size()) + ")"; 
        }
        highlightSubway(info, resultID);
        
        
        
    } else if(match1 == -4) { // find Features
        resultID = street1ID;
        displayMessage = "Feature Found: ";
        displayMessage += info.FeatureInfo[resultID[0]].name;
        info.corInput1 = info.FeatureInfo[resultID[0]].name;
        if(resultID.size()>1){
            displayMessage += " (" + std::to_string(resultID.size()) + ")"; 
        }
        highlightFeature(info, resultID[0]);
    }
    
    return displayMessage;
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

// -1 for no input, 0 for no match, >0 for number of matches, -2 for POI, -3 for Subway, -4 for feature 
int clickActions::findMatches(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info){
    int match = -1, numMatches = 0;
    std::transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);
    std::string station = "station";
    
    if(userInput != ""){
        streetID = find_street_ids_from_partial_street_name(userInput);        
        numMatches = streetID.size();
        if(userInput.size()>station.size() && userInput.compare(userInput.size()-(station.size()), station.size(), station)==0){
            findSubwaysByName(streetID, userInput, info);
            numMatches = streetID.size();
            if(numMatches > 0){
                match = -3;
            }                
        } else {
            if(numMatches == 0){
                match = 0;

                findPOIByName(streetID, userInput, info);
                numMatches = streetID.size();
                
                if(numMatches > 0){
                    match = -2;
                } else {
                    findFeaturesByName(streetID, userInput, info);
                    numMatches = streetID.size();
                    if(numMatches > 0){
                        match = -4;
                    }
                }
                
            } else if(numMatches == 1){
                match = 1;
            } else { //if(numMatches > 1)
                match = numMatches+5;
            }
        }
    }
    return match;
}

void clickActions::findPOIByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info){
    std::string temp;
    int sim;
    
    for(unsigned i=0 ; i<info.POIInfo.size() ; i++){
        temp = info.POIInfo[i].name;
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        sim = temp.compare(0, userInput.size(), userInput);
        if(sim == 0){
            streetID.push_back(i);
        }
    }
}

void clickActions::findSubwaysByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info){
    std::string temp;
    int sim = 0;

    for(unsigned i=0 ; i<info.SubwayInfo.size() ; i++){
        
        temp = info.SubwayInfo[i].name;
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        
        if(temp.size() >= userInput.size()){
            sim = temp.compare(0, userInput.size(), userInput);
        }
        
        if(sim == 0){
            streetID.push_back(i);
        }
    }
}

void clickActions::findFeaturesByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info){
    std::string temp;
    int sim;
    
    for(unsigned i=0 ; i<info.FeatureInfo.size() ; i++){
        temp = info.FeatureInfo[i].name;
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        sim = temp.compare(0, userInput.size(), userInput);
        if(sim == 0){
            streetID.push_back(i);
        }
    }
}

std::string clickActions::getMessagesFromMatches(int match1, int match2){
    std::string displayMessage = "";
    
    if(match1 == -1){ //no input in field 1
        displayMessage = "Please try again <input 1 - no names detected>!";
    } else if(match1 == 0 || match1 >= 5){ //no match, too many matches found for field 1
        if(match2 == 0 || match2 >= 5){
            displayMessage = "Please try again | ";
            displayMessage += std::to_string(match1) + " matches found for name 1 | ";
            displayMessage += std::to_string(match2) + " matches found for name 2";
        } else if((match2 == 1) || (match2 == -1)) {
            displayMessage = "Please try again | ";
            displayMessage += std::to_string(match1) + " matches found for name 1.";
        }
    } else if(match1 == 1){  //unique match for field 1
        if(match2 == 0 || match2 >= 5){ // no match or too many matches
            displayMessage = "Please try again | ";
            displayMessage += std::to_string(match2) + " matches found for name 2.";
        }
    }
    return displayMessage;
}

void clickActions::highlightStreet(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highIDinVec;
    highIDinVec.clear();
    highIDinVec.push_back(highID);
    highlightPOI(info, highIDinVec);
}

void clickActions::highlightStreet(infoStrucs &info, std::vector<unsigned> highID){
    std::vector<unsigned> highSegs;
    
    clearPreviousHighlights(info);
    
    for(unsigned i=0 ; i<highID.size() ; i++){
        std::vector<unsigned> temp;
        temp = find_street_street_segments(highID[i]);
        for(unsigned j=0 ; j<temp.size() ; j++){
            highSegs.push_back(temp[j]);
        }
    }
    
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

void clickActions::highlightSubway(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highIDinVec;
    highIDinVec.clear();
    highIDinVec.push_back(highID);
    highlightSubway(info, highIDinVec);
}

void clickActions::highlightSubway(infoStrucs &info, std::vector<unsigned> &highID){
    clearPreviousHighlights(info);
    
    for(unsigned i=0 ; i<highID.size() ; i++){
        info.SubwayInfo[highID[i]].clicked = true;
        
        for(unsigned j=0 ; j<info.SubwayInfo[highID[i]].routeNum.size() ; j++){
     //       std::cout << info.SubwayInfo[highID[i]].routeNum << std::endl;
            info.SubwayRouteInfo[info.SubwayInfo[highID[i]].routeNum[j]].clicked = true;
        }
    }
    info.lastSubway = highID;
}

void clickActions::highlightFeature(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highIDinVec;
    highIDinVec.clear();
    highIDinVec.push_back(highID);
    highlightFeature(info, highIDinVec);
}

void clickActions::highlightFeature(infoStrucs &info, std::vector<unsigned> &highID){
    clearPreviousHighlights(info);
    
    for(unsigned i=0 ; i<highID.size() ; i++){
        info.FeatureInfo[highID[i]].clicked = true;
    }
    info.lastFeature = highID;
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
    
    for(unsigned i=0 ; i<info.lastSubway.size() ; i++){
        currentIndex = info.lastSubway[i];
        info.SubwayInfo[currentIndex].clicked = false;
        
        for(unsigned j=0 ; j<info.SubwayInfo[currentIndex].routeNum.size() ; j++){
            info.SubwayRouteInfo[info.SubwayInfo[currentIndex].routeNum[j]].clicked = false;
        }
    }
    info.lastSubway.clear();
    
    for(unsigned i=0 ; i<info.lastFeature.size() ; i++){
        currentIndex = info.lastFeature[i];
        info.FeatureInfo[currentIndex].clicked = false;
    }
    info.lastFeature.clear();
}