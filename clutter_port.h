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

#ifndef _CLUTTER_PORT_H
#define _CLUTTER_PORT_H

#include <cluttermm.h>

#include "common.h"

class ClutterPort: public Clutter::Actor
{
public:
  typedef enum {
    EAST,
    SOUTH,
    WEST,
    NORTH
  } Orientation;

private:
  Clutter::Color  m_color;
  Orientation     m_orientation;
  Direction       m_direction;

public:
  static Glib::RefPtr<ClutterPort> create(const Clutter::Color &color);

  void setOrientation(Orientation orientation);
  void setDirection(Direction direction);

private:
  explicit ClutterPort(const Clutter::Color &color);

  void drawTriangle(const Clutter::Color &color, bool hasBorder);

  void paint_vfunc() override;
  void pick_vfunc(const Clutter::Color &color);
};

#endif /* _CLUTTER_PORT_H */
