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

//============================ Class Declarations ============================

infoStrucs info;
mapBoundary xy;
populateData pop;
featureDrawing ft;
roadDrawing rd;

//=========================== Function Prototypes ===========================

void draw_main_canvas(ezgl::renderer &g);




////

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

    application.run(initial_setup, act_on_mouse_press, act_on_mouse_move, act_on_key_press);
}

void draw_main_canvas(ezgl::renderer &g){
    g.set_color(219,219,219,255); //light gray for background
    g.fill_rectangle(g.get_visible_world());
    
    ft.drawFeatures(getNumFeatures(), info, g);
    rd.drawStreetRoads(getNumStreetSegments(), xy, info, g);
    rd.drawIntersections(getNumIntersections(), xy, info, g);
    ft.drawPOI(getNumPointsOfInterest(), xy, g);
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
}



/**
 * Function to handle mouse press event
 * The current mouse position in the main canvas' world coordinate system is returned
 * A pointer to the application and the entire GDK event are also returned
 */
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y){
    application->update_message("Mouse Clicked");

    std::cout << "User clicked the ";

    if (event->button == 1)
      std::cout << "left ";
    else if (event->button == 2)
      std::cout << "middle ";
    else if (event->button == 3)
      std::cout << "right ";

    std::cout << "mouse button at coordinates (" << x << "," << y << ") ";

    if ((event->state & GDK_CONTROL_MASK) && (event->state & GDK_SHIFT_MASK))
      std::cout << "with control and shift pressed ";
    else if (event->state & GDK_CONTROL_MASK)
      std::cout << "with control pressed ";
    else if (event->state & GDK_SHIFT_MASK)
      std::cout << "with shift pressed ";

    std::cout << std::endl;
}


/**
 * Function to handle mouse move event
 * The current mouse position in the main canvas' world coordinate system is returned
 * A pointer to the application and the entire GDK event are also returned
 */
void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y){
  std::cout << "Mouse move at coordinates (" << x << "," << y << ") "<< std::endl;
}

/**
 * Function to handle keyboard press event
 * The name of the key pressed is returned (0-9, a-z, A-Z, Up, Down, Left, Right, Shift_R, Control_L, space, Tab, ...)
 * A pointer to the application and the entire GDK event are also returned
 */
void act_on_key_press(ezgl::application *application, GdkEventKey *event, char *key_name){
  application->update_message("Key Pressed");

  std::cout << key_name <<" key is pressed" << std::endl;
}

/**
 * A callback function to test the Test button
 */
void test_button(GtkWidget *widget, ezgl::application *application){
  application->update_message("Test Button Pressed");
  application->refresh_drawing();
}
    
