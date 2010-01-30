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

#include "layout_port.h"

/*
 * Public methods
 */

LayoutPort::LayoutPort():
  m_pPort(NULL)
{
}

LayoutPort::~LayoutPort()
{
  printf("LayoutPort::~LayoutPort()\n");
  removed.emit(m_edge, m_position, this);
}

void LayoutPort::associateVHDLPort(INamedItem *pPort)
{
  g_assert(m_pPort == NULL);
  m_pPort = pPort;
}

INamedItem *LayoutPort::getAssociatedVHDLPort()
{
  g_assert(m_pPort != NULL);
  return m_pPort;
}

Glib::ustring LayoutPort::getName()
{
  return m_pPort->getName();
}

/*
 * Protected methods
 */

void LayoutPort::setLocation(Edge edge, int position)
{
  if( (m_edge != edge) || (m_position != position) )
  {
    m_edge = edge;
    m_position = position;
    moved.emit(edge, position, this);
  }
}
