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

#include "directionInfo.h"

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
void setCompletionModel(ezgl::application *application);

void findButton(GtkWidget *, ezgl::application *application);
void recoverStreetsFromInput(std::string input, std::string &retStreet1, std::string &retStreet2);
void closeButton(GtkWidget *, ezgl::application *application);
void flipButton(GtkWidget *, ezgl::application *application);
void directionButton(GtkWidget *, ezgl::application *application);
void dialog_box(GtkWidget *, ezgl::application *application, std::string message);
void on_dialog_response(GtkDialog *dialog);
std::vector<std::pair<std::string, int>>  processInstructions();

void transitButton(GtkWidget *, ezgl::application *application);
void touristButton(GtkWidget *, ezgl::application *application);
void fdButton(GtkWidget *, ezgl::application *application);
void shopsButton(GtkWidget *, ezgl::application *application);
void helpButton(GtkWidget *widget, ezgl::application *application);
void initiateTheSicko(GtkWidget *, ezgl::application *application); 

void newMap(std::string path, ezgl::application *application);
void initializeMap(); 
void zoomLocation(ezgl::application *application, std::vector<unsigned> zoomVec, int zoomCode);
void zoomStreet(ezgl::application *application);
void zoomFeature(ezgl::application *application);
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
    
    if(info.initiateSicko == 1){
        g.set_color(0,0,0,255);
    }else{
        //light gray for background
        g.set_color(219,219,219,255);
    }
     
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
    setCompletionModel(application);
    application->update_message("Left-click for Points of Interest | Right-click for Intersections | <ctrl> + Left-click for Subways ");

    // Connecting the buttons in Glade with their respective call back functions
    application->connect_feature(findButton, directionButton, touristButton, fdButton, 
            shopsButton, transitButton, closeButton, findButton, flipButton, helpButton, 
            initiateTheSicko);
}


/* setCompletionModel function
 * - fills the list store containing all the intersection names for entry completion
 * - only adds intersections if and only if they contain 1 &
 * 
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void setCompletionModel(ezgl::application *application){
    GtkListStore *completeModel = (GtkListStore *) application->get_object("NameSuggestion");
    GtkEntryCompletion *completionBox1 = (GtkEntryCompletion *) application->get_object("NameCompletion1");
    GtkEntryCompletion *completionBox2 = (GtkEntryCompletion *) application->get_object("NameCompletion2");
    GtkEntryCompletion *completionBox3 = (GtkEntryCompletion *) application->get_object("NameCompletion3");
    GtkTreeIter iter;
    
    // make sure entry completion is correct
    gtk_list_store_clear (completeModel);
    gtk_list_store_append(completeModel, &iter);
    gtk_entry_completion_set_text_column(completionBox1, 0);
    gtk_entry_completion_set_text_column(completionBox2, 0);
    gtk_entry_completion_set_text_column(completionBox3, 0);
    
    // add all intersections
    for(unsigned i=0 ; i< static_cast<unsigned>(getNumIntersections()) ; i++){
        std::string str = getIntersectionName(i);
        
        // count the number of &, since our program can only handle one &
        int numAmpersand = std::count(str.begin(), str.end(), '&');
        
        if(numAmpersand == 1){ // only add intersection to model if only one &
            // convert string to char*
            char * nameChar = new char[str.size() + 1];
            std::copy(str.begin(), str.end(), nameChar);
            nameChar[str.size()] = '\0';

            // add new row to list store and store street name
            gtk_list_store_insert(completeModel, &iter, -1);
            gtk_list_store_set(completeModel, &iter, 0, nameChar, -1);

            // free char* memory
            delete[] nameChar;
        }
    }
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
 */        //zoomAllPoints(application);

