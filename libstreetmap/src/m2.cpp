/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m1.h"
#include "m2.h"
//#include "globals.h"//I think this can be removed
#include "StreetsDatabaseAPI.h"
#include <map>
#include <unordered_map>

#include <math.h>
#include <algorithm>
#include <string>
#include <iostream>

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

struct intersectionData{
    LatLon position;
    std::string name;
};

//will likely need to add more things later
struct streetSegData{
    unsigned fromIntersection;
    unsigned toIntersection;
    int numCurvePoints;
    //as of FEB 18 5:27am name is not populated because I only wanted to draw roads and names weren't needed to do that
    std::string name;
};

double averageLatInRad;
double maxLat;
double maxLon;
double minLat;
double minLon;
std::vector<intersectionData> intersectionInfo;
std::vector<streetSegData> streetSegInfo;


void draw_main_canvas(ezgl::renderer &g);
void Average_Lat();
float y_from_lat(float lat);
float x_from_lon(float lon);
void populateStreetSegInfo();


void draw_map(){
    ezgl::application::settings settings;
    settings.main_ui_resource="libstreetmap/resources/main.ui";
    settings.window_identifier="MainWindow";
    settings.canvas_identifier="MainCanvas";
    ezgl::application application(settings);
    
    Average_Lat();//to make sure that we have the max stuff/min stuff so we can set bounds later
    populateStreetSegInfo();
    //ezgl::rectangle initial_world({0,0},{1000,1000});
    ezgl::rectangle initial_world({x_from_lon(minLon),y_from_lat(minLat)},{x_from_lon(maxLon),y_from_lat(maxLat)});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world);
    
    application.run(NULL,NULL,NULL,NULL);
    
}

void populateStreetSegInfo(){
    int numStreetSegments=getNumStreetSegments();
    streetSegInfo.resize(numStreetSegments);
    for(int i=0;i<numStreetSegments;i++){
        streetSegInfo[i].fromIntersection=getInfoStreetSegment(i).from;
        streetSegInfo[i].toIntersection=getInfoStreetSegment(i).to;
        streetSegInfo[i].numCurvePoints=getInfoStreetSegment(i).curvePointCount;
    }
}

//doesn't really need to exist but like hey maybe it'll be useful one day
std::pair<double, double> Lat_Lon_To_X_Y(LatLon point){ //call average lat before using this 
    double projectionFactor = cos(averageLatInRad);
    double x=point.lon()*projectionFactor;
    double y=point.lat();
    return std::make_pair(x,y);
}


float x_from_lon(float lon){
    float projectionFactor = cos(averageLatInRad);
    float x=lon*projectionFactor;
    return x;
}


float y_from_lat(float lat){
    return lat;
}

//to save processing time I am also going to make maxlat+maxlon+minlat+minlon here
//and I'm gonna make them global so pls don't kill me
//I am also going to populate intersectionInfo
//this should be really renamed
void Average_Lat(){
    maxLat=getIntersectionPosition(0).lat();
    minLat=maxLat;
    maxLon=getIntersectionPosition(0).lon();
    minLon=maxLon;
    int numOfIntersections=getNumIntersections();//you have to load map before you do any of this stuff
    intersectionInfo.resize(numOfIntersections);
    for(int i=0;i<numOfIntersections;++i){
        
        
        intersectionInfo[i].position=getIntersectionPosition(i);
        intersectionInfo[i].name=getIntersectionName(i);
        

        
        if(getIntersectionPosition(i).lat()>maxLat){
            maxLat=getIntersectionPosition(i).lat();
        }
        if(getIntersectionPosition(i).lat()<minLat){
            minLat=getIntersectionPosition(i).lat();
        }
        if(getIntersectionPosition(i).lon()>maxLon){
            maxLon=getIntersectionPosition(i).lon();
        }
        if(getIntersectionPosition(i).lon()<minLon){
            minLon=getIntersectionPosition(i).lon();
        }
    }
    averageLatInRad=DEG_TO_RAD*(maxLat+minLat)/2;//is't correct
}


void draw_main_canvas(ezgl::renderer &g){
    //g.draw_rectangle({0,0},{1000,1000});
    g.draw_rectangle({x_from_lon(minLon),y_from_lat(minLat)},{x_from_lon(maxLon),y_from_lat(maxLat)});//questionable?
    //std::cout<<minLon<<"!!!!!!"<<minLat<<"!!!!!!"<<maxLon<<"!!!!!!"<<maxLat<<'\n';
    
    //this draws the intersections
    int numOfIntersections=getNumIntersections();//you have to load map before you do any of this stuff
    for(int i=0;i<numOfIntersections;++i){
        float x=x_from_lon(intersectionInfo[i].position.lon());
        float y=y_from_lat(intersectionInfo[i].position.lat());
        //std::cout<<y<<"!!!!!!"<<x<<'\n';
        float width=0.0001;
        //float width=10;
        float height=width;
        g.fill_rectangle({x, y},{x+width, y+height});
    }
    
    //this draws roads as lines (doesn't do curvepoints yet)
    //setLineWidth(1);//this won't change unless we do that ourselves again later
    
    int numStreetSegments=getNumStreetSegments();
    for(int i=0;i<numStreetSegments;i++){
        if(streetSegInfo[i].numCurvePoints==0){
            float xInitial=x_from_lon(intersectionInfo[streetSegInfo[i].fromIntersection].position.lon());
            float yInitial=y_from_lat(intersectionInfo[streetSegInfo[i].fromIntersection].position.lat());

            float xFinal=x_from_lon(intersectionInfo[streetSegInfo[i].toIntersection].position.lon());
            float yFinal=y_from_lat(intersectionInfo[streetSegInfo[i].toIntersection].position.lat());

            g.draw_line({xInitial, yInitial},{xFinal, yFinal});
        }
        else{
            float xInitial=x_from_lon(intersectionInfo[streetSegInfo[i].fromIntersection].position.lon());
            float yInitial=y_from_lat(intersectionInfo[streetSegInfo[i].fromIntersection].position.lat());
            float xFinal;
            float yFinal;
            for(int c=0;c<streetSegInfo[i].numCurvePoints;c++){
                xFinal=x_from_lon(getStreetSegmentCurvePoint(c,i).lon());
                yFinal=y_from_lat(getStreetSegmentCurvePoint(c,i).lat());
                g.draw_line({xInitial, yInitial},{xFinal, yFinal});
                xInitial=xFinal;
                yInitial=yFinal;
            }
            xFinal=x_from_lon(intersectionInfo[streetSegInfo[i].toIntersection].position.lon());
            yFinal=y_from_lat(intersectionInfo[streetSegInfo[i].toIntersection].position.lat());

            g.draw_line({xInitial, yInitial},{xFinal, yFinal});
        }
    }
    
    //std::cout<<minLat<<"!!!!!!"<<minLon<<"!!!!!!"<<maxLat<<"!!!!!!"<<maxLon<<'\n';
}


