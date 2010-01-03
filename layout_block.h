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

#ifndef _LAYOUT_BLOCK_H
#define _LAYOUT_BLOCK_H

#include <glibmm.h>

#include "layout_types.h"
#include "vhdl_instance.h"

class LayoutBlock
{
public:
  typedef enum {
    EDGE_LEFT,
    EDGE_TOP,
    EDGE_RIGHT,
    EDGE_BOTTOM,
    NR_OF_EDGES
  } Edge;

  typedef struct
  {
    Glib::ustring       name;
    int                 position;
  } Port;

private:
  VHDLInstance           *m_pInstance;

  LayoutPosition          m_position;
  LayoutSize              m_size;
  std::list<Port>         m_ports[NR_OF_EDGES];

public:
  LayoutBlock();

  void associateInstance(VHDLInstance *pInstance);
  VHDLInstance *getAssociatedInstance();

  void addPort(Edge edge, int position, Glib::ustring name);
};

#endif /* _LAYOUT_BLOCK_H */
