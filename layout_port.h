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

#ifndef _LAYOUT_PORT_H
#define _LAYOUT_PORT_H

#include "i_named_item.h"
#include "layout_types.h"

class LayoutPort
{
private:
  INamedItem *m_pPort;

public:
  static const int WIDTH = 20;
  static const int SPACING = 10;

  /* Signals */
  sigc::signal<void, Edge, int> moved;
  sigc::signal<void> disconnected;

  LayoutPort();

  void associateVHDLPort(INamedItem *pPort);
  INamedItem *getAssociatedVHDLPort();

  Glib::ustring getName();
};

#endif /* _LAYOUT_PORT_H */
