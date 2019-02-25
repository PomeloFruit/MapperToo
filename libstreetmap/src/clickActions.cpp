// class clickActions definition file
#include "clickActions.h"

#include "ezgl/application.hpp"
#include "m1.h"
#include "m2.h"
#include "LatLon.h"
#include "globals.h"
#include "latLonToXY.h"
#include <string>

const int RESULTNONE = 0; 
const int RESULTEMPTY = -1;
const int RESULTPOI = -2;
const int RESULTSUBWAY = -3;
const int RESULTFEATURE = -4;

/* clickedOnIntersection function
 * - determines which intersection was clicked
 * - calls for said intersection to be highlighted
 * - returns message about it
 * 
 * @param x <double> - x coordinate of click in screen coordinates
 * @param y <double> - y coordinate of click in screen coordinates
 * @param xy <mapBoundary> - object of type mapBoundary with x,y/Lat,Lon conversions
 * @param info <infoStrucs> - object containing all essential map information
 * 
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


/* clickedOnPOI function
 * - determines which POI was clicked
 * - calls for said POI to be highlighted
 * - returns message about it
 * 
 * @param x <double> - x coordinate of click in screen coordinates
 * @param y <double> - y coordinate of click in screen coordinates
 * @param xy <mapBoundary> - object of type mapBoundary with x,y/Lat,Lon conversions
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return displayName <std::string > - message with information about POI and nearest interseciton
 */

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


/* clickedOnSubway function
 * - determines which subway was clicked
 * - calls for said subway to be highlighted
 * - if available, provides its route information as well 
 * - returns message about it (station and route)
 * 
 * @param x <double> - x coordinate of click in screen coordinates
 * @param y <double> - y coordinate of click in screen coordinates
 * @param xy <mapBoundary> - object of type mapBoundary with x,y/Lat,Lon conversions
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return displayName <std::string > - message with information about POI and nearest interseciton
 */

std::string clickActions::clickedOnSubway(double x, double y, mapBoundary &xy, infoStrucs &info){
    LatLon clickPos;
    unsigned clickedID = 0;
    std::string displayName = "Subway Station Clicked: ";
    
    clickPos = xy.LatLonFromXY(x,y);
    clickedID = findNearestSubway(info, clickPos);
    displayName += info.SubwayInfo[clickedID].name;
    
    for(unsigned i=0 ; i<info.SubwayInfo[clickedID].routeNum.size() ; i++){
        std::string tempName = info.SubwayRouteInfo[info.SubwayInfo[clickedID].routeNum[i]].name;
        std::string tempOperator = info.SubwayRouteInfo[info.SubwayInfo[clickedID].routeNum[i]].operatorName;
        
        if(tempName == ""){
            tempName = "<route name unknown>";
        }
        if(tempOperator == ""){
            tempOperator = "<operator unknown>";
        }

        displayName += " | Route Name: " + tempName + " | Operator: " + tempOperator;
    }
    
    highlightSubway(info, clickedID);
            
    return displayName;
}


/* searchOnMap function
 * - searches for various things on the map
 * - returns information on:
 * 1. intersections - input field 1 and 2
 * 2. streets - input field 1 or 2
 * 3. POI - input field 1
 * 4. Subways (stations) - input field 1
 * 5. Features - input field 1
 * -- if input not unique, it will take first in returned list
 * 
 * @param info <infoStrucs> - object containing all essential map information
 * 
 * @return displayMessage <std::string > - message with information about POI and nearest intersection
 */

