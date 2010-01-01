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

#include "layout_port.h"
#include "layout_types.h"

class LayoutBlock
{
private:
  Glib::ustring           m_instanceName;
  LayoutPosition          m_position;
  LayoutSize              m_size;
  std::vector<LayoutPort> m_ports;
};

#endif /* _LAYOUT_BLOCK_H */
