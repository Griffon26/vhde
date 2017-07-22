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

#include "clutter_block.h"

/*
 * Public methods
 */

Glib::RefPtr<ClutterBlock> ClutterBlock::create(const Clutter::Color &color, bool &selectionState)
{
  return Glib::RefPtr<ClutterBlock>(new ClutterBlock(color, selectionState));
}

/*
 * Private methods
 */

ClutterBlock::ClutterBlock(const Clutter::Color &color, bool &selectionState):
  m_color(color),
  m_rSelectionState(selectionState)
{
}

void ClutterBlock::drawBox(const Clutter::Color &color, bool hasBorder, bool selected)
{
  float width, height;

  get_size(width, height);

  cogl_set_source_color4ub(color.get_red(), color.get_green(), color.get_blue(), get_opacity());
  cogl_rectangle(0, 0, width, height);

  if(hasBorder)
  {
    const int cornerRadius = 3;

    cogl_set_source_color4ub(0x00, 0x00, 0x00, get_opacity());

    std::vector<std::pair<float, float>> handles = { {0,0}, {width / 2, 0}, {width, 0},
                                                     {0, height / 2}, {width, height / 2},
                                                     {0, height}, {width / 2, height}, {width, height} };

    for(auto &point: handles)
    {
      cogl_rectangle(point.first - cornerRadius, point.second - cornerRadius,
                     point.first + cornerRadius, point.second + cornerRadius);
    }

    cogl_path_rectangle(0, 0, width, height);
    cogl_path_stroke();
  }
}

void ClutterBlock::paint_vfunc()
{
  drawBox(m_color, true, m_rSelectionState);
}

void ClutterBlock::pick_vfunc(const Clutter::Color &color)
{
  drawBox(color, false, false);
}