std::string clickActions::searchOnMap(infoStrucs &info){
    std::string displayMessage;
    std::vector<unsigned> street1ID, street2ID, resultID;
    int match1, match2;
    unsigned correct1 = 0;
    unsigned correct2 = 0;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    const std::string STREETEXT = ".street.bin";
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
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    // get the match type of the search
    match1 = findMatches(street1ID, info.textInput1, info);
    match2 = findMatches(street2ID, info.textInput2, info);
    
    // reset correct input output names to blank
    info.corInput1 = "";
    info.corInput2 = "";
    
    // if inputs are invalid, get the error message
    displayMessage = getMessagesFromMatches(match1, match2);
      
    // find intersections, both inputs have at least one matching name
    if((match1 > RESULTNONE) && (match2 > RESULTNONE)){
        
        for(unsigned i=0 ; i<street1ID.size() ; i++){
            for(unsigned j=0 ; j<street2ID.size() ; j++){
                
                std::vector<unsigned> temp;
                temp = find_intersection_ids_from_street_ids(street1ID[i], street2ID[j]);
               
                for(unsigned k=0 ; k<temp.size() ; k++){
                    
                    bool found = false;
                    
                    // look for duplicates
                    for(unsigned l=0 ; l<resultID.size() ; l++){
                        if(resultID[l] == temp[k]){
                            found = true;
                        }
                    }
                    
                    // save the index of the intersection so we can fill in the full street name later
                    if(!found){
                        correct1 = i;
                        correct2 = j;
                        resultID.push_back(temp[k]);
                    }
                    
                }
                
            }
        }
        
        if(resultID.size() > 0){ // if intersection found
        
            // add full name of streets into search field
            info.corInput1=getStreetName(street1ID[correct1]);
            info.corInput2=getStreetName(street2ID[correct2]);

            // call for the intersection to be highlighted
            highlightIntersection(info, resultID[0]);

            // create a message about the intersection found
            displayMessage = "Intersection(s) Found: ";
            displayMessage += getIntersectionName(resultID[0]);
            
            std::cout << "Intersection search result(s):" << std::endl;
            
            for(unsigned i=0 ; i<resultID.size()-1 ; i++){
                std::cout << getIntersectionName(resultID[i]) << std::endl;
            }
            
            displayMessage += getIntersectionName(resultID[resultID.size()-1]);
            
        } else { // no intersection found
            displayMessage = "No intersections found.";
            std::cout << displayMessage << std::endl;
        }
    
    // find street from input 1
    } else if(match1 > RESULTNONE) { 
        
        resultID = street1ID;
        displayMessage = "Street Found: ";
        displayMessage += getStreetName(resultID[0]);
        info.corInput1 = getStreetName(resultID[0]);
        
        // if multiple results, tell user which option they are seeing
        if(resultID.size()>1){
            displayMessage += " (Displaying 1 of " + std::to_string(resultID.size()) + " found)"; 
        }
        
        highlightStreet(info, resultID[0]);

    // find street from input 2
    } else if(match2 > RESULTNONE) { 
        
        resultID = street2ID;
        displayMessage = "Street Found: ";
        displayMessage += getStreetName(resultID[0]);
        info.corInput2 = getStreetName(resultID[0]);
        
        if(resultID.size()>1){
            displayMessage += " (Displaying 1 of " + std::to_string(resultID.size()) + " found)"; 
        }
        
        highlightStreet(info, resultID[0]);
    
    // find POI from input 1
    } else if(match1 == RESULTPOI) { 
        
        resultID = street1ID;
        displayMessage = "Point of Interest Found: ";
        displayMessage += info.POIInfo[resultID[0]].name;
        info.corInput1 = info.POIInfo[resultID[0]].name;
        
        if(resultID.size()>1){
            displayMessage += " (Displaying 1 of " + std::to_string(resultID.size()) + " found)"; 
        }
        
        highlightPOI(info, resultID);
    
    // find subway station from input 1
    } else if(match1 == RESULTSUBWAY) { 
        
        resultID = street1ID;
        displayMessage = "Subway Found: ";
        displayMessage += info.SubwayInfo[resultID[0]].name;
        info.corInput1 = info.SubwayInfo[resultID[0]].name;
        
        if(resultID.size()>1){
            displayMessage += " (Displaying 1 of " + std::to_string(resultID.size()) + " found)"; 
        }
        
        highlightSubway(info, resultID);
        
        
    // find Features from input 1   
    } else if(match1 == RESULTFEATURE) { 
        
        resultID = street1ID;
        
        displayMessage = "Feature Found: ";
        displayMessage += info.FeatureInfo[resultID[0]].name;
        info.corInput1 = info.FeatureInfo[resultID[0]].name;
        
        if(resultID.size()>1){
            displayMessage += " (Displaying 1 of " + std::to_string(resultID.size()) + " found)"; 
        }
        
        highlightFeature(info, resultID[0]);
    }
    
    return displayMessage;
}


