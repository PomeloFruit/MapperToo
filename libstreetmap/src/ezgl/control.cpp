#include "ezgl/control.hpp"

#include "ezgl/camera.hpp"
#include "ezgl/canvas.hpp"
#include <iostream>

namespace ezgl {

static rectangle zoom_in_world(point2d zoom_point, rectangle world, double zoom_factor)
{
  double const left = zoom_point.x - (zoom_point.x - world.left()) / zoom_factor;
  double const bottom = zoom_point.y + (world.bottom() - zoom_point.y) / zoom_factor;

  double const right = zoom_point.x + (world.right() - zoom_point.x) / zoom_factor;
  double const top = zoom_point.y - (zoom_point.y - world.top()) / zoom_factor;

  return {{left, bottom}, {right, top}};
}

static rectangle zoom_out_world(point2d zoom_point, rectangle world, double zoom_factor)
{
  double const left = zoom_point.x - (zoom_point.x - world.left()) * zoom_factor;
  double const bottom = zoom_point.y + (world.bottom() - zoom_point.y) * zoom_factor;

  double const right = zoom_point.x + (world.right() - zoom_point.x) * zoom_factor;
  double const top = zoom_point.y - (zoom_point.y - world.top()) * zoom_factor;

  return {{left, bottom}, {right, top}};
}

void zoom_in(canvas *cnv, double zoom_factor)
{
  point2d const zoom_point = cnv->get_camera().get_world().center();
  rectangle const world = cnv->get_camera().get_world();

  cnv->get_camera().set_world(zoom_in_world(zoom_point, world, zoom_factor));
  cnv->redraw();
}

void zoom_in(canvas *cnv, point2d zoom_point, double zoom_factor)
{
  zoom_point = cnv->get_camera().widget_to_world(zoom_point);
  rectangle const world = cnv->get_camera().get_world();

  cnv->get_camera().set_world(zoom_in_world(zoom_point, world, zoom_factor));
  cnv->redraw();
}

void zoom_location(canvas *cnv, point2d zoom_point){
    double currentH = cnv->get_camera().get_world().height();
    double currentW = cnv->get_camera().get_world().width();
    double ratioHW = currentH/currentW; //x-y ratio
    double worldW = cnv->get_camera().get_initial_world().width();  
    double zoomAreaW = (0.005*worldW)/0.57;
    
    zoom_point.x = zoom_point.x-(zoomAreaW/2);
    zoom_point.y = zoom_point.y-(zoomAreaW*ratioHW/2);
    
    rectangle zoomArea(zoom_point, zoomAreaW, zoomAreaW*ratioHW);
    
    cnv->get_camera().set_world(zoomArea);
    cnv->redraw();
}

void zoom_out(canvas *cnv, double zoom_factor)
{
  point2d const zoom_point = cnv->get_camera().get_world().center();
  rectangle const world = cnv->get_camera().get_world();

  cnv->get_camera().set_world(zoom_out_world(zoom_point, world, zoom_factor));
  cnv->redraw();
}

void zoom_out(canvas *cnv, point2d zoom_point, double zoom_factor)
{
  zoom_point = cnv->get_camera().widget_to_world(zoom_point);
  rectangle const world = cnv->get_camera().get_world();

  cnv->get_camera().set_world(zoom_out_world(zoom_point, world, zoom_factor));
  cnv->redraw();
}

void zoom_fit(canvas *cnv, rectangle region)
{
  cnv->get_camera().set_world(region);
  cnv->redraw();
}

void translate(canvas *cnv, double dx, double dy)
{
  rectangle new_world = cnv->get_camera().get_world();
  new_world += ezgl::point2d(dx, dy);

  cnv->get_camera().set_world(new_world);
  cnv->redraw();
}

void translate_up(canvas *cnv, double translate_factor)
{
  rectangle new_world = cnv->get_camera().get_world();
  double dy = new_world.height() / translate_factor;

  translate(cnv, 0.0, dy);
}

void translate_down(canvas *cnv, double translate_factor)
{
  rectangle new_world = cnv->get_camera().get_world();
  double dy = new_world.height() / translate_factor;

  translate(cnv, 0.0, -dy);
}

void translate_left(canvas *cnv, double translate_factor)
{
  rectangle new_world = cnv->get_camera().get_world();
  double dx = new_world.width() / translate_factor;

  translate(cnv, -dx, 0.0);
}

void translate_right(canvas *cnv, double translate_factor)
{
  rectangle new_world = cnv->get_camera().get_world();
  double dx = new_world.width() / translate_factor;

  translate(cnv, dx, 0.0);
}
}
