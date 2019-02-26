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
#include "ezgl/canvas.hpp"

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
void act_on_key_press(ezgl::application *application, GdkEventKey *event, char *key_name);
void findButton(GtkWidget *widget, ezgl::application *application);
void dialog_box(GtkWidget *widget, ezgl::application *application, std::string message);
void on_dialog_response(GtkDialog *dialog);

void hideSubwayButton(GtkWidget *widget, ezgl::application *application);
void showSubwayButton(GtkWidget *widget, ezgl::application *application);
void hideTrainsButton(GtkWidget *widget, ezgl::application *application);
void showTrainsButton(GtkWidget *widget, ezgl::application *application);

void touristButton(GtkWidget *widget, ezgl::application *application);
void fdButton(GtkWidget *widget, ezgl::application *application);
void shopsButton(GtkWidget *widget, ezgl::application *application);
void helpButton(GtkWidget *widget, ezgl::application *application);

void newMap(std::string path, ezgl::application *application);
void initializeMap(); 
void zoomLocation(ezgl::application *application, std::vector<unsigned> zoomVec, int zoomCode);
void zoomAllPoints(ezgl::application *application);

//=========================== Global Variables =========================== 

std::vector<std::pair<std::string, std::string>> city {std::make_pair("beijing","china"), 
        std::make_pair("cairo","egypt"), std::make_pair("cape-town","south-africa"),
        std::make_pair("golden-horseshoe","canada"), std::make_pair("hamilton","canada"),
        std::make_pair("hong-kong","china"), std::make_pair("iceland",""), 
        std::make_pair("interlaken","switzerland"), std::make_pair("london","england"), 
        std::make_pair("moscow","russia"), std::make_pair("new-delhi","india"), 
        std::make_pair("new-york","usa"), std::make_pair("rio-de-janeiro","brazil"), 
        std::make_pair("saint-helena",""), std::make_pair("singapore",""), 
        std::make_pair("sydney","australia"), std::make_pair("tehran","iran"), 
        std::make_pair("tokyo","japan"), std::make_pair("toronto","canada")
};

//=========================== Function Definitions ===========================


/* initializeMap function
 * - initializes all data for all data structures 
 * 
 * @param none
 * @return void
 */

void initializeMap(){
    xy.initialize();
    pop.initialize(info, xy);
    dt.initilize();
}

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
    
    initializeMap(); 
    
    ezgl::application application(settings);
    ezgl::rectangle initial_world({xy.xMin,xy.yMin},{xy.xMax,xy.yMax});

    
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world);
    application.run(initial_setup, act_on_mouse_press, NULL, act_on_key_press);
}


/* draw_main_canvas function
 * - calls for all draw functions for map elements
 * 
 * @param g <ezgl::renderer> - renderer object to draw and modify graphics
 * 
 * @return void
 */

void draw_main_canvas(ezgl::renderer &g){
    double startArea;
    
    g.set_color(219,219,219,255); //light gray for background
    g.fill_rectangle(g.get_visible_world());
    
    ezgl::rectangle startRectangle({xy.xMin,xy.yMin},{xy.xMax,xy.yMax});
    startArea=abs((startRectangle.right()-startRectangle.left())*(startRectangle.top()-startRectangle.bottom()));
    ezgl::rectangle currentRectangle=g.get_visible_world();
    
    double currentArea=abs((currentRectangle.right()-currentRectangle.left())*(currentRectangle.top()-currentRectangle.bottom()));
    double screenRatio=(currentArea/startArea);
    
    ft.drawFeatures(getNumFeatures(), info, g ,currentArea, startArea);
    rd.drawStreetRoads(getNumStreetSegments(), xy, info, g, startArea, currentArea, currentRectangle);
    rd.drawIntersections(getNumIntersections(), xy, info, g);
    ft.drawPOI(getNumPointsOfInterest(), xy, info, g, screenRatio);
    ft.drawSubways(info.showRoute, xy, info, g);
    rd.drawSpecialIntersections(xy,info,g);
    dt.createText(getNumStreetSegments(), getNumStreets(), info, g);
    ft.drawTextOnPOI(g, info);
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
    
    application->create_button("Show Subways",8,showSubwayButton);
    application->create_button("Show Trains",9,showTrainsButton);
    application->create_button("Show Tourist POIs", 10, touristButton); 
    application->create_button("Show Food/Drink POIs", 11, fdButton); 
    application->create_button("Show Shopping POIs", 12, shopsButton);
    application->create_button("Help", 13, helpButton);
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
    
    zoomAllPoints(application);
    
    application->update_message(message);
    application->refresh_drawing();
}