/* findNearestSubway function
 * - returns the index of the closest subway station to pt
 * 
 * @param info <infoStrucs> - object containing all essential map information
 * @param pt <LatLon> - point to find subway closest to
 * 
 * @return nearestIndex <unsigned> - index of closest subway station
 */

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


/* findMatches function
 * fills streetID with in the id of the respective map element
 * - returns the type of match found for the input
 * - -1 for no input
 * -  0 for no match
 * -  >0 for number of matches
 * -  -2 for POI
 * -  -3 for Subway
 * -  -4 for feature 

 * 
 * @param streetID <std::vector<unsigned>> - vector by reference containing results
 * @param userInput <std::string> - the input from the user in the text field
 * @param info <infoStrucs> - contains all map information needed
 * 
 * @return match <int> - type of match found
 */

int clickActions::findMatches(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info){
    int match = RESULTEMPTY;
    int numMatches = 0;
    std::string station = "station";
    
    std::transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);

    if(userInput != ""){
        streetID = find_street_ids_from_partial_street_name(userInput);        
        numMatches = streetID.size();
        
        // check if input ends with "station"
        if(userInput.size()>station.size() && userInput.compare(userInput.size()-(station.size()), station.size(), station)==0){
            
            // check if input matches any subway stations
            findSubwaysByName(streetID, userInput, info);
            numMatches = streetID.size();
            if(numMatches > 0){
                match = RESULTSUBWAY;
            }
            
        } else {
            
            if(numMatches == 0){
                match = RESULTNONE;
                
                //check if input matches any POI
                findPOIByName(streetID, userInput, info);
                numMatches = streetID.size();
                
                if(numMatches > 0){
                    match = RESULTPOI;
                } else {
                    // check if input matches any features
                    findFeaturesByName(streetID, userInput, info);
                    numMatches = streetID.size();
                    if(numMatches > 0){
                        match = RESULTFEATURE;
                    }
                }
                
            } else { // matches > 0
                match = numMatches;
            }
            
        }
    }
    
    return match;
}


/* findPOIByName function
 * - finds the POI matching the userInput name and puts their index into streetID
 * 
 * @param streetID <std::vector<unsigned>> - vector by reference containing results
 * @param userInput <std::string> - the input from the user in the text field
 * @param info <infoStrucs> - contains all map information needed
 * 
 * @return void
 */

void clickActions::findPOIByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info){
    std::string temp;
    int sim;
    
    for(unsigned i=0 ; i<info.POIInfo.size() ; i++){
        temp = info.POIInfo[i].name;
        
        // make sure comparing lower case only
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        sim = temp.compare(0, userInput.size(), userInput);
        if(sim == 0){
            streetID.push_back(i);
        }
    }
}


/* findSubwaysByName function
 * - finds the subways matching the userInput name and puts their index into streetID
 * 
 * @param streetID <std::vector<unsigned>> - vector by reference containing results
 * @param userInput <std::string> - the input from the user in the text field
 * @param info <infoStrucs> - contains all map information needed
 * 
 * @return void
 */

void clickActions::findSubwaysByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info){
    std::string temp;
    int sim = 0;

    for(unsigned i=0 ; i<info.SubwayInfo.size() ; i++){
        
        temp = info.SubwayInfo[i].name;
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        
        // in case a name is not found for a subway
        if(temp.size() >= userInput.size()){
            sim = temp.compare(0, userInput.size(), userInput);
        }
        
        if(sim == 0){
            streetID.push_back(i);
        }
    }
}


