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

#include "clutter_wire.h"

/*
 * Public methods
 */

Glib::RefPtr<ClutterWire> ClutterWire::create(const Clutter::Color &color)
{
  return Glib::RefPtr<ClutterWire>(new ClutterWire(color));
}

void ClutterWire::setCorners(int numPoints, float *pCoords)
{
  if(m_pCornerCoords)
  {
    g_free(m_pCornerCoords);
  }
  m_pCornerCoords = pCoords;
  m_numPoints = numPoints;
}

/*
 * Private methods
 */

ClutterWire::ClutterWire(const Clutter::Color &color):
  m_color(color),
  m_pCornerCoords(NULL)
{
}

ClutterWire::~ClutterWire()
{
  if(m_pCornerCoords)
  {
    g_free(m_pCornerCoords);
    m_pCornerCoords = NULL;
  }
}

void ClutterWire::drawLine(const Clutter::Color &color)
{
  cogl_path_polyline(m_pCornerCoords, m_numPoints);
  cogl_set_source_color4ub(color.get_red(), color.get_green(), color.get_blue(), get_opacity());
  cogl_path_stroke();
}

void ClutterWire::paint_vfunc()
{
  drawLine(m_color);
}

void ClutterWire::pick_vfunc(const Clutter::Color &color)
{
  drawLine(color);
}
