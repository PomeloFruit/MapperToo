#include "m1.h"
#include "m2.h"

#include "globals.h"
#include "latLonToXY.h"
#include "fillStructs.h"
#include "drawFeatures.h"
#include "drawRoads.h"

#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "OSMID.h"

#include <math.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <map>

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"

infoStrucs info;
mapBoundary xy;
populateData pop;
featureDrawing ft;
roadDrawing rd;

void draw_main_canvas(ezgl::renderer &g);

void draw_map(){
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    ezgl::application application(settings);

    xy.initialize();
    xy.setAverageLat(); 
    pop.initialize(info, xy);   
    
    pop.populateOSMWayInfo(info);
    pop.populateStreetSegInfo(info);
    pop.populateIntersectionInfo(info);

    pop.populatePOIInfo(info);
     
    double xMax, xMin, yMax, yMin; 
    xMax = xy.xFromLon(xy.maxLon);
    xMin = xy.xFromLon(xy.minLon);
    yMax = xy.yFromLat(xy.maxLat);
    yMin = xy.yFromLat(xy.minLat); 
    
    pop.populateFeatureInfo(info, xy);

    ezgl::rectangle initial_world({xMin,yMin},{xMax,yMax});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world);
    
    application.run(NULL,NULL,NULL,NULL);
    
}

void draw_main_canvas(ezgl::renderer &g){
    g.set_color(219,219,219,255); //light gray for background
    g.fill_rectangle(g.get_visible_world());
    
    ft.drawFeatures(getNumFeatures(), info, g);
    rd.drawStreetRoads(getNumStreetSegments(), xy, info, g);
    rd.drawIntersections(getNumIntersections(), xy, info, g);
    ft.drawPOI(getNumPointsOfInterest(), xy, g);
}
    