/* findFeaturesByName function
 * - finds the features matching the userInput name and puts their index into streetID
 * 
 * @param streetID <std::vector<unsigned>> - vector by reference containing results
 * @param userInput <std::string> - the input from the user in the text field
 * @param info <infoStrucs> - contains all map information needed
 * 
 * @return void
 */

void clickActions::findFeaturesByName(std::vector<unsigned> &streetID, std::string userInput, infoStrucs &info){
    std::string temp;
    int sim = 0;
    
    for(unsigned i=0 ; i<info.FeatureInfo.size() ; i++){
        
        // in case a feature does not have a name
        temp = info.FeatureInfo[i].name;
        
        // compare lower case only
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        sim = temp.compare(0, userInput.size(), userInput);
        if(sim == 0){
            streetID.push_back(i);
        }
    }
}


/* getMessagesFromMatches function
 * - gets error messages using the information from match1 and match2
 * - tells user what was wrong with input
 * 
 * @param match1 <int> - match type of input 1
 * @param match2 <int> - match type of input 2
 * 
 * @return displayMessage <string> - error message (if needed) with information about error
 */

std::string clickActions::getMessagesFromMatches(int match1, int match2){
    std::string displayMessage = "";
    
    if(match1 == RESULTEMPTY){ //no input in field 1
        
        displayMessage = "Please try again <input 1 - no names detected>!";
        
    } else if(match1 == RESULTNONE){ //no match found for field 1
        
        if(match2 == RESULTNONE || match2 >= 1){ //no match found or too many matches for field 2
            
            displayMessage = "Please try again | ";
            displayMessage += std::to_string(match1) + " matches found for name 1 | ";
            displayMessage += std::to_string(match2) + " matches found for name 2";
            
        } else if((match2 == 1) || (match2 == RESULTEMPTY)) { // unique in field 2, but field 1 is no good
            
            displayMessage = "Please try again | ";
            displayMessage += std::to_string(match1) + " matches found for name 1.";
            
        }
        
    } else if(match1 == 1){  //unique match for field 1
        
        if(match2 == RESULTNONE || match2 >= 1){ // no match or too many matches
            
            displayMessage = "Please try again | ";
            displayMessage += std::to_string(match2) + " matches found for name 2.";
            
        }
    }
    
    return displayMessage;
}


/* highlightStreet function
 * - converts into vector and calls below function
 * - raises flag on all segments on highID street to highlight
 * - saves the highlighted index in info
 *
 * @param info <infoStrucs> - contains all map information needed
 * @param highID <unsigned> - index for street to highlight
 * 
 * @return void
 */

void clickActions::highlightStreet(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highIDinVec;
    highIDinVec.clear();
    highIDinVec.push_back(highID);
    highlightStreet(info, highIDinVec);
}


/* highlightStreet function
 * - raises flag on all segments on all highID streets to highlight
 * - saves the highlighted indices in info
 *
 * @param info <infoStrucs> - contains all map information needed
 * @param highID <std::vector<unsigned>> - indices for streets to highlight
 * 
 * @return void
 */

