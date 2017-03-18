/* vim: ts=2:sw=2:expandtab
 *
 * VHDE - The VHDL Diagram Editor
 * Copyright (C) 2009-2010  Maurice van der Pot <griffon26@kfk4ever.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "clutter_port.h"

/*
 * Public methods
 */

Glib::RefPtr<ClutterPort> ClutterPort::create(const Clutter::Color &color)
{
  return Glib::RefPtr<ClutterPort>(new ClutterPort(color));
}

void ClutterPort::setOrientation(Orientation orientation)
{
  m_orientation = orientation;
  queue_redraw();
}

void ClutterPort::setDirection(Direction direction)
{
  m_direction = direction;
  /* queue_redraw() */
}

/*
 * Private methods
 */

ClutterPort::ClutterPort(const Clutter::Color &color):
  m_color(color),
  m_orientation(NORTH),
  m_direction(DIR_IN)
{
}

void ClutterPort::drawTriangle(const Clutter::Color &color, bool hasBorder)
{
  float width, height;

  int angle = 90 * m_orientation + ((m_direction == DIR_OUT) ? 180 : 0);

  get_size(width, height);

  cogl_translate(width / 2, height / 2, 0);
  cogl_rotate(angle, 0, 0, 1);

  float triangleCoords[] = {-width/2, -height/2, width/2, 0, -width/2, height/2};
  float diamondCoords[] = {-width/2, 0, 0, -height/2, width/2, 0, 0, height/2};

  if(m_direction == DIR_INOUT)
  {
    cogl_path_polygon(diamondCoords, 4);
  }
  else
  {
    cogl_path_polygon(triangleCoords, 3);
  }

  cogl_set_source_color4ub(color.get_red(), color.get_green(), color.get_blue(), get_opacity());

  if(hasBorder)
  {
    cogl_path_fill_preserve();
    cogl_set_source_color4ub(0x00, 0x00, 0x00, get_opacity());
    cogl_path_stroke();
  }
  else
  {
    cogl_path_fill();
  }
}

void ClutterPort::paint_vfunc()
{
  drawTriangle(m_color, true);
}

void ClutterPort::pick_vfunc(const Clutter::Color &color)
{
  drawTriangle(color, false);
}
