#ifndef EZGL_APPLICATION_HPP
#define EZGL_APPLICATION_HPP

#include <ezgl/canvas.hpp>
#include <ezgl/control.hpp>
#include <ezgl/callback.hpp>

#include <map>
#include <memory>
#include <string>

#include <gtk/gtk.h>

/**
 * A library for creating a graphical user interface.
 */
namespace ezgl {

class application;

/**
 * The signature of a function that connects GObject to functions via signals.
 *
 * @see application::get_object.
 */
using connect_g_objects_fn = void (*)(application *app);

/**
 * The signature of a setup callback function
 */
using setup_callback_fn = void (*)(application *app);

/**
 * The signature of a button callback function
 */
using button_callback_fn = void (*)(GtkWidget *widget, application *app);

/**
 * The signature of a user-defined callback function for mouse events
 */
using mouse_callback_fn = void (*)(application *app, GdkEventButton *event, double x, double y);

/**
 * The signature of a user-defined callback function for keyboard events
 */
using key_callback_fn = void (*)(application *app, GdkEventKey *event, char *key_name);

/**
 * The core application.
 *
 * The GUI of an application is created from an XML file. Widgets created in the XML file can be retrieved from an
 * application object, but only after the object has been initialized by GTK via application::run.
 */
class application {
public:
  /**
   * Configuration settings for the application.
   *
   * The GUI will be built from the XML description given by main_ui_resource.
   * The XML file must contain a GtkWindow with the name in window_identifier.
   */
  struct settings {
    /**
     * The file path that contains the XML file, which describes the GUI.
     */
    std::string main_ui_resource;

    /**
     * The name of the main window in the XML file.
     */
    std::string window_identifier;

    /**
     * The name of the main canvas in the XML file.
     */
    std::string canvas_identifier;

    /**
     * Specficy the function that will connect GUI objects to user-defined callbacks.
     *
     * GUI objects (i.e., a GObject) can be retrieved from this application object. These objects can then be connected
     * to specific events using g_signal_connect. A list of signals that can be used to make these connections can be
     * found <a href = "https://developer.gnome.org/gtk3/stable/GtkWidget.html#GtkWidget.signals">here</a>.
     *
     * @see application::get_object
     */
    connect_g_objects_fn setup_callbacks = nullptr;
  };

public:
  /**
   * Create an application.
   *
   * @param s The preconfigured settings.
   */
  explicit application(application::settings s);

  /**
   * Add a canvas to the application.
   *
   * If the canvas has already been added, it will not be overwritten and a warning will be displayed.
   *
   * @param canvas_id The id of the GtkDrawingArea in the XML file.
   * @param draw_callback The function to call that draws to this canvas.
   * @param coordinate_system The initial coordinate system of this canvas.
   *
   * @return A pointer to the newly created cavas.
   */
  canvas *add_canvas(std::string const &canvas_id,
      draw_canvas_fn draw_callback,
      rectangle coordinate_system);
  
  
  //connect features that were created not default
  void connect_feature(button_callback_fn press_find, button_callback_fn press_direction, 
        button_callback_fn press_tourist, button_callback_fn press_food, button_callback_fn press_shop,
        button_callback_fn press_transit, button_callback_fn press_close, button_callback_fn press_findDirection,
        button_callback_fn press_flip, button_callback_fn press_help, button_callback_fn press_sicko);
  
  //retrieve input text from text fields
  void get_input_text(const char *&street1, const char *&street2, const char *&street3);
  // Set the text in the Searchbars
  void set_input_text(const char *&street1, const char *&street2, const char *&street3);
  // Set the text in the Searchbars when the direction panel is open
  void set_text_in_directions();
  // Clear the text in the Searchbars when the direction panel is open
  void clear_direction_inputs();
  // Flip the inputs from the two Searchbars in the direction panel
  void flip_direction_inputs();
  
  /*
   *Updates the ETA and Distance labels in the display 
   * 
   * @param time the ETA time 
   * @param distance the total distance traveled to reach destination
   *    
   */
  void update_travelInfo(std::string time, std::string distance);
  
  /*
   * Creates a new direction instruction on the display 
   * 
   * @param instruction the text to be displayed in the direction 
   * @param direction  the direction that the path is going 
   * @param step    the current step of the path 
   */
  void create_direction(const char *instruction, int direction, int step);
  
  /*
   * Deletes all direction instructions in the grid 
   * 
   * @param step  the total number of steps in the path
   */
  void destroy_direction(int steps);
  
  /**
   * Add a button
   *
   * @param button_text the new button text
   * @param left the column number to attach the left side of the new button to
   * @param top the row number to attach the top side of the new button to
   * @param width the number of columns that the button will span
   * @param height the number of rows that the button will span
   * @param button_func callback function for the button
   */
  void create_button(const char *button_text,
      int left,
      int top,
      int width,
      int height,
      button_callback_fn button_func);