void clickActions::highlightStreet(infoStrucs &info, std::vector<unsigned> highID){
    std::vector<unsigned> highSegs;
    
    clearPreviousHighlights(info);
    
    //get all segs to change
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


/* highlightPOI function
 * - converts into vector and calls below function
 * - raises flag on highID POI to highlight
 * - saves the highlighted index in info
 *
 * @param info <infoStrucs> - contains all map information needed
 * @param highID <unsigned> - index for POI to highlight
 * 
 * @return void
 */

void clickActions::highlightPOI(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highIDinVec;
    highIDinVec.clear();
    highIDinVec.push_back(highID);
    highlightPOI(info, highIDinVec);
}


/* highlightPOI function
 * - raises flag on all highID POI to highlight
 * - saves the highlighted indices in info
 *
 * @param info <infoStrucs> - contains all map information needed
 * @param highID <std::vector<unsigned>> - indices for poi to highlight
 * 
 * @return void
 */

void clickActions::highlightPOI(infoStrucs &info, std::vector<unsigned> &highID){
    clearPreviousHighlights(info);
    
    for(unsigned i=0 ; i<highID.size() ; i++){
        info.POIInfo[highID[i]].clicked = true;
    }
    info.lastPOI = highID;
}


/* highlightIntersection function
 * - converts into vector and calls below function
 * - raises flag on highID intersection to highlight
 * - saves the highlighted index in info
 *
 * @param info <infoStrucs> - contains all map information needed
 * @param highID <unsigned> - index for intersection to highlight
 * 
 * @return void
 */

void clickActions::highlightIntersection(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highIDinVec;
    highIDinVec.clear();
    highIDinVec.push_back(highID);
    highlightIntersection(info, highIDinVec);
}


/* highlightIntersection function
 * - raises flag on all highID intersection to highlight
 * - saves the highlighted indices in info
 *
 * @param info <infoStrucs> - contains all map information needed
 * @param highID <std::vector<unsigned>> - indices for intersection to highlight
 * 
 * @return void
 */

void clickActions::highlightIntersection(infoStrucs &info, std::vector<unsigned> &highID){
    clearPreviousHighlights(info);
    
    for(unsigned i=0 ; i<highID.size() ; i++){
        info.IntersectionInfo[highID[i]].clicked = true;
    }
    info.lastIntersection = highID;
}


/* highlightSubway function
 * - converts into vector and calls below function
 * - raises flag on highID subway to highlight
 * - saves the highlighted index in info
 *
 * @param info <infoStrucs> - contains all map information needed
 * @param highID <unsigned> - index for subway to highlight
 * 
 * @return void
 */

void clickActions::highlightSubway(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highIDinVec;
    highIDinVec.clear();
    highIDinVec.push_back(highID);
    highlightSubway(info, highIDinVec);
}


/* highlightSubway function
 * - raises flag on all highID subway to highlight
 * - saves the highlighted indices in info
 *
 * @param info <infoStrucs> - contains all map information needed
 * @param highID <std::vector<unsigned>> - indices for subway to highlight
 * 
 * @return void
 */

void clickActions::highlightSubway(infoStrucs &info, std::vector<unsigned> &highID){
    clearPreviousHighlights(info);
    
    for(unsigned i=0 ; i<highID.size() ; i++){
        info.SubwayInfo[highID[i]].clicked = true;
        
        for(unsigned j=0 ; j<info.SubwayInfo[highID[i]].routeNum.size() ; j++){
            info.SubwayRouteInfo[info.SubwayInfo[highID[i]].routeNum[j]].clicked = true;
        }
    }
    info.lastSubway = highID;
}


/* highlightPOI function
 * - converts into vector and calls below function
 * - raises flag on highID feature to highlight
 * - saves the highlighted index in info
 *
 * @param info <infoStrucs> - contains all map information needed
 * @param highID <unsigned> - index for feature to highlight
 * 
 * @return void
 */

void clickActions::highlightFeature(infoStrucs &info, unsigned highID){
    std::vector<unsigned> highIDinVec;
    highIDinVec.clear();
    highIDinVec.push_back(highID);
    highlightFeature(info, highIDinVec);
}


/* highlightFeature function
 * - raises flag on all highID features to highlight
 * - saves the highlighted indices in info
 *
 * @param info <infoStrucs> - contains all map information needed
 * @param highID <std::vector<unsigned>> - indices for features to highlight
 * 
 * @return void
 */

void clickActions::highlightFeature(infoStrucs &info, std::vector<unsigned> &highID){
    clearPreviousHighlights(info);
    
    for(unsigned i=0 ; i<highID.size() ; i++){
        info.FeatureInfo[highID[i]].clicked = true;
    }
    info.lastFeature = highID;
}


/* clearPreviousHighlights function
 * - clears flag on all highlight-able features, and empties "last" storage
 * - goes through each "last" storage, and clears all flags in indices of respective feature
 *
 * @param info <infoStrucs> - contains all map information needed
 * 
 * @return void
 */

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