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

//=========================== Function Prototypes ===========================

void draw_main_canvas(ezgl::renderer &g);
void initial_setup(ezgl::application *application);
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y);
void pressFind(GtkWidget *widget, ezgl::application *application);

// Callback functions for event handling
<<<<<<< HEAD

//void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y);
//void act_on_key_press(ezgl::application *application, GdkEventKey *event, char *key_name);

=======
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y);
void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y);
void act_on_key_press(ezgl::application *application, GdkEventKey *event, char *key_name);
void initial_setup(ezgl::application *application);
void find_button(GtkWidget *widget, ezgl::application *application);
void test_button(GtkWidget *widget, ezgl::application *application);
///////
>>>>>>> Names on roads nearly complete

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
<<<<<<< HEAD
    rd.drawSpecialIntersections(xy, info, g);
=======
<<<<<<< HEAD
    rd.drawOneIntersection(info.lastIntersection, xy, info, g);
>>>>>>> Commiting before pulling new changes to merge
}

void initial_setup(ezgl::application *application){
    application->update_message("Left-click for Points of Interest | Right-click for Intersections");
    application->connect_feature(pressFind);
=======
    dt.createText(getNumStreetSegments(), getNumStreets(), xy, info, g);
}





/* Function called before the activation of the application
 * Can be used to create additional buttons, initialize the status message,
 * or connect added widgets to their callback functions
 */
void initial_setup(ezgl::application *application)
{
  // Update the status bar message
  application->update_message("EZGL Application");

  // Create a Test button and link it with test_button callback fn.
  application->create_button("Test", 6, test_button);
>>>>>>> Commiting before pulling new changes to merge
}

// left click for POI, right click for intersection
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y){
    std::string message;

<<<<<<< HEAD
=======
    application->update_message("Mouse Clicked");

    std::cout << "User clicked the ";

    if (event->button == 1)
      std::cout << "left ";
    else if (event->button == 2)
      std::cout << "middle ";
    else if (event->button == 3)
      std::cout << "right ";

    //std::cout << "mouse button at coordinates (" << x << "," << y << ") ";

    if ((event->state & GDK_CONTROL_MASK) && (event->state & GDK_SHIFT_MASK))
      std::cout << "with control and shift pressed ";
    else if (event->state & GDK_CONTROL_MASK)
      std::cout << "with control pressed ";
    else if (event->state & GDK_SHIFT_MASK)
      std::cout << "with shift pressed ";

   // g_signal_connect(find_button, "clicked", G_CALLBACK(press_find), application);
    
>>>>>>> Commiting before pulling new changes to merge
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
<<<<<<< HEAD

<<<<<<< HEAD
<<<<<<< HEAD
void pressFind(GtkWidget *widget, ezgl::application *application){
    const char *name1;
    const char *name2;
    std::string message;
    
    application->get_input_text(name1, name2);
    
    info.textInput1 = name1;
    info.textInput2 = name2;
    /////////////
    //application->update_message(info.textInput1 + "...." + info.textInput2);
    /////////////
    message = ck.searchOnMap(info);
    application->update_message(message);
    application->refresh_drawing();
}
=======
<<<<<<< HEAD
=======
>>>>>>> Commiting before pulling new changes to merge
=======
/*
>>>>>>> Names on roads nearly complete
void initial_setup(ezgl::application *application){
  application->update_message("Left-click for Points of Interest | "
                                "Right-click for Intersections");
  application->connect_feature(press_find);
//  application->create_button("Find", 9, find_button);
//  application->create_button("I", 10, _button);
}
 * */
/**
 * Function to handle mouse move event
 * The current mouse position in the main canvas' world coordinate system is returned
 * A pointer to the application and the entire GDK event are also returned
 */
/*
void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y){
  std::cout << "Mouse move at coordinates (" << x << "," << y << ") "<< std::endl;
}
*/

/**
 * A ca
 * callback function to test the Test button
 */

void test_button(GtkWidget *widget, ezgl::application *application){
  application->update_message("Test Button Pressed");
  application->refresh_drawing();
}

    
//For now I'm going to just draw everything I need to inside the draw_main_canvas function
//but in the future sometime I plan on putting this stuff inside of seperate functions 
<<<<<<< HEAD

>>>>>>> Pulling to become updated so I can begin working again
>>>>>>> Pulling to become updated so I can begin working again
=======
>>>>>>> Commiting before pulling new changes to merge
