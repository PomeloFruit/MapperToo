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
#include "grid.h"

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
void findButton(GtkWidget *widget, ezgl::application *application);
void loadSubwayButton(GtkWidget *widget, ezgl::application *application);
void hideSubwayButton(GtkWidget *widget, ezgl::application *application);
void showSubwayButton(GtkWidget *widget, ezgl::application *application);
void loadTrainsButton(GtkWidget *widget, ezgl::application *application);
void hideTrainsButton(GtkWidget *widget, ezgl::application *application);
void showTrainsButton(GtkWidget *widget, ezgl::application *application);

//=========================== Global Variables ===========================

double startArea; //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< this should really be in global.h

//=========================== Function Definitions ===========================

/* draw_map function
 * - loads the settings for the application window
 * - calls for initialization of everything needed to draw
 * 
 * @param none
 * @return void
 */

void draw_map(){
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    ezgl::application application(settings);
    
    //see if we can move this stuff away to another function for change map
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    xy.initialize();
    pop.initialize(info, xy);

    ezgl::rectangle initial_world({xy.xMin,xy.yMin},{xy.xMax,xy.yMax});
        
    startArea=abs((initial_world.right()-initial_world.left())*(initial_world.top()-initial_world.bottom()));
    
    dt.initilize(getNumStreetSegments(), initial_world, xy, info);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world);

    application.run(initial_setup, act_on_mouse_press, NULL, NULL);
}


/* draw_main_canvas function
 * - calls for all draw functions for map elements
 * 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * 
 * @return void
 */

void draw_main_canvas(ezgl::renderer &g){
    g.set_color(219,219,219,255); //light gray for background
    g.fill_rectangle(g.get_visible_world());
    
    ezgl::rectangle currentRectangle=g.get_visible_world();
    double currentArea=abs((currentRectangle.right()-currentRectangle.left())*(currentRectangle.top()-currentRectangle.bottom()));
    double adjustmentFactor=1/(currentArea/startArea);//for some reason it's not letting me take in the visible screen stuff, so I'm just going to do it here
    double screenRatio=(currentArea/startArea);
//    
//    std::cout<<adjustmentFactor<<currentArea<<startArea<<'\n';
    
    ft.drawFeatures(getNumFeatures(), info, g);
    rd.drawStreetRoads(getNumStreetSegments(), xy, info, g, startArea, currentArea);
    rd.drawIntersections(getNumIntersections(), xy, info, g);
    ft.drawPOI(getNumPointsOfInterest(), xy, info, g, screenRatio, currentArea);
    ft.drawSubways(info.showRoute, xy, info, g);
    rd.drawSpecialIntersections(xy,info,g);


    dt.createText(getNumStreetSegments(), getNumStreets(), xy, info, g);
 //   std::cout<<currentArea<<'\n';
}


/* initial_setup function
 * - creates initial buttons and puts instructional message in status bar at startup
 * 
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void initial_setup(ezgl::application *application){
    application->update_message("Left-click for Points of Interest | Right-click for Intersections | <ctrl> + Left-click for Subways ");
    application->connect_feature(findButton);
    
    application->create_button("Show Subways",8,loadSubwayButton);
    application->create_button("Show Trains",9,loadTrainsButton);
}


/* act_on_mouse_press function
 * - determines mouse-click/key combination and calls for appropriate action
 * - calls for update of graphics to reflect change, and updates status
 * 
 * - left click alone finds POI
 * - right click alone gets intersections
 * - left-click with CTRL key gets subways/routes
 * 
 * @param application <ezgl::application> - application object to access window elements
 * @param event <GdkEventButton> -event object to determine mouse action
 * @param x <double> - x coordinate of the mouse click (in screen co-ords)
 * @param y <double> - y coordinate of the mouse click (in screen co-ords)
 * 
 * @return void
 */

void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y){
    std::string message;

    if (event->button == 1) { //left click
        
        if ((event->state & GDK_CONTROL_MASK) && info.showRoute>0) { // control key   
            message = ck.clickedOnSubway(x, y, xy, info);   
        } else {  //without shift key
            message = ck.clickedOnPOI(x, y, xy, info);
        }
        
    } else if (event->button == 3) { // right click
        
        message = ck.clickedOnIntersection(x, y, xy, info);
    }
    
    application->update_message(message);
    application->refresh_drawing();
}


/* findButton function
 * - writes and reads words in input fields 1 and 2
 * - sets the text in fields to "corrected" name if searched
 * - reads the text in fields to search for
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void findButton(GtkWidget *widget, ezgl::application *application){

    //cancels warning / not needed at all
    widget->parent_instance.ref_count;
        
    const char *name1;
    const char *name2;
    std::string message;
    
    // do the search
    application->get_input_text(name1, name2);
    
    info.textInput1 = name1;
    info.textInput2 = name2;

    message = ck.searchOnMap(info);
    
    // auto-correct the input
    if(info.corInput1 != ""){
        name1 = info.corInput1.c_str();
    }
    if(info.corInput2 != ""){
        name2 = info.corInput2.c_str();
    }
    application->set_input_text(name1, name2);
    
    // reflect the changes
    application->update_message(message);
    application->refresh_drawing();
}



void loadMapButton(GtkWidget *widget, ezgl::application *application){
    const char *map_name; 
    
    
}

/* loadSubwayButton function
 * - calls for the population of the subway structures (if not already populated)
 * - calls another function to switch button to "hide subway"
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */
void loadSubwayButton(GtkWidget *widget, ezgl::application *application){
    if(info.SubwayInfo.size()==0){
        pop.loadAfterDraw(info);
    }
    
    showSubwayButton(widget, application);
}


/* hideSubwayButton function
 * - calls to change button from hide to show
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void hideSubwayButton(GtkWidget *widget, ezgl::application *application){
    
    //cancels warning / not needed at all
    widget->parent_instance.ref_count;
        
    info.showRoute = info.showRoute - 1;

    application->destroy_button("Hide Subways");
    application->create_button("Show Subways",8,showSubwayButton);
    
    application->refresh_drawing();
}


/* showSubwayButton function
 * - calls to change button from show to hide
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void showSubwayButton(GtkWidget *widget, ezgl::application *application){
    
    //cancels warning / not needed at all
    widget->parent_instance.ref_count;
        
    info.showRoute = info.showRoute + 1;
    
    application->destroy_button("Show Subways");
    application->create_button("Hide Subways",8,hideSubwayButton);
        
    application->refresh_drawing();
}


/* loadTrainsButton function
 * - calls for the population of the subway structures (if not already populated)
 * - calls another function to switch button to "hide trains"
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void loadTrainsButton(GtkWidget *widget, ezgl::application *application){
    if(info.SubwayInfo.size()==0){
        pop.loadAfterDraw(info);
    }
    showTrainsButton(widget, application);
}


/* hideTrainsButton function
 * - calls to change button from hide to show
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void hideTrainsButton(GtkWidget *widget, ezgl::application *application){
    
    //cancels warning / not needed at all
    widget->parent_instance.ref_count;
        
    info.showRoute = info.showRoute - 2;
    
    application->destroy_button("Hide Trains");
    application->create_button("Show Trains",9,showTrainsButton);
    
    application->refresh_drawing();
}


/* showTrainsButton function
 * - calls to change button from show to hide
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void showTrainsButton(GtkWidget *widget, ezgl::application *application){
    
    //cancels warning / not needed at all
    widget->parent_instance.ref_count;
        
    info.showRoute = info.showRoute + 2;
    
    application->destroy_button("Show Trains");
    application->create_button("Hide Trains",9,hideTrainsButton);
        
    application->refresh_drawing();
}