/* vim: ts=2:sw=2:expandtab
 *
 * Copyright 2009 Maurice van der Pot <griffon26@kfk4ever.com>
 *
 * This file is part of Foobar.
 *
 * Foobar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _LAYOUT_PORT_H
#define _LAYOUT_PORT_H

#include <glibmm.h>

class LayoutPort
{
public:
  typedef enum {
    EDGE_LEFT,
    EDGE_TOP,
    EDGE_RIGHT,
    EDGE_BOTTOM
  } Edge;

private:
  Glib::ustring       m_portName;
  Edge                m_edge;
  int                 position;
};

#endif /* _LAYOUT_PORT_H */
