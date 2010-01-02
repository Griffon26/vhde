/* Copyright 2007 Openismus GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "triangle_actor.h"
#include <cogl/cogl.h>

namespace Tutorial
{

Glib::RefPtr<Triangle> Triangle::create()
{
  return Glib::RefPtr<Triangle>(new Triangle());
}

Glib::RefPtr<Triangle> Triangle::create(const Clutter::Color& color)
{
  return Glib::RefPtr<Triangle>(new Triangle(color));
}

Triangle::Triangle()
:
  color_ (0xFF, 0xFF, 0xFF, 0xFF)
{}

Triangle::Triangle(const Clutter::Color& color)
:
  color_ (color)
{}

Triangle::~Triangle()
{}

void Triangle::do_triangle_paint(const CoglColor* color)
{
  const Clutter::Geometry geom = get_geometry();

  cogl_push_matrix();
  cogl_set_source_color(color);

  float coords[6];

  // Paint a triangle.  The parent paint call will have translated us into
  // position so paint from 0, 0.
  coords[0] = (0);
  coords[1] = (0);

  coords[2] = geom.get_width();
  coords[3] = geom.get_height() / 2.0;

  coords[4] = 0;
  coords[5] = geom.get_height();

  cogl_path_polygon(coords, G_N_ELEMENTS(coords) / 2);
  cogl_path_fill_preserve();
  cogl_set_source_color4ub(0x00, 0x00, 0x00, 0xFF);
  cogl_path_stroke();

  cogl_pop_matrix();
}

void Triangle::on_paint()
{
  CoglColor coglcolor;

  // Paint the triangle with the actor's color:
  cogl_color_set_from_4ub(&coglcolor,
    color_.get_red(),
    color_.get_green(),
    color_.get_blue(),
    get_opacity());

  do_triangle_paint(&coglcolor);
}

void Triangle::pick_vfunc(const Clutter::Color& color)
{
  // Paint the triangle with the pick color, offscreen.
  // This is used by Clutter to detect the actor under the cursor
  // by identifying the unique color under the cursor.
  CoglColor coglcolor;
  cogl_color_set_from_4ub(&coglcolor,
                          color.get_red(),
                          color.get_green(),
                          color.get_blue(),
                          color.get_alpha());
  do_triangle_paint(&coglcolor);
}

/**
 * Tutorial::Triangle::get_color:
 *
 * @returns the color of the triangle.
 */
Clutter::Color Triangle::get_color() const
{
  return color_;
}

/**
 * Tutorial::Triangle::set_color:
 * @color: a Clutter::Color
 *
 * Sets the color of the triangle.
 */
void Triangle::set_color(const Clutter::Color& color)
{
  color_ = color;
  set_opacity(color_.get_alpha());

  if(is_visible())
    queue_redraw();
}

} // namespace Tutorial
