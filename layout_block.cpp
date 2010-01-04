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
#include "layout_port.h"

/*
 * Public methods
 */

void LayoutBlock::setSize(const LayoutSize &size)
{
  m_size = size;
  resized.emit(size);
}

void LayoutBlock::addPort(Edge edge, int position, LayoutPort *pPort)
{
  m_ports[edge][position] = pPort;
}

void LayoutBlock::movePort(Edge oldEdge, int oldPosition, Edge newEdge, int newPosition)
{
  g_assert(m_ports[oldEdge].find(oldPosition) != m_ports[oldEdge].end());

  m_ports[newEdge][newPosition] = m_ports[oldEdge][oldPosition];
  m_ports[oldEdge].erase(oldPosition);

  m_ports[newEdge][newPosition]->moved.emit(newEdge, newPosition);
}

LayoutPort *LayoutBlock::getPort(Edge edge, int position)
{
  g_assert(m_ports[edge].find(position) != m_ports[edge].end());
  return m_ports[edge][position];
}

void LayoutBlock::calculatePortPosition(Edge edge, int position, int *pX, int *pY)
{
  switch(edge)
  {
  case EDGE_LEFT:
    *pX = -LayoutPort::WIDTH / 2;
    break;
  case EDGE_RIGHT:
    *pX = m_size.width + LayoutPort::WIDTH / 2;
    break;
  case EDGE_TOP:
    *pY = -LayoutPort::WIDTH / 2;
    break;
  case EDGE_BOTTOM:
    *pY = m_size.height + LayoutPort::WIDTH / 2;
    break;
  default:
    g_assert_not_reached();
  }

  if(edge == EDGE_LEFT || edge == EDGE_RIGHT)
  {
    *pY = LayoutPort::SPACING + LayoutPort::WIDTH / 2 + position * (LayoutPort::SPACING + LayoutPort::WIDTH);
  }
  else
  {
    *pX = LayoutPort::SPACING + LayoutPort::WIDTH / 2 + position * (LayoutPort::SPACING + LayoutPort::WIDTH);
  }
}

/*
 * Private methods
 */

int LayoutBlock::calculateMaxNrOfPorts(Edge edge)
{
  int edgeLength = (edge == EDGE_LEFT || edge == EDGE_RIGHT) ? m_size.height : m_size.width;
  return (edgeLength - LayoutPort::SPACING) / (LayoutPort::SPACING + LayoutPort::WIDTH);
}