void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y){
    std::string message;
    
    application->destroy_direction(Hum.humanInstructions.size()+2);
    
    if(info.findDirections){
        if (event->button == 1) { //left click
            ck.clickedOnIntersection(x, y, xy, info, 1);
        } else if (event->button == 3) { // right click
            ck.clickedOnIntersection(x, y, xy, info, 2);
        }
        
        // write clicked intersections to search bar
        const char *name1, *name2, *name3;
        name1 = info.corInput1.c_str();
        name2 = info.corInput2.c_str();
        name3 = info.corInput3.c_str();
        application->set_input_text(name1, name2, name3);
    } else {
        if (event->button == 1) { //left click

            if ((event->state & GDK_CONTROL_MASK) && info.showRoute>0) { // control key   
                message = ck.clickedOnSubway(x, y, xy, info);   
            } else {  //without shift key
                message = ck.clickedOnPOI(x, y, xy, info);
            }

        } else if (event->button == 3) { // right click

            message = ck.clickedOnIntersection(x, y, xy, info, 0);
        }
    }

    // if click generated a valid path, update navigation information
    if(Hum.humanInstructions.size()>0) {
        std::vector<std::pair<std::string, int>> processedInstructions = processInstructions();

        for(int i=0;i<static_cast<int> (processedInstructions.size());i++){
            application->create_direction(processedInstructions[i].first.c_str(), processedInstructions[i].second, i);
            application->update_travelInfo(Hum.totTimePrint, Hum.totDistancePrint);
        }
    } else {
        application->update_travelInfo("", "");
    }
    
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

void findButton(GtkWidget *, ezgl::application *application){ 
        
    const char *name1;
    const char *name2;
    const char *name3; 
    
    std::string message;
    std::string path_name; 
    bool changeMap = false;
    
    // get the user input(s))
    application->get_input_text(name1, name2, name3);
    
    //========================= change map ======================================
    /* takes in the city name from name1 and then matches it with the country name 
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
            
            // clear old directions
            application->destroy_direction(Hum.humanInstructions.size()+2);
            application->update_travelInfo("", "");
            
            application->update_message("New map loaded");
            newMap(path_name, application);
            break;
        } else if (it->first == potentialCityName && !(it->second.empty())){
            changeMap = true;
            path_name = it->first + "_" + it->second; 
            
            // clear old directions
            application->destroy_direction(Hum.humanInstructions.size()+2);
            application->update_travelInfo("", "");
            
            application->update_message("New map loaded");
            newMap(path_name, application);
            break;
        } 
    }
    
    if(changeMap){
        return;
    }
    
    //======================== Map Element Searching ===========================
    application->destroy_direction(Hum.humanInstructions.size()+2);
    
    std::string temp;
    
    //split up the input if there is '&' 
    if(info.findDirections){
        recoverStreetsFromInput(name2, info.textInput1, info.textInput2);
        info.corInput2.clear();
        temp = name2;
        if(temp != info.prevName2){
            info.changedInput2 = true;
        }
        info.prevName2 = temp;
        
        recoverStreetsFromInput(name3, info.textInput3, info.textInput4);
        info.corInput3.clear();
        temp = name3;
        if(temp != info.prevName3){
            info.changedInput3 = true;
        }
        info.prevName3 = temp;
        
        message = ck.searchForDirections(info);
    } else {
        recoverStreetsFromInput(name1, info.textInput1, info.textInput2);
        message = ck.searchOnMap(info);
    }

    // update the search fields with the correct names
    if(info.corInput1 != ""){
        name1 = info.corInput1.c_str();
    }
    if(info.corInput2 != ""){
        name2 = info.corInput2.c_str();
    }
    if(info.corInput3 != ""){
        name3 = info.corInput3.c_str();
    }

    application->set_input_text(name1, name2, name3);
    
    zoomAllPoints(application);

    // add the navigational instructions    
    // reflect the changes  
    if(static_cast<int> (Hum.humanInstructions.size())>0){
        std::vector<std::pair<std::string, int>> processedInstructions = processInstructions();

        for(int i=0;i<static_cast<int> (processedInstructions.size());i++){
            application->create_direction(processedInstructions[i].first.c_str(), processedInstructions[i].second, i);
            application->update_travelInfo(Hum.totTimePrint, Hum.totDistancePrint);
        }
    } else { // clear all information if no path exists
        
        application->update_travelInfo("", "");
    }
    
    application->update_message(message);
    application->refresh_drawing();
}

/* processInstructions function
 * - Takes all the information in the Hum object and forms it into a multiple strings
 * - Strings are stored in a pair with an encoded number to determine the icon to place
 * - After the strings are made it prints them out into console
 * 
 * @param int distance - Distance of the entire street (in meters) rounded up to the nearest whole number
 * @param double time - The total distance in seconds of the entire path
 * 
 * @return void
 */

std::vector<std::pair<std::string, int>> processInstructions(){
    std::vector<std::pair<std::string, int>>  finalInstructions;
    //Since Hum does not have specific instructions set for start and stop intersections
    //the loop is going to start at -1 to fill start intersection information
    //and end at one past the length of the vector in Hum to fill end intersection information
    for(int i=-1;i<static_cast<int>(Hum.humanInstructions.size()+1);i++){
        std::string pushIn;
        int directionDeterminer;
        if((i!=static_cast<int>(Hum.humanInstructions.size())) && (i!=-1)){
            if(i==static_cast<int> (Hum.humanInstructions.size()-1)){
                pushIn="Continue on " + Hum.humanInstructions.at(i).onStreet + " for " +
                Hum.humanInstructions.at(i).distancePrint + " to arrive at your destination ";  
            }
            else if(i==0){
                // Determine if the current direction instruction is to travel straight 
                pushIn= "Proceed on " + Hum.humanInstructions.at(i).onStreet + " for " +
                Hum.humanInstructions.at(i).distancePrint+" then ";
                if(Hum.humanInstructions.at(i).turnPrint=="straight"){         
                   pushIn=pushIn + " continue " + Hum.humanInstructions.at(i).turnPrint
                    +" "+Hum.humanInstructions.at(i).nextStreet;
                } else{  
                   pushIn=pushIn +" turn " + Hum.humanInstructions.at(i).turnPrint
                   +" onto "+Hum.humanInstructions.at(i).nextStreet;
                }

            }
            else{
                pushIn = "Continue on "+ Hum.humanInstructions.at(i).onStreet + " for " +
                Hum.humanInstructions.at(i).distancePrint+" then go "+
                Hum.humanInstructions.at(i).turnPrint + " onto " + 
                Hum.humanInstructions.at(i).nextStreet;
            }
            
            // Determine if the current direction instruction is a left, right, or slightly left/right turn
            if(Hum.humanInstructions.at(i).turnPrint == "left"){
                directionDeterminer=2;
            }else if (Hum.humanInstructions.at(i).turnPrint == "slightly left"){
                directionDeterminer = 4;
            }else if(Hum.humanInstructions.at(i).turnPrint == "right"){
                directionDeterminer=1;
            }else if (Hum.humanInstructions.at(i).turnPrint == "slightly right" ){
                directionDeterminer = 3;
            }else{
                directionDeterminer=0;
            }
            // Determine if the current direction instruction is the starting or ending point 
        } else if(i==-1){
            pushIn="Start at " + Hum.startIntersection;
            std::cout << "=============================================\n";
            std::cout << "Navigation Instructions: \n";
            std::cout << "=============================================\n";
            directionDeterminer=5;
        } else if(static_cast<int> (Hum.humanInstructions.size())==i){
            pushIn="Arrive at destination "+ Hum.endIntersection;
            directionDeterminer=6;
        }
        std::cout<<pushIn<<'\n';
        finalInstructions.push_back(std::make_pair(pushIn, directionDeterminer));
    }
    return finalInstructions;
}

/* recoverStreetsFromInput function
 * - looks for & in the input and seperates the 2 streets from the input field
 * - returns the streets seperated by reference in retStreet1/2
 * 
 * @param input <std::string> - user input into the search bar
 * @param retStreet1 <std::string> - street 1 in the input
 * @param retStreet2 <std::string> - street 2 from the intersection
 * 
 * @return void
 */

void recoverStreetsFromInput(std::string input, std::string &retStreet1, std::string &retStreet2){
    //split the input from the search bars into individual street names if applicable
    int splitEnd, splitStart, tempCombsNum;
    std::vector<std::string> tempCombs = {" & ", " &", "& ", "&"};
    tempCombsNum = 0;
    
    retStreet1 = "";
    retStreet2 = "";
    
    do {
        splitEnd = (int)input.find(tempCombs[tempCombsNum]);
        splitStart = splitEnd + tempCombs[tempCombsNum].size();
        tempCombsNum ++;
    } while(splitEnd == -1 && tempCombsNum < static_cast<int> (tempCombs.size()));
        
    // intersection inputted, so need to split it up
    if(splitEnd != -1){
        for(int i = 0; i < splitEnd; i++){
            retStreet1 = retStreet1 + input[i]; 
        }
        for(unsigned i = splitStart; i < input.length(); i++){
            retStreet2 = retStreet2 + input[i];
        }
    } else {
        retStreet1 = input;
    }
}


/* directionButton function
 * - calls to show the directions panel and hide the POI/Search panel
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void directionButton(GtkWidget *, ezgl::application *application){
    
    GtkWidget *directionBackground = (GtkWidget*)application->get_object("directionBackground");
    GtkWidget *closeOverlay = (GtkWidget*)application->get_object("closeOverlay");
    GtkWidget *directionGrid = (GtkWidget*)application->get_object("directionBackground2");
    
    GtkWidget *searchFrame = (GtkWidget*)application->get_object("SearchFrame");
    GtkWidget *directionButton = (GtkWidget*)application->get_object("DirectionButton");
    GtkWidget *poiGrid = (GtkWidget*)application->get_object("PoiGrid");
    GtkWidget *findButton = (GtkWidget*)application->get_object("FindButton");
    
    
    if(info.findDirections == false){
        info.findDirections = true;
        
        gtk_widget_show(directionBackground);
        gtk_widget_show(closeOverlay);
        gtk_widget_show(directionGrid);
        
        gtk_widget_hide(searchFrame);
        gtk_widget_hide(directionButton);
        gtk_widget_hide(findButton);
        gtk_widget_hide(poiGrid);
    }
    
    application->set_text_in_directions();
}

/* closeButton function
 * - calls to hide the directions panel and show the POI/Search panel
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void closeButton(GtkWidget *, ezgl::application *application){
    
    GtkWidget *directionBackground = (GtkWidget*)application->get_object("directionBackground");
    GtkWidget *closeOverlay = (GtkWidget*)application->get_object("closeOverlay");
    GtkWidget *directionGrid = (GtkWidget*)application->get_object("directionBackground2");
    
    GtkWidget *searchFrame = (GtkWidget*)application->get_object("SearchFrame");
    GtkWidget *directionButton = (GtkWidget*)application->get_object("DirectionButton");
    GtkWidget *poiGrid = (GtkWidget*)application->get_object("PoiGrid");
    GtkWidget *findButton = (GtkWidget*)application->get_object("FindButton");
    
    if(info.findDirections == true){
        info.findDirections = false;
        
        gtk_widget_hide(directionBackground);
        gtk_widget_hide(closeOverlay);
        gtk_widget_hide(directionGrid);
        
        gtk_widget_show(searchFrame);
        gtk_widget_show(directionButton);
        gtk_widget_show(findButton);
        gtk_widget_show(poiGrid);
    }    
    application->clear_direction_inputs();
}


/* flipButton function
 * - calls to flip inputs from search bars in directions panel
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void flipButton(GtkWidget *, ezgl::application *application){
    application->flip_direction_inputs();
    
    unsigned temp1 = info.directionStart;
    unsigned temp2 = info.directionEnd;
    info.directionEnd = temp1;
    info.directionStart = temp2;
    
    std::string temp;
    temp = info.prevName2;
    info.prevName2 = info.prevName3;
    info.prevName3 = temp;
    
    findButton(NULL, application);
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

void dialog_box(GtkWidget *, ezgl::application *application, std::string message){
    GObject *window; // the parent window over which to add the dialog
    GtkWidget *content_area; // the content area of the dialog (i.e. where to put stuff in the dialog)
    GtkWidget *label; // the label we will create to display a message in the content area
    GtkWidget *dialog; // the dialog box we will create
    
    // get a pointer to the main application window
    window = application->get_object(application->get_main_window_id().c_str());
    
    // Create the dialog window. Modal windows prevent interaction with other windows in the same application
    dialog = gtk_dialog_new_with_buttons("MapperToo Help",(GtkWindow*) window, GTK_DIALOG_MODAL, ("Close Help"), 
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


/* transitButton function
 * - calls to show/hide the transit information
 * 
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void transitButton(GtkWidget *, ezgl::application *application){
    if(info.SubwayInfo.size()==0){
        pop.loadAfterDraw(info);
    }
    
    if(info.poiButtonStatus[3] == 0){
        info.poiButtonStatus[3] = 1;
        info.showRoute = info.showRoute + 1;
    } else {
        info.poiButtonStatus[3] = 0;
        info.showRoute = info.showRoute - 1;
    }
    
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

void touristButton(GtkWidget *, ezgl::application *application){
    
    if(info.poiButtonStatus[0] == 0){
        info.poiButtonStatus[0] = 1;
    }else{
        info.poiButtonStatus[0] = 0;
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

void fdButton(GtkWidget *, ezgl::application *application){

    if(info.poiButtonStatus[1] == 0){
        info.poiButtonStatus[1] = 1;
    } else {
        info.poiButtonStatus[1] = 0;
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

void shopsButton(GtkWidget *, ezgl::application *application){

    if(info.poiButtonStatus[2] == 0){
        info.poiButtonStatus[2] = 1;
    }else{
        info.poiButtonStatus[2] = 0;
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
    Hum.clear();
    close_map(); 
    load_map(path); 
    initializeMap();
    application->clear_direction_inputs();
    
    std::string canvasID = application->get_main_canvas_id(); 
    ezgl::rectangle new_world({xy.xMin,xy.yMin},{xy.xMax,xy.yMax});
    ezgl::canvas* myCanvas = application->get_canvas(canvasID);
    
    myCanvas->get_camera().set_m_screen(new_world);
    myCanvas->get_camera().update_widget(myCanvas->width(), myCanvas->height());
    myCanvas->get_camera().set_world(new_world);
    myCanvas->get_camera().set_zoom_fit(new_world);
    
    setCompletionModel(application);
    
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
            "\t - enter the name of the first street followed by ' & ' and then the second street. \n"
            " - streets, POI, Features,\n"
            "\t - enter name in the search box\n"
            " - subway stations, \n"
            "\t  - ensure 'show subways' or 'show trains' button has been clicked\n"
            "\t  - enter the station name in the search box, ensuring the name ends with 'station'\n"
            "\n"
            "***** Press the Enter key or the Magnifying Glass/Find button to search *****\n"
            "\n"
            "To search for directions from Point A to Point B: \n"
            "\t - Press the Directions Button beside the Find button\n"
            "\t - Left Click to set the Starting Point - marked by a Red Pointer.\n"
            "\t - Right Click to set the Ending Destination - marked by a Purple Flag.\n"
            "\t - You can press the Swap key to switch the start and end points.\n"
            "\t - You can press the 'X' icon on the side of the search bar to close the Direction Panel and return to the regular map.\n"
            "\n"
            " As well, you can access more information by: \n"
            " - Clicking on the POI icons on the top left bar will show/hide POIs of that type on the map.\n"
            " - left-clicking while the direction panel is closed will show the nearest POI - marked by a Red Pointer.\n"
            " - right-clicking while the direction panel is closed will show the nearest intersections - marked by a Green Human.\n"
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


/* zoomStreet function
 * - zooms into the specified street using the points from getCornerStreetSeg
 * 
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void zoomStreet(ezgl::application *application){
    //get the corners of the rectangles if they exist (190 means they do not exist)
    LatLon topPt, botPt; 
    std::tie(topPt, botPt) = ck.getCornerStreetSeg(info); 
    if(topPt.lat() == 190){
        return; 
    }
    
    std::string canvasID = application->get_main_canvas_id(); 
    ezgl::canvas* cnv = application->get_canvas(canvasID);
    
    //find the dimensions of the new rectangle/viewing area
    double x1 = xy.xFromLon(topPt.lon());
    double y1 = xy.yFromLat(topPt.lat());
    double x2 = xy.xFromLon(botPt.lon());
    double y2 = xy.yFromLat(botPt.lat());
    
    double deltaX = abs(x2 - x1);
    double deltaY = abs(y2 - y1);
    const double BUFFER = 2.0;
    
    double currentH = cnv->get_camera().get_world().height();
    double currentW = cnv->get_camera().get_world().width();
    double ratioHW = currentH/currentW;
    
    const double MIN_AREA = 8e-04;
    const double MIN_X = sqrt(MIN_AREA/ratioHW);
    const double MIN_Y = MIN_AREA/MIN_X;
    const double MINDIVIDER = 4.0;
    const double MAXMULTIPLIER = 0.57;
    const double MAX_X = cnv->get_camera().get_initial_world().width();
    const double MAX_Y = cnv->get_camera().get_initial_world().height();
    
    double recX, recY;
    
    //scale the rectangle to the appropriate ratio of the screen 
    if(deltaX <= deltaY) {
        recX = deltaY/ratioHW*BUFFER;
        recY = deltaY*BUFFER;
    } else {
        recX = deltaX*BUFFER;
        recY = deltaX*ratioHW*BUFFER;
    }
    
    double adjustmentFactor = recX/2;
    
    //change the viewing area
    double startX = (x1 + x2 - recX)/2; 
    double startY = (y1 + y2 - recY)/2;
    
    // for directions, adjust the position to the right since search bar
    // also, set minimum zoom distance
    if(info.findDirections){
        startX = (x1 + x2 - recX - adjustmentFactor)/2;
        if(deltaX < MIN_X/MINDIVIDER && deltaY < MIN_Y/MINDIVIDER){
            startX = (x1 + x2 - MIN_X - MIN_X/2)/2;
            startY = (y1 + y2 - MIN_Y)/2;
        } else if(deltaX > MAX_X*MAXMULTIPLIER || deltaY > MAX_Y*MAXMULTIPLIER){
            startX = 0;
            startY = 0;
        }
    } else {
        if(deltaX < MIN_X/MINDIVIDER && deltaY < MIN_Y/MINDIVIDER){
            startX = (x1 + x2 - MIN_X - MIN_X/2)/2;
            startY = (y1 + y2 - MIN_Y)/2;
        } else if(deltaX > MAX_X*MAXMULTIPLIER || deltaY > MAX_Y*MAXMULTIPLIER){
            startX = 0;
            startY = 0;
        }
    }
        
    ezgl::point2d focusPt(startX, startY);
    ezgl::rectangle zoomArea (focusPt, MIN_X,  MIN_Y);
    
    if(startX == 0 && startY == 0){
        zoomArea = cnv->get_camera().get_initial_world();
    } else if(recX > MIN_X){
        ezgl::rectangle temp (focusPt, recX, recY);
        zoomArea = temp;
    }
    
    cnv->get_camera().set_world(zoomArea);
    cnv->redraw();
}


/* zoomFeature function
 * - zooms into the specified feature by iterating through the feature and finding
 * - its maximum and minimum x and y points
 * 
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void zoomFeature(ezgl::application *application){
    double x1, x2, y1, y2; 
    if(info.lastFeature.size() > 0){
        std::vector<ezgl::point2d> feature = info.FeaturePointVec[info.lastFeature[0]]; 
        ezgl::point2d startPt = feature[0]; 
        
        x1 = startPt.x;
        y1 = startPt.y;
        x2 = startPt.x;
        y2 = startPt.y;
        
        //find the maximum and minimum x and y points of the feature
        for(unsigned i = 0; i < feature.size(); i++){
            startPt = feature[i]; 
            if(startPt.x < x1){
                x1 = startPt.x; 
            }else if (startPt.x > x2){
                x2 = startPt.x; 
            }

            if(startPt.y < y2){
                y2 = startPt.y; 
            }else if (startPt.y > y1){
                y1 = startPt.y; 
            }
        }
                
        std::string canvasID = application->get_main_canvas_id(); 
        ezgl::canvas* cnv = application->get_canvas(canvasID);
        
        //find the dimensions of the new rectangle/viewing area
        double deltaX = x2 - x1;
        double deltaY = y2 - y1;
        const double BUFFER = 5;  

        double currentH = cnv->get_camera().get_world().height();
        double currentW = cnv->get_camera().get_world().width();
        double ratioHW = currentH/currentW;

        double recX, recY;

        //scale the rectangle to the appropriate ratio of the screen 
        if(abs(deltaX) <= abs(deltaY)) {
            recX = abs(deltaY)/ratioHW*BUFFER;
            recY = abs(deltaY)*BUFFER;
        } else {
            recX = abs(deltaX)*BUFFER;
            recY = abs(deltaX)*ratioHW*BUFFER;
        }


        //change the viewing area
        double startX = (x1 + x2 - recX)/2;
        double startY = (y1 + y2 - recY)/2;

        ezgl::point2d focusPt(startX, startY); 
        ezgl::rectangle zoomArea (focusPt, recX,  recY); 
        if(zoomArea.area() < cnv->get_camera().get_initial_world().area()){
            cnv->get_camera().set_world(zoomArea);
            cnv->redraw();
        } else {
            cnv->get_camera().set_world(cnv->get_camera().get_initial_world());
            cnv->redraw();
        }
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
    zoomStreet(application);
    zoomFeature(application);
}


/* initiateTheSicko function
 * - literally the best function ever 
 * - Drake shows up  
 *
 * @param widget <GtkWidget> -event object to determine mouse action
 * @param application <ezgl::application> - application object to access window elements
 * 
 * @return void
 */

void initiateTheSicko(GtkWidget *, ezgl::application *application){
    std::string message;
    
    if(info.initiateSicko == 0){
        info.initiateSicko = 1;
        message = "Warning, you may be in awe by how amazing this mode is.\n"
            "Once you go sicko, you can never go back!\n"
            "If you look closely enough at the CN Tower you can see Drake.";
    }else{
        info.initiateSicko = 0;
        message = "You have left sicko mode.\n"
            "Your life will never be the same!\n";
    }
    
    dialog_box(NULL, application, message.c_str());
            
    application->refresh_drawing(); 
}