/* act_on_key_press function
 * - allows user to use 'enter' key to search
 * 
 * @param application <ezgl::application> - application object to access window elements
 * @param event <GdkEventButton> -event object to determine mouse action
 * @param key_name <char*> - name of key pressed
 * 
 * @return void
 */

void act_on_key_press(ezgl::application *application, GdkEventKey *, char *key_name) {
    std::string canvasID = application->get_main_canvas_id(); 
    ezgl::canvas* cnv = application->get_canvas(canvasID);
    
    if(strcmp(key_name, "Return") == 0){
        findButton(NULL, application);
    } else if(strcmp(key_name, "Up") == 0){
        ezgl::translate_up(cnv, 5.0);
    } else if(strcmp(key_name, "Down") == 0){
        ezgl::translate_down(cnv, 5.0);
    } else if(strcmp(key_name, "Left") == 0){
        ezgl::translate_left(cnv, 5.0);
    } else if(strcmp(key_name, "Right") == 0){
        ezgl::translate_right(cnv, 5.0); 
    }
}


/* findButton function
 * - writes and reads words in input fields 1 and 2
 * - sets the text in fields to "corrected" name if searched
 * - reads the text in fields to search for
 * 
 * @param widget <GtkWidget> - widget object to determine which gtk object to modify
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void findButton(GtkWidget *widget, ezgl::application *application){

    //cancels warning / not needed at all
    if(widget != NULL){
        widget->parent_instance.ref_count;
    }    
        
    const char *name1;
    const char *name2;
    std::string message;
    std::string path_name; 
    bool changeMap = false;
    
    // get the user input(s))
    application->get_input_text(name1, name2);
    
    //======================= change map ======================================
    /*takes in the city name from name1 and then matches it with the country name 
     * from the city vector. If a match is found, the city/country is sent to newMap()
     */
    
    std::string potentialCityName = name1;
    for(unsigned i=0;i<potentialCityName.length();i++){
        potentialCityName[i]=tolower(potentialCityName[i]);
    }
    
    for(auto it = city.begin(); it != city.end(); it++){
        if(it->first == potentialCityName && it->second.empty()){
            changeMap = true; 
            path_name = it->first;
            
            application->update_message("New map loaded");
            newMap(path_name, application);
            break;
        } else if (it->first == potentialCityName && !(it->second.empty())){
            changeMap = true;
            path_name = it->first + "_" + it->second; 
            
            application->update_message("New map loaded");
            newMap(path_name, application);
            break;
        } 
    }
    
    //======================== Map Element Searching ===========================
    
    if(!changeMap){
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
        
        zoomAllPoints(application);
        
        // reflect the changes
        application->update_message(message);
        application->refresh_drawing();
    }
}


/* dialog_box function
 * - calls for message to be displayed in the dialog box
 * 
 * @param widget <GtkWidget> - gtk object pointer to get access to dialog box
 * @param application <ezgl::application> - application object to access window elements
 * @param message <string> - message to display in the dialog box
 * 
 * @return void
 */

