#include "m1.h"
#include "m2.h"
#include "LatLon.h"

#include "globals.h"
#include "latLonToXY.h"
#include "fillStructs.h"
#include "drawFeatures.h"
#include "drawRoads.h"
#include "clickActions.h"
#include "drawText.h"

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

//============================ Class Objects ============================

infoStrucs info;
mapBoundary xy;
populateData pop;
featureDrawing ft;
roadDrawing rd;
clickActions ck;
drawText dt;


//=========================== Function Prototypes ===========================

void draw_main_canvas(ezgl::renderer &g);
void initial_setup(ezgl::application *application);
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y);
void pressFind(GtkWidget *widget, ezgl::application *application);
void hideSubwayButton(GtkWidget *widget, ezgl::application *application);
void showSubwayButton(GtkWidget *widget, ezgl::application *application);

// Callback functions for event handling


//void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y);
//void act_on_key_press(ezgl::application *application, GdkEventKey *event, char *key_name);

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
    
    dt.initilize(getNumStreetSegments(), initial_world, xy, info);
    
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world);

    application.run(initial_setup, act_on_mouse_press, NULL, NULL);
    //application.run(initial_setup, act_on_mouse_press, act_on_mouse_move, act_on_key_press);
}

void draw_main_canvas(ezgl::renderer &g){
    g.set_color(219,219,219,255); //light gray for background
    g.fill_rectangle(g.get_visible_world());
    
    ft.drawFeatures(getNumFeatures(), info, g);
    rd.drawStreetRoads(getNumStreetSegments(), xy, info, g);
    rd.drawIntersections(getNumIntersections(), xy, info, g);
    ft.drawPOI(getNumPointsOfInterest(), xy, info, g);
    ft.drawSubways(info.showSubway, xy, info, g);
    rd.drawSpecialIntersections(xy,info,g);

    dt.createText(getNumStreetSegments(), getNumStreets(), xy, info, g);
}

void initial_setup(ezgl::application *application){
    application->update_message("Left-click for Points of Interest | Right-click for Intersections");
    application->connect_feature(pressFind);
    
    application->create_button("Hide Subways",8,hideSubwayButton);
}


// left click for POI, right click for intersection
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y){
    std::string message;

    if (event->button == 1) { //left click
        message = ck.clickedOnPOI(x, y, xy, info);
    } else if (event->button == 3) { //right click
        message = ck.clickedOnIntersection(x, y, xy, info);
    }
        

//    if ((event->state & GDK_CONTROL_MASK) && (event->state & GDK_SHIFT_MASK))
//    std::cout << "with control and shift pressed ";
//    else if (event->state & GDK_CONTROL_MASK)
//    std::cout << "with control pressed ";
//    else if (event->state & GDK_SHIFT_MASK)
//    std::cout << "with shift pressed ";
    
    
    application->update_message(message);
    application->refresh_drawing();
}

void pressFind(GtkWidget *widget, ezgl::application *application){
    const char *name1;
    const char *name2;
    std::string message;
    
    application->get_input_text(name1, name2);
    
    info.textInput1 = name1;
    info.textInput2 = name2;

    message = ck.searchOnMap(info);
    application->update_message(message);
    application->refresh_drawing();
}

void hideSubwayButton(GtkWidget *widget, ezgl::application *application){
    info.showSubway = false;
    application->destroy_button("Hide Subways");
    application->create_button("Show Subways",8,showSubwayButton);
    application->refresh_drawing();
}

void showSubwayButton(GtkWidget *widget, ezgl::application *application){
    info.showSubway = true;
    application->destroy_button("Show Subways");
    application->create_button("Hide Subways",8,hideSubwayButton);
    application->refresh_drawing();
}