/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m1.h"
#include "m2.h"
#include "globals.h"
#include "StreetsDatabaseAPI.h"
#include <map>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include <string>
#include <iostream>
void draw_map(){

}

//Somewhere in a big function(load_map like-thing) we should define & make global some try to make constant the adjustment factor (latmin+latmax)/2
std::pair<double, double> Lat_Lon_To_X_Y(LatLon point){
    //double pt1LatInRad = point1.lat()*DEG_TO_RAD;
    //double pt2LatInRad = point2.lat()*DEG_TO_RAD;
    //double averageLatInRad = (pt1LatInRad+pt2LatInRad)/2.0; to be defined in another function
    double projectionFactor = cos(averageLatInRad);
    double x=point.lon()*projectionFactor;
    double y=point.lat();
    return make_pair(x,y);
}
double Average_Lat(){
    //for this function I assume I already have all the intersections loaded
    //to do this I'd need to make a global variables.h thing
    //and since I don't want to do that without telling anyone and you are all
    //probably asleep I simply will not make one
    double maxLat=getIntersectionPosition(1).lat();
    double minLat=maxLat;;
    for(int i=0;i<){
    
    }
    return (maxLat+minLat)/2;
}