void dialog_box(GtkWidget *widget, ezgl::application *application, std::string message){
    GObject *window; // the parent window over which to add the dialog
    GtkWidget *content_area; // the content area of the dialog (i.e. where to put stuff in the dialog)
    GtkWidget *label; // the label we will create to display a message in the content area
    GtkWidget *dialog; // the dialog box we will create
    
    //cancels warning / not needed at all
    widget->parent_instance.ref_count;
    
    // get a pointer to the main application window
    window = application->get_object(application->get_main_window_id().c_str());
    
    // Create the dialog window. Modal windows prevent interaction with other windows in the same application
    dialog = gtk_dialog_new_with_buttons("MapperToo",(GtkWindow*) window, GTK_DIALOG_MODAL, ("OK"), 
                                    GTK_RESPONSE_ACCEPT, NULL);
    
    // Create a label and attach it to the content area of the dialog
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new(message.c_str());
    gtk_container_add(GTK_CONTAINER(content_area), label);
    
    // The main purpose of this is to show dialog’s child widget, label
    gtk_widget_show_all(dialog);
    
    // Connecting the "response" signal from the user to the associated callback function
    g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(on_dialog_response), NULL);
    
}


/* dialog_box function
 * - calls for dialog box to be destroyed
 * 
 * @param dialog <GtkDialog> - gtk object pointer to get access to dialog box
 * 
 * @return void
 */

void on_dialog_response(GtkDialog *dialog){
    gtk_widget_destroy(GTK_WIDGET (dialog));
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
    if(info.SubwayInfo.size()==0){
        pop.loadAfterDraw(info);
    }
    
    //cancels warning / not needed at all
    widget->parent_instance.ref_count;
        
    info.showRoute = info.showRoute + 1;
    
    application->destroy_button("Show Subways");
    application->create_button("Hide Subways",8,hideSubwayButton);
    
    application->refresh_drawing();
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
    if(info.SubwayInfo.size()==0){
        pop.loadAfterDraw(info);
    }
    //cancels warning / not needed at all
    widget->parent_instance.ref_count;
        
    info.showRoute = info.showRoute + 2;
    
    application->destroy_button("Show Trains");
    application->create_button("Hide Trains",9,hideTrainsButton);
    
    application->refresh_drawing();
}


