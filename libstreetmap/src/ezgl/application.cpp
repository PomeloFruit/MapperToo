#include "ezgl/application.hpp"

// A flag to disable event loop (default is false)
bool disable_event_loop = false;

// File paths for direction image icons
const char* straightPath = "libstreetmap/resources/straight.png";
const char* rightPath = "libstreetmap/resources/right turn.png";
const char* leftPath = "libstreetmap/resources/left_turn.png";
const char* slightRightPath = "libstreetmap/resources/slight_right.png";
const char* slightLeftPath = "libstreetmap/resources/slight_left.png"; 
const char* start = "libstreetmap/resources/current.png"; 
const char* end = "libstreetmap/resources/destination.png"; 

namespace ezgl {

void application::startup(GtkApplication *, gpointer user_data)
{
  auto ezgl_app = static_cast<application *>(user_data);
  g_return_if_fail(ezgl_app != nullptr);

  char const *main_ui_resource = ezgl_app->m_main_ui.c_str();

  // Build the main user interface from the XML resource.
  GError *error = nullptr;
  if(gtk_builder_add_from_file(ezgl_app->m_builder, main_ui_resource, &error) == 0) {
    g_error("%s.", error->message);
  }

  for(auto &c_pair : ezgl_app->m_canvases) {
    GObject *drawing_area = ezgl_app->get_object(c_pair.second->id());
    c_pair.second->initialize(GTK_WIDGET(drawing_area));
  }

  g_info("application::startup successful.");
}

void application::activate(GtkApplication *, gpointer user_data)
{
  auto ezgl_app = static_cast<application *>(user_data);
  g_return_if_fail(ezgl_app != nullptr);

  // The main parent window needs to be explicitly added to our GTK application.
  GObject *window = ezgl_app->get_object(ezgl_app->m_window_id.c_str());
  gtk_application_add_window(ezgl_app->m_application, GTK_WINDOW(window));
  
  // Reads from the CSS Style Sheet 
  GtkCssProvider *cssProvider = gtk_css_provider_new ();
  gtk_css_provider_load_from_path(cssProvider,"style.css",NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  
  if(ezgl_app->m_register_callbacks != nullptr) {
    ezgl_app->m_register_callbacks(ezgl_app);
  } else {
    register_default_buttons_callbacks(ezgl_app);
    register_default_events_callbacks(ezgl_app);
  }

  if(ezgl_app->initial_setup_callback != nullptr)
    ezgl_app->initial_setup_callback(ezgl_app);

  g_info("application::activate successful.");
}

application::application(application::settings s)
    : m_main_ui(s.main_ui_resource)
    , m_window_id(s.window_identifier)
    , m_canvas_id(s.canvas_identifier)
    , m_application(gtk_application_new("edu.toronto.eecg.ezgl.app", G_APPLICATION_FLAGS_NONE))
    , m_builder(gtk_builder_new())
    , m_register_callbacks(s.setup_callbacks)
{
  // Connect our static functions application::{startup, activate} to their callbacks. We pass 'this' as the userdata
  // so that we can use it in our static functions.
  g_signal_connect(m_application, "startup", G_CALLBACK(startup), this);
  g_signal_connect(m_application, "activate", G_CALLBACK(activate), this);

  first_run = true;
}

application::~application()
{
  // GTK uses reference counting to track object lifetime. Since we called *_new() for our application and builder, we
  // need to unreference them. This should set their reference count to 0, letting GTK know that they should be cleaned
  // up in memory.
  g_object_unref(m_builder);
  g_object_unref(m_application);
}

canvas *application::get_canvas(const std::string &canvas_id) const
{
  auto it = m_canvases.find(canvas_id);
  if(it != m_canvases.end()) {
    return it->second.get();
  }

  g_warning("Could not find canvas with name %s.", canvas_id.c_str());
  return nullptr;
}

canvas *application::add_canvas(std::string const &canvas_id,
    draw_canvas_fn draw_callback,
    rectangle coordinate_system)
{
  if(draw_callback == nullptr) {
    // A NULL draw callback means the canvas will never render anything to the screen.
    g_warning("Canvas %s's draw callback is NULL.", canvas_id.c_str());
  }

  // Can't use make_unique with protected constructor without fancy code that will confuse students, so we use new
  // instead.
  std::unique_ptr<canvas> canvas_ptr(new canvas(canvas_id, draw_callback, coordinate_system));
  auto it = m_canvases.emplace(canvas_id, std::move(canvas_ptr));

  if(!it.second) {
    // std::map's emplace does not insert the value when the key is already present.
    g_warning("Duplicate key (%s) ignored in application::add_canvas.", canvas_id.c_str());
  } else {
    g_info("The %s canvas has been added to the application.", canvas_id.c_str());
  }

  return it.first->second.get();
}

GObject *application::get_object(gchar const *name) const
{
  // Getting an object from the GTK builder does not increase its reference count.
  GObject *object = gtk_builder_get_object(m_builder, name);
  g_return_val_if_fail(object != nullptr, nullptr);

  return object;
}

int application::run(setup_callback_fn initial_setup_user_callback,
    mouse_callback_fn mouse_press_user_callback,
    mouse_callback_fn mouse_move_user_callback,
    key_callback_fn key_press_user_callback)
{
  if(disable_event_loop)
    return 0;

  initial_setup_callback = initial_setup_user_callback;
  mouse_press_callback = mouse_press_user_callback;
  mouse_move_callback = mouse_move_user_callback;
  key_press_callback = key_press_user_callback;

  // The result of calling g_application_run() again after it returns is unspecified.
  // So we have to destruct and reconstruct the GTKApplication
  if(!first_run) {
    // Destroy the GTK application
    g_object_unref(m_application);
    g_object_unref(m_builder);

    // Reconstruct the GTK application
    m_application = (gtk_application_new("edu.toronto.eecg.ezgl.app", G_APPLICATION_FLAGS_NONE));
    m_builder = (gtk_builder_new());
    g_signal_connect(m_application, "startup", G_CALLBACK(startup), this);
    g_signal_connect(m_application, "activate", G_CALLBACK(activate), this);
  }

  // set the first_run flag to false
  first_run = false;

  g_info("The event loop is now starting.");

  // see: https://developer.gnome.org/gio/unstable/GApplication.html#g-application-run
  return g_application_run(G_APPLICATION(m_application), 0, 0);
}

void application::quit()
{
  // Close the current window
  GObject *window = get_object(m_window_id.c_str());
  gtk_window_close(GTK_WINDOW(window));

  // Quit the GTK application
  g_application_quit(G_APPLICATION(m_application));
}

void application::register_default_events_callbacks(ezgl::application *application)
{
  // Get a pointer to the main window GUI object by using its name.
  std::string main_window_id = application->get_main_window_id();
  GObject *window = application->get_object(main_window_id.c_str());

  // Get a pointer to the main canvas GUI object by using its name.
  std::string main_canvas_id = application->get_main_canvas_id();
  GObject *main_canvas = application->get_object(main_canvas_id.c_str());

  // Connect press_key function to keyboard presses in the MainWindow.
  g_signal_connect(window, "key_press_event", G_CALLBACK(press_key), application);

  // Connect press_mouse function to mouse presses and releases in the MainWindow.
  g_signal_connect(main_canvas, "button_press_event", G_CALLBACK(press_mouse), application);

  // Connect release_mouse function to mouse presses and releases in the MainWindow.
  g_signal_connect(main_canvas, "button_release_event", G_CALLBACK(release_mouse), application);

  // Connect release_mouse function to mouse presses and releases in the MainWindow.
  g_signal_connect(main_canvas, "motion_notify_event", G_CALLBACK(move_mouse), application);

  // Connect scroll_mouse function to the mouse scroll event (up, down, left and right)
  g_signal_connect(main_canvas, "scroll_event", G_CALLBACK(scroll_mouse), application);

}


void application::register_default_buttons_callbacks(ezgl::application *application)
{  
  // Connect press_zoom_fit function to the Zoom-fit button
  GObject *zoom_fit_button = application->get_object("ZoomFitButton");
  g_signal_connect(zoom_fit_button, "clicked", G_CALLBACK(press_zoom_fit), application);

  // Connect press_zoom_in function to the Zoom-in button
  GObject *zoom_in_button = application->get_object("ZoomInButton");
  g_signal_connect(zoom_in_button, "clicked", G_CALLBACK(press_zoom_in), application);

  // Connect press_zoom_out function to the Zoom-out button
  GObject *zoom_out_button = application->get_object("ZoomOutButton");
  g_signal_connect(zoom_out_button, "clicked", G_CALLBACK(press_zoom_out), application);

  // Connect press_up function to the Up button
  GObject *shift_up_button = application->get_object("UpButton");
  g_signal_connect(shift_up_button, "clicked", G_CALLBACK(press_up), application);

  // Connect press_down function to the Down button
  GObject *shift_down_button = application->get_object("DownButton");
  g_signal_connect(shift_down_button, "clicked", G_CALLBACK(press_down), application);

  // Connect press_left function to the Left button
  GObject *shift_left_button = application->get_object("LeftButton");
  g_signal_connect(shift_left_button, "clicked", G_CALLBACK(press_left), application);

  // Connect press_right function to the Right button
  GObject *shift_right_button = application->get_object("RightButton");
  g_signal_connect(shift_right_button, "clicked", G_CALLBACK(press_right), application);

  // Connect press_proceed function to the Proceed button
  GObject *proceed_button = application->get_object("ProceedButton");
  g_signal_connect(proceed_button, "clicked", G_CALLBACK(press_proceed), application);
  
}

void application::update_message(std::string const &message)
{
  // Get the StatusBar Object
  GtkStatusbar *status_bar = (GtkStatusbar *)get_object("StatusBar");

  // Remove all previous messages from the message stack
  gtk_statusbar_remove_all(status_bar, 0);

  // Push user message to the message stack
  gtk_statusbar_push(status_bar, 0, message.c_str());
}

// Connect buttons from Glade to callback functions in M2
void application::connect_feature(button_callback_fn press_find, button_callback_fn press_direction, 
        button_callback_fn press_tourist, button_callback_fn press_food, button_callback_fn press_shop,
        button_callback_fn press_transit, button_callback_fn press_close, button_callback_fn press_findDirection,
        button_callback_fn press_flip, button_callback_fn press_help, button_callback_fn press_sicko){
    
    // Connect press_zoom_out function to the Zoom-out button
    GtkWidget *find_button = (GtkWidget *) this->get_object("FindButton");
    g_signal_connect(G_OBJECT(find_button), "clicked", G_CALLBACK(press_find), this);
    // Show/Hide the direction panel button
    GtkWidget *direction_button = (GtkWidget *) this->get_object("DirectionButton");
    g_signal_connect(G_OBJECT(direction_button), "clicked", G_CALLBACK(press_direction), this);
    // Show the tourist POI toggle button
    GtkWidget *tourist_button = (GtkWidget *) this->get_object("Tourist");
    g_signal_connect(G_OBJECT(tourist_button), "toggled", G_CALLBACK(press_tourist), this);
    // Show the food/Drink POI toggle button
    GtkWidget *food_button = (GtkWidget *) this->get_object("FoodDrink");
    g_signal_connect(G_OBJECT(food_button), "toggled", G_CALLBACK(press_food), this);
    // Show the shops POI toggle button
    GtkWidget *shop_button = (GtkWidget *) this->get_object("Shop");
    g_signal_connect(G_OBJECT(shop_button), "toggled", G_CALLBACK(press_shop), this);
    /// Show the subway/train POI toggle button
    GtkWidget *transit_button = (GtkWidget *) this->get_object("Subway");
    g_signal_connect(G_OBJECT(transit_button), "toggled", G_CALLBACK(press_transit), this);
    // Hide the direction panel button
    GtkWidget *close_button = (GtkWidget *) this->get_object("closeOverlay");
    g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(press_close), this);
    // Search for directions button
    GtkWidget *findDirection_button = (GtkWidget *) this->get_object("directionButton");
    g_signal_connect(G_OBJECT(findDirection_button), "clicked", G_CALLBACK(press_findDirection), this);
    // Flip the inputs from search bar 2 and 3 button
    GtkWidget *flip_button = (GtkWidget *) this->get_object("flipButton");
    g_signal_connect(G_OBJECT(flip_button), "clicked", G_CALLBACK(press_flip), this);
    // Open up help dialog box button
    GtkWidget *help_button = (GtkWidget *) this->get_object("helpButton");
    g_signal_connect(G_OBJECT(help_button), "clicked", G_CALLBACK(press_help), this);
    // Initiate the sicko button
    GtkWidget *sicko_button = (GtkWidget *) this->get_object("sickoButton");
    g_signal_connect(G_OBJECT(sicko_button), "clicked", G_CALLBACK(press_sicko), this);
}

// Get input text from the Searchbars
void application::get_input_text(const char *&street1, const char *&street2,
                                 const char *&street3){
    GtkEntry *street_entry1 = (GtkEntry *) this->get_object("FindStreet1");
    street1 = gtk_entry_get_text(street_entry1);
    GtkEntry *street_entry2 = (GtkEntry *) this->get_object("FindStreet2");
    street2 = gtk_entry_get_text(street_entry2);
    GtkEntry *street_entry3 = (GtkEntry *) this->get_object("FindStreet3");
    street3 = gtk_entry_get_text(street_entry3);
}

// Set the text in the Searchbars
void application::set_input_text(const char *&street1, const char *&street2,
                                 const char *&street3){
    GtkEntry *street_entry1 = (GtkEntry *) this->get_object("FindStreet1");
    gtk_entry_set_text(street_entry1, street1);
    GtkEntry *street_entry2 = (GtkEntry *) this->get_object("FindStreet2");
    gtk_entry_set_text(street_entry2, street2);
    GtkEntry *street_entry3 = (GtkEntry *) this->get_object("FindStreet3");
    gtk_entry_set_text(street_entry3, street3);
}

// Set the text in the Searchbars when the direction panel is open
void application::set_text_in_directions(){
    const char *input;
    std::string blankStr = "";
    const char *blank = blankStr.c_str();
    
    GtkEntry *street_entry1 = (GtkEntry *) this->get_object("FindStreet1");
    input = gtk_entry_get_text(street_entry1);
    //gtk_entry_set_text(street_entry1, blank);
    GtkEntry *street_entry2 = (GtkEntry *) this->get_object("FindStreet2");
    gtk_entry_set_text(street_entry2, input);
    GtkEntry *street_entry3 = (GtkEntry *) this->get_object("FindStreet3");
    gtk_entry_set_text(street_entry3, blank);
}

// Clear the text in the Searchbars when the direction panel is open
void application::clear_direction_inputs(){
    std::string blankStr = "";
    const char *blank = blankStr.c_str();
    
    GtkEntry *street_entry1 = (GtkEntry *) this->get_object("FindStreet1");
    gtk_entry_set_text(street_entry1, blank);
    GtkEntry *street_entry2 = (GtkEntry *) this->get_object("FindStreet2");
    gtk_entry_set_text(street_entry2, blank);
    GtkEntry *street_entry3 = (GtkEntry *) this->get_object("FindStreet3");
    gtk_entry_set_text(street_entry3, blank);
}

// Flip the inputs from the two Searchbars in the direction panel
void application::flip_direction_inputs(){
    const char *temp;
    std::string input2, input3;
    
    GtkEntry *street_entry2 = (GtkEntry *) this->get_object("FindStreet2");
    GtkEntry *street_entry3 = (GtkEntry *) this->get_object("FindStreet3");
    
    input2 = gtk_entry_get_text(street_entry2);
    input3 = gtk_entry_get_text(street_entry3);
    
    temp = input2.c_str();
    gtk_entry_set_text(street_entry3, temp);
    temp = input3.c_str();
    gtk_entry_set_text(street_entry2, temp);
}

// Update the ETA and distance labels 
void application::update_travelInfo(std::string time, std::string distance){
    std::string ETA = "ETA: " + time; 
    std::string Distance = "Distance " + distance; 
    
    GtkWidget *etaText = (GtkWidget*) get_object("ETA");
    GtkWidget *distanceText = (GtkWidget*) get_object("Distance");
    
    gtk_label_set_text((GtkLabel*)etaText, ETA.c_str());
    gtk_label_set_text((GtkLabel*)distanceText, Distance.c_str());
}

// Delete the current directions in the direction panels
void application::destroy_direction(int steps){
    GtkGrid* dGrid = (GtkGrid*) get_object("directionGrid");
    for(int i = 0; i < steps; i++){
        gtk_grid_remove_row(dGrid, 0);
    }
}

// Create new direction instructions in the direction panel 
// Based on the direction type, a different icon will be displayed for the step 
void application::create_direction(const char *instruction, int direction, int step){
    GtkGrid* dGrid = (GtkGrid*) get_object("directionGrid");
    const char* path = ""; 
    if(direction == 0){
        path = straightPath;
    }else if (direction == 1){
        path = rightPath;
    }else if (direction == 2){
        path = leftPath;
    }else if (direction == 3){
        path = slightRightPath;
    }else if (direction == 4){
        path = slightLeftPath; 
    }else if (direction == 5){
        path = start;
    }else if (direction == 6){
        path = end; 
    }
    
    GtkWidget *directionText = gtk_label_new(instruction);
    GtkWidget *directionIcon = gtk_image_new_from_file(path);
    
    gtk_label_set_line_wrap((GtkLabel *) directionText, TRUE);
    gtk_label_set_width_chars((GtkLabel *)directionText, 30);
    gtk_label_set_xalign((GtkLabel *)directionText, 0);
    
    gtk_grid_attach(dGrid, directionIcon, 0, step, 1, 1);
    gtk_grid_attach(dGrid, directionText, 1, step, 1, 1);
    
    gtk_widget_show(directionText);
    gtk_widget_show(directionIcon);
}


void application::create_button(const char *button_text,
    int left,
    int top,
    int width,
    int height,
    button_callback_fn button_func)
{
  // get the internal Gtk grid
  GtkGrid *in_grid = (GtkGrid *)get_object("InnerGrid");

  // create the new button with the given label
  GtkWidget *new_button = gtk_button_new_with_label(button_text);

  // connect the buttons clicked event to the callback
  if(button_func != NULL) {
    g_signal_connect(G_OBJECT(new_button), "clicked", G_CALLBACK(button_func), this);
  }

  // add the new button
  gtk_grid_attach(in_grid, new_button, left, top, width, height);

  // show the button
  gtk_widget_show(new_button);
}

void application::create_button(const char *button_text,
    int insert_row,
    button_callback_fn button_func)
{
  // get the internal Gtk grid
  GtkGrid *in_grid = (GtkGrid *)get_object("InnerGrid");

  // add a row where we want to insert
  gtk_grid_insert_row(in_grid, insert_row);

  // create the button
  create_button(button_text, 0, insert_row, 3, 1, button_func);
}

bool application::destroy_button(const char *button_text_to_destroy)
{
  // get the inner grid
  GtkGrid *in_grid = (GtkGrid *)get_object("InnerGrid");

  // the text to delete, in c++ string form
  std::string text_to_del = std::string(button_text_to_destroy);

  // iterate over all of the children in the grid and find the button by it's text
  GList *children, *iter;
  children = gtk_container_get_children(GTK_CONTAINER(in_grid));
  for(iter = children; iter != NULL; iter = g_list_next(iter)) {
    // iterator to widget
    GtkWidget *widget = GTK_WIDGET(iter->data);

    // check if widget is a button
    if(GTK_IS_BUTTON(widget)) {
      // convert to button
      GtkButton *button = GTK_BUTTON(widget);

      // get the button label
      const char *button_label = gtk_button_get_label(button);
      if(button_label != nullptr) {
        std::string button_text = std::string(button_label);

        // does the label of the button match the one we want to delete?
        if(button_text == text_to_del) {
          // destroy the button (widget) and return true
          gtk_widget_destroy(widget);
          return true;
        }
      }
    }
  }

  // couldn't find the button with the label 'button_text_to_destroy'
  return false;
}

void application::change_button_text(const char *button_text, const char *new_button_text)
{
  // get the inner grid
  GtkGrid *in_grid = (GtkGrid *)get_object("InnerGrid");

  // the text to change, in c++ string form
  std::string text_to_change = std::string(button_text);

  // iterate over all of the children in the grid and find the button by it's text
  GList *children, *iter;
  children = gtk_container_get_children(GTK_CONTAINER(in_grid));
  for(iter = children; iter != NULL; iter = g_list_next(iter)) {
    // iterator to widget
    GtkWidget *widget = GTK_WIDGET(iter->data);

    // check if widget is a button
    if(GTK_IS_BUTTON(widget)) {
      // convert to button
      GtkButton *button = GTK_BUTTON(widget);

      // get the button label
      const char *button_label = gtk_button_get_label(button);
      if(button_label != nullptr) {
        std::string button_text_str = std::string(button_label);

        // does the label of the button match the one we want to change?
        if(button_text_str == text_to_change) {
          // change button label
          gtk_button_set_label(button, new_button_text);
        }
      }
    }
  }
}

void application::refresh_drawing()
{
  // get the main canvas
  canvas *cnv = get_canvas(m_canvas_id);

  // force redrawing
  cnv->redraw();
}
}

void set_disable_event_loop(bool new_setting)
{
  disable_event_loop = new_setting;
}
