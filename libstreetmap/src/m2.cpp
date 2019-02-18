/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m1.h"
#include "m2.h"
#include "drawRoads.h"
#include "drawFeatures.h"
#include "latLonToXY.h"
//#include "globals.h" //I think this can be removed
#include "StreetsDatabaseAPI.h"
#include <math.h>
#include <algorithm>
#include <string>
#include <iostream>

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"


///////////================================================================================

double averageLatInRad;
double maxLat, maxLon, minLat, minLon;

// finds the average latitude within the map bounds and stores in averageLatInRad
void averageLat();

//returns the y coordinate corresponding the latitude
float yFromLat(float lat);

//returns the x coordinate corresponding the longitude
float xFromLon(float lon);

///////////================================================================================

void averageLat(){
    
    maxLat = getIntersectionPosition(0).lat();
    minLat = maxLat;
    maxLon = getIntersectionPosition(0).lon();
    minLon = maxLon;
    
    int numOfIntersections = getNumIntersections();
    
    //find min and max lat/lon points
    for(int i=0;i<numOfIntersections;i++){
        if(getIntersectionPosition(i).lat()>maxLat){
            maxLat = getIntersectionPosition(i).lat();
        }
        if(getIntersectionPosition(i).lat()<minLat){
            minLat = getIntersectionPosition(i).lat();
        }
        if(getIntersectionPosition(i).lon()>maxLon){
            maxLon = getIntersectionPosition(i).lon();
        }
        if(getIntersectionPosition(i).lon()<minLon){
            minLon = getIntersectionPosition(i).lon();
        }
    }
    
    averageLatInRad = DEG_TO_RAD*(maxLat+minLat)/2;
}


float xFromLon(float lon){
    float projectionFactor,x;
    
    projectionFactor = cos(averageLatInRad);
    x = lon*projectionFactor;
    
    return x;
}


float yFromLat(float lat){
    return lat;
}

///////////================================================================================


struct intersectionData{
    LatLon position;
    std::string name;
};

//will likely need to add more things later
struct streetSegData{
    unsigned fromIntersection;
    unsigned toIntersection;
    int numCurvePoints;
    std::string name;
};

struct featureData{
    int featureType;
    int numPoints;
    std::string name;
};

std::vector<intersectionData> IntersectionInfo;
std::vector<streetSegData> StreetSegInfo;
std::vector<featureData> FeatureInfo;

void populateStreetSegInfo();
void populateIntersectionInfo();
void populateFeatureInfo();

///////////================================================================================

void populateStreetSegInfo(){
    int numStreetSegments = getNumStreetSegments();
    StreetSegInfo.resize(numStreetSegments);
    
    for(int i=0;i<numStreetSegments;i++){
        StreetSegInfo[i].fromIntersection = getInfoStreetSegment(i).from;
        StreetSegInfo[i].toIntersection = getInfoStreetSegment(i).to;
        StreetSegInfo[i].numCurvePoints = getInfoStreetSegment(i).curvePointCount;
    }
}

void populateIntersectionInfo(){
    int numOfIntersections = getNumIntersections();
    IntersectionInfo.resize(numOfIntersections);
    
    for(int i=0;i<numOfIntersections;++i){
        IntersectionInfo[i].position = getIntersectionPosition(i);
        IntersectionInfo[i].name = getIntersectionName(i);
    }
}

void populateFeatureInfo(){
    int numFeatures = getNumFeatures();
    FeatureInfo.resize(numFeatures);
    
    for(int i=0;i<numFeatures;i++){
        FeatureInfo[i].name = getFeatureName(i);
        FeatureInfo[i].featureType = getFeatureType(i);
        FeatureInfo[i].numPoints = getFeaturePointCount(i);
    }
}

///////////================================================================================

void setFeatureColour(int type, ezgl::renderer &g);
void drawFeatures(int numFeatures, ezgl::renderer &g);
void drawStreetRoads(int numSegs, ezgl::renderer &g);
void drawStraightStreet(LatLon pt1, LatLon pt2, ezgl::renderer &g);
void drawIntersections(int numInter, ezgl::renderer &g);

/////////////================================================================================
//void setFeatureColour(int type, ezgl::renderer &g){
//    
//    g.set_color(100,100,100,255);
//}
        