/* touristButton function
 * - toggles on and off Tourist POIs
 * - and changes button text to match new status
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void touristButton(GtkWidget *widget, ezgl::application *application){
    widget->parent_instance.ref_count;
    
    if(info.poiButtonStatus[0] == 0){
        info.poiButtonStatus[0] = 1;
        application->change_button_text("Show Tourist POIs", "Hide Tourist POIs");
    }else{
        info.poiButtonStatus[0] = 0;
        application->change_button_text("Hide Tourist POIs", "Show Tourist POIs");
    }
    
    application->refresh_drawing();
}


/* fdButton function
 * - toggles on and off Food/Drink POIs
 * - and changes button text to match new status
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void fdButton(GtkWidget *widget, ezgl::application *application){
    widget->parent_instance.ref_count;
    
    if(info.poiButtonStatus[1] == 0){
        info.poiButtonStatus[1] = 1;
        application->change_button_text("Show Food/Drink POIs", "Hide Food/Drink POIs");
    } else {
        info.poiButtonStatus[1] = 0;
        application->change_button_text("Hide Food/Drink POIs", "Show Food/Drink POIs");
    }
    
    application->refresh_drawing();
}


/* shopsButton function
 * - toggles on and off Shopping POIs
 * - and changes button text to match new status
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void shopsButton(GtkWidget *widget, ezgl::application *application){
    widget->parent_instance.ref_count;
    
    if(info.poiButtonStatus[2] == 0){
        info.poiButtonStatus[2] = 1;
        application->change_button_text("Show Shopping POIs", "Hide Shopping POIs");
    }else{
        info.poiButtonStatus[2] = 0;
        application->change_button_text("Hide Shopping POIs", "Show Shopping POIs");
    }
    
    application->refresh_drawing();
}


/* newMap function
 * - closes old map, loads new map and then initializes data structures for new map 
 * - updates the camera and canvas to the dimensions of the new map 
 * 
 * @param path <std::string> - city name for path 
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void newMap(std::string path, ezgl::application *application){
    path = "/cad2/ece297s/public/maps/" + path + ".streets.bin";
    
    pop.clear(info);  
    close_map(); 
    load_map(path); 
    initializeMap();
   
    std::string canvasID = application->get_main_canvas_id(); 
    ezgl::rectangle new_world({xy.xMin,xy.yMin},{xy.xMax,xy.yMax});
    ezgl::canvas* myCanvas = application->get_canvas(canvasID);
    
    myCanvas->get_camera().set_m_screen(new_world);
    myCanvas->get_camera().update_widget(myCanvas->width(), myCanvas->height());
    myCanvas->get_camera().set_world(new_world);
    myCanvas->get_camera().set_zoom_fit(new_world);
    
    application->refresh_drawing(); 
}


/* helpButton function
 * - triggers opening of dialogue box and provides message 
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void helpButton(GtkWidget *widget, ezgl::application *application){
    std::string message;
    
    message = "Welcome to MapperToo - The GIS of magic\n"
            "\n"
            "The search bar at the top can be used to find intersections, streets, POI, Features, and Subways.\n"
            "To search for:\n"
            " - intersections,\n"
            "\t - enter the name of street 1 on the left search box and\n"
            "\t - name of street 2 in the right search box\n"
            " - streets, POI, Features,\n"
            "\t - enter name in the left search box\n"
            " - subway stations, \n"
            "\t  - ensure 'show subways' or 'show trains' button has been clicked\n"
            "\t  - enter the station name in the left search box, ensuring the name ends with 'station'\n"
            "\n"
            "***** Make sure to hit the 'Find' button after entering the names! *****\n"
            "\n"
            " As well, you can access more information by: \n"
            " - left-clicking on POI, where a red marker will be placed\n"
            " - right-clicking on intersections, where a green figure will be placed\n"
            " - left-click while holding the <ctrl> key, to inquire subway stations\n"
            "\n"
            "To change maps, simply enter the city name, and hit find.\n";
            
    dialog_box(widget, application, message.c_str());
}


/* zoomLocation function
 * - determines the position to zoom into when a point is searched or clicked
 * 
 * @param application <ezgl::application> - application object to access window elements
 * @param zoomVec <std::vector<unsigned>> - contains the IDs of the clicked objects/points 
 * @param zoomCode <int> - contains which type of point was clicked 
 * 
 * @return void
 */

void zoomLocation(ezgl::application *application, std::vector<unsigned> zoomVec, int zoomCode){
    if(zoomVec.size() > 0){
        std::string canvasID = application->get_main_canvas_id(); 
        ezgl::canvas* myCanvas = application->get_canvas(canvasID);
        
        double lat = 0; 
        double lon = 0; 
        
        if(zoomCode == 0){
            lat = getPointOfInterestPosition(zoomVec[0]).lat();
            lon = getPointOfInterestPosition(zoomVec[0]).lon();
        }else if (zoomCode == 1){
            lat = getIntersectionPosition(zoomVec[0]).lat();
            lon = getIntersectionPosition(zoomVec[0]).lon();
        }else if (zoomCode == 2){
            lat = info.SubwayInfo[zoomVec[0]].point.lat();
            lon = info.SubwayInfo[zoomVec[0]].point.lon();
        }
        
        double x = xy.xFromLon(lon);
        double y = xy.yFromLat(lat);
        
        ezgl::point2d focusPt(x, y); 
        ezgl::zoom_location(myCanvas, focusPt);
    }
}


/* zoomAllPoints function
 * - provides the correct vector and zoomCode for the zoomLocation function  
 * 
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void zoomAllPoints(ezgl::application *application){
    zoomLocation(application, info.lastPOI, 0);
    zoomLocation(application, info.lastIntersection, 1);
    zoomLocation(application, info.lastSubway, 2);
}