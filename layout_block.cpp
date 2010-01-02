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

#include "layout_block.h"

void LayoutBlock::associateInstance(VHDLInstance *pInstance)
{
  g_assert(m_pInstance == NULL);
  m_pInstance = pInstance;
}

void LayoutBlock::addPort(Edge edge, int position, Glib::ustring name)
{
  Port port;

  port.name = name;
  port.position = position;

  m_ports[edge].push_back(port);
}