void drawFeatures(int numFeatures, ezgl::renderer &g){
    int numPoints;
    LatLon newPoint;
    double xNew, yNew;
    std::vector<ezgl::point2d> points;
    
    for(int i=0 ; i<numFeatures ; i++){
        numPoints = FeatureInfo[i].numPoints;
        std::cout << "feature # " << i << " num points: " << numPoints << " type: " << FeatureInfo[i].featureType << std::endl;
        points.clear();
        for(int p=0 ; p<numPoints; p++){
            newPoint = getFeaturePoint(p, i);
            
            xNew = xFromLon(newPoint.lon());
            yNew = yFromLat(newPoint.lat());
            
            points.push_back(ezgl::point2d(xNew,yNew));
        }
        
       // setFeatureColour(int type, ezgl::renderer &g);
        g.set_color(100,100,100,255);
        
        if(points.size()>1){
            g.fill_poly(points);
        } else {
            g.fill_rectangle(points[0],0.0001,0.0001);
        }
    }
}

void drawStreetRoads(int numSegs, ezgl::renderer &g){
    LatLon from, to;
    
    for(int i=0 ; i<numSegs ; i++){
        
        from = IntersectionInfo[StreetSegInfo[i].fromIntersection].position;

        for(int c=0;c<StreetSegInfo[i].numCurvePoints;c++){
                
            to = getStreetSegmentCurvePoint(c,i);
            drawStraightStreet(from, to, g);
            from = to;
        }
        to = IntersectionInfo[StreetSegInfo[i].toIntersection].position;
        drawStraightStreet(from, to, g);
    }
}

void drawStraightStreet(LatLon pt1, LatLon pt2, ezgl::renderer &g){
    float xInitial, yInitial, xFinal, yFinal;
    
    xInitial=xFromLon(pt1.lon());
    yInitial=yFromLat(pt1.lat());
    xFinal=xFromLon(pt2.lon());
    yFinal=yFromLat(pt2.lat());
    
    double width = 0.0001;
    
//    std::vector<ezgl::point2d> coords;
//    coords.push_back(ezgl::point2d(xInitial-width,yInitial-width));
//    coords.push_back(ezgl::point2d(xInitial+width,yInitial+width));
//    coords.push_back(ezgl::point2d(xFinal+width,yFinal+width));
//    coords.push_back(ezgl::point2d(xFinal-width,yFinal-width));
    g.set_line_width(3);
    g.set_color(0,0,0,255);
    
    
    g.draw_line({xInitial, yInitial},{xFinal, yFinal});
    
    //g.fill_poly(coords);
}

void drawIntersections(int numInter, ezgl::renderer &g){
     for(int i=0 ; i<numInter ; i++){
        float x = xFromLon(IntersectionInfo[i].position.lon());
        float y = yFromLat(IntersectionInfo[i].position.lat());
        
        float width=0.0001; ///no magic numbers!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //float width=10;
        float height=width;
        g.fill_rectangle({x, y},{x+width, y+height});
    }
}

///////////================================================================================

void draw_main_canvas(ezgl::renderer &g);

void draw_map(){
    ezgl::application::settings settings;
    settings.main_ui_resource="libstreetmap/resources/main.ui";
    settings.window_identifier="MainWindow";
    settings.canvas_identifier="MainCanvas";
    ezgl::application application(settings);
    
    averageLat();//to make sure that we have the max stuff/min stuff so we can set bounds later
    populateStreetSegInfo();
    populateIntersectionInfo();
    populateFeatureInfo();
    
    ezgl::rectangle initial_world({xFromLon(minLon),yFromLat(minLat)},{xFromLon(maxLon),yFromLat(maxLat)});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world);
    
    application.run(NULL,NULL,NULL,NULL);
    
}


//
////doesn't really need to exist but like hey maybe it'll be useful one day
//std::pair<double, double> Lat_Lon_To_X_Y(LatLon point){ //call average lat before using this 
//    double projectionFactor = cos(averageLatInRad);
//    double x = point.lon()*projectionFactor;
//    double y = point.lat();
//    return std::make_pair(x,y);
//}


void draw_main_canvas(ezgl::renderer &g){
    g.draw_rectangle({xFromLon(minLon),yFromLat(minLat)},{xFromLon(maxLon),yFromLat(maxLat)});//questionable?
    
    drawFeatures(getNumFeatures(),g);
    drawStreetRoads(getNumStreetSegments(), g);
    drawIntersections(getNumIntersections(), g);
}
    