  /**
   * Add a button convenience
   * Adds a button at a given row index (assuming buttons in the right bar use 1 row each)
   * by inserting a row in the grid and adding the button. Uses the default width of 3 and height of 1
   * 
   * @param button_text the new button text
   * @param insert_row the row in the right bar to insert the button
   * @param button_func callback function for the button
   */
  void create_button(const char *button_text, int insert_row, button_callback_fn button_func);

  /**
   * Deletes a button by its label (displayed text)
   * 
   * @param the text of the button to delete
   * @return whether the button was found and deleted
   */
  bool destroy_button(const char *button_text_to_destroy);

  /**
   * Change the label of the button (displayed text)
   *
   * @param button_text the old text of the button
   * @param new_button_text the new button text
   */
  void change_button_text(const char *button_text, const char *new_button_text);

  /**
   * Update the message in the status bar
   *
   * @param message The message that will be displayed on the status bar
   */
  void update_message(std::string const &message);

  /**
   * refresh the DrawingArea
   */
  void refresh_drawing();

  /**
   * Run the application.
   *
   * Once this is called, the application will be initialized first. Initialization will build the GUI based on the XML
   * resource given in the constructor. Once the GUI has been created, the function initial_setup_user_callback will be
   * called.
   *
   * After initialization, control of the program will be given to GTK. You will only regain control for the signals
   * that you have registered callbacks for.
   *
   * @param initial_setup_user_callback A user-defined function that is called before application activation
   * @param mouse_press_user_callback The user-defined callback function for mouse press
   * @param mouse_move_user_callback The user-defined callback function for mouse move
   * @param key_press_user_callback The user-defined callback function for keyboard press
   *
   * @return The exit status.
   */
  int run(setup_callback_fn initial_setup_user_callback,
      mouse_callback_fn mouse_press_user_callback,
      mouse_callback_fn mouse_move_user_callback,
      key_callback_fn key_press_user_callback);

  /**
   * Destructor.
   */
  ~application();

  /**
   * Copies are disabled.
   */
  application(application const &) = delete;

  /**
   * Copies are disabled.
   */
  application &operator=(application const &) = delete;

  /**
   * Ownership of an application is transferrable.
   */
  application(application &&) = default;

  /**
   * Ownership of an application is transferrable.
   */
  application &operator=(application &&) = default;

  /**
   * Retrieve a pointer to a canvas that was previously added to the application.
   *
   * Calling this function before application::run results in undefined behaviour.
   *
   * @param canvas_id The key used when the canvas was added.
   *
   * @return A non-owning pointer, or nullptr if not found.
   *
   * @see application::get_object
   */
  canvas *get_canvas(std::string const &canvas_id) const;

  /**
   * Retrieve a GLib Object (i.e., a GObject).
   *
   * This is useful for retrieving GUI elements specified in your XML file(s). You should only call this function after
   * the application has been run, otherwise the GUI elements will have not been created yet.
   *
   * @param name The ID of the object.
   * @return The object with the ID, or NULL if it could not be found.
   *
   * @see application::run
   */
  GObject *get_object(gchar const *name) const;

  /**
   * Get the ID of the main window
   */
  std::string get_main_window_id() const
  {
    return m_window_id;
  }

  /**
   * Get the ID of the main canvas
   */
  std::string get_main_canvas_id() const
  {
    return m_canvas_id;
  }

  /**
   * Quit the application
   */
  void quit();

private:
  // The package path to the XML file that describes the UI.
  std::string m_main_ui;

  // The ID of the main window to add to our GTK application.
  std::string m_window_id;

  // The ID of the main canvas
  std::string m_canvas_id;

  // The GTK application.
  GtkApplication *m_application;

  // The GUI builder that parses an XML user interface.
  GtkBuilder *m_builder;

  // The function to call when the application is starting up.
  connect_g_objects_fn m_register_callbacks;

  // The collection of canvases added to the application.
  std::map<std::string, std::unique_ptr<canvas>> m_canvases;

  // A flag that indicates if the run() was called before or not to allow multiple reruns
  bool first_run;

private:
  // Called when our GtkApplication is initialized for the first time.
  static void startup(GtkApplication *gtk_app, gpointer user_data);

  // Called when GTK activates our application for the first time.
  static void activate(GtkApplication *gtk_app, gpointer user_data);

  // Called during application activation to setup the default callbacks for the prebuilt buttons
  static void register_default_buttons_callbacks(application *application);

  // Called during application activation to setup the default callbacks for the mouse and key events
  static void register_default_events_callbacks(application *application);

public:
  // The user-defined initial setup callback function
  setup_callback_fn initial_setup_callback;

  // The user-defined callback function for handling mouse press
  mouse_callback_fn mouse_press_callback;

  // The user-defined callback function for handling mouse move
  mouse_callback_fn mouse_move_callback;

  // The user-defined callback function for handling keyboard press
  key_callback_fn key_press_callback;
};
}

/**
 * Set the disable_event_loop flag to new_setting
 * Call with new_setting == true to make the event_loop immediately return.
 * Needed only for auto-marking
 *
 * @param new_setting The new state of disable_event_loop flag
 */
void set_disable_event_loop(bool new_setting);

#endif //EZGL_APPLICATION_HPP
