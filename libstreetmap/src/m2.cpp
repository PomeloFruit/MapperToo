#include "m1.h"
#include "m2.h"
#include "LatLon.h"

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
#include <iomanip>
#include <map>

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"

//============================ Class Declarations ============================

infoStrucs info;
mapBoundary xy;
populateData pop;
featureDrawing ft;
roadDrawing rd;

//=========================== Function Prototypes ===========================

void draw_main_canvas(ezgl::renderer &g);
std::string clickedOnIntersection(double x, double y);

// Callback functions for event handling
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y);
void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y);
void act_on_key_press(ezgl::application *application, GdkEventKey *event, char *key_name);
void initial_setup(ezgl::application *application);
void test_button(GtkWidget *widget, ezgl::application *application);
///////

//=========================== Function Definitions ===========================

void draw_map(){
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    ezgl::application application(settings);
    
    xy.initialize();
    pop.initialize(info, xy); 

    ezgl::rectangle initial_world({xy.xMin,xy.yMin},{xy.xMax,xy.yMax});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world);

    application.run(NULL, act_on_mouse_press, NULL, NULL);
    //application.run(initial_setup, act_on_mouse_press, act_on_mouse_move, act_on_key_press);
}

void draw_main_canvas(ezgl::renderer &g){
    g.set_color(219,219,219,255); //light gray for background
    g.fill_rectangle(g.get_visible_world());
    
    ft.drawFeatures(getNumFeatures(), info, g);
    rd.drawStreetRoads(getNumStreetSegments(), xy, info, g);
    rd.drawIntersections(getNumIntersections(), xy, info, g);
    ft.drawPOI(getNumPointsOfInterest(), xy, g);
}

// button 1=left, 2 = middle, 3 = right; (event->state & GDK_SHIFT_MASK), (event->state & GDK_CONTROL_MASK) for shift and ctrl keys
// ^^may be useful later
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y){
    std::string message;
    
    message = clickedOnIntersection(x, y);
    
    application->update_message(message);
    application->refresh_drawing();
}

std::string clickedOnIntersection(double x, double y){
    LatLon clickPos;
    unsigned clickedID;
    std::string displayName = "Clicked on: ";
    
    clickPos = xy.LatLonFromXY(x,y);
    clickedID = find_closest_intersection(clickPos);
    displayName = info.IntersectionInfo[clickedID].name;
    
    info.IntersectionInfo[info.lastIntersection].clicked = false;
    info.IntersectionInfo[clickedID].clicked = true;
    info.lastIntersection = clickedID;
    
    return displayName;
}