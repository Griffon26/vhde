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

void LayoutBlock::getPosition(LayoutPosition *pLayoutPosition)
{
  *pLayoutPosition = m_position;
}

void LayoutBlock::setSize(const LayoutSize &size)
{
  //printf("LayoutBlock::setSize, max ports in x: %d, max ports in y: %d\n", calculateMaxNrOfPorts(size.width), calculateMaxNrOfPorts(size.height));

  resizeEdge(EDGE_TOP, size.width);
  resizeEdge(EDGE_BOTTOM, size.width);

  resizeEdge(EDGE_LEFT, size.height);
  resizeEdge(EDGE_RIGHT, size.height);

  m_size = size;
  resized.emit(size);
}

void LayoutBlock::getSize(LayoutSize *pLayoutSize)
{
  *pLayoutSize = m_size;
}

void LayoutBlock::getMinimumSize(LayoutSize *pLayoutSize)
{
  pLayoutSize->height = MAX(m_ports[EDGE_LEFT].size(), m_ports[EDGE_RIGHT].size()) * (LayoutPort::SPACING + LayoutPort::WIDTH) + LayoutPort::SPACING;
  pLayoutSize->width  = MAX(m_ports[EDGE_TOP].size(), m_ports[EDGE_BOTTOM].size()) * (LayoutPort::SPACING + LayoutPort::WIDTH) + LayoutPort::SPACING;
}

void LayoutBlock::addPort(Edge edge, int position, LayoutPort *pPort)
{
  g_assert(m_ports[edge].find(position) == m_ports[edge].end());
  m_ports[edge][position] = pPort;

  port_added.emit(edge, position, pPort);
}

void LayoutBlock::movePort(Edge oldEdge, int oldPosition, Edge newEdge, int newPosition)
{
  //printf("LayoutBlock::movePort (%s,%d) -> (%s,%d)\n", EDGE_TO_NAME(oldEdge), oldPosition, EDGE_TO_NAME(newEdge), newPosition);

  if(oldEdge != newEdge || oldPosition != newPosition)
  {
    g_assert(m_ports[oldEdge].find(oldPosition) != m_ports[oldEdge].end());
    g_assert(m_ports[newEdge].find(newPosition) == m_ports[newEdge].end());

    m_ports[newEdge][newPosition] = m_ports[oldEdge][oldPosition];
    m_ports[oldEdge].erase(oldPosition);

    m_ports[newEdge][newPosition]->moved.emit(newEdge, newPosition);
  }
}

LayoutPort *LayoutBlock::getPort(Edge edge, int position)
{
  std::map<int, LayoutPort *>::iterator it;

  it = m_ports[edge].find(position);

  return (it == m_ports[edge].end()) ? NULL : it->second;
}

std::list<LayoutBlock::PortData> *LayoutBlock::getPortList()
{
  PortData portData;
  std::map<int, LayoutPort *>::iterator it;

  std::list<PortData> *pPortList = new std::list<PortData>();

  for(int edge = 0; edge < NR_OF_EDGES; edge++)
  {
    for(it = m_ports[edge].begin(); it != m_ports[edge].end(); it++)
    {
      portData.edge = (Edge)edge;
      portData.position = it->first;
      portData.pLayoutPort = it->second;

      pPortList->push_back(portData);
    }
  }

  return pPortList;
}

void LayoutBlock::calculatePortPosition(Edge edge, int position, int *pX, int *pY) const
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

int LayoutBlock::calculateMaxNrOfPorts(int edgeLength)
{
  return (edgeLength - LayoutPort::SPACING) / (LayoutPort::SPACING + LayoutPort::WIDTH);
}

/*
 * Private methods
 */

void LayoutBlock::resizeEdge(Edge edge, int newSize)
{
  std::map<int, LayoutPort *>::iterator it, prevIt;

  int nrOfUnusedSpotsAllowed = calculateMaxNrOfPorts(newSize) - m_ports[edge].size();
  g_assert(nrOfUnusedSpotsAllowed >= 0);

  int previousUsedSpot = -1;
  int nrOfSkippedSpots;
  int currentSpot;
  for(it = m_ports[edge].begin(); it != m_ports[edge].end(); it++)
  {
    currentSpot = it->first;
    nrOfSkippedSpots = (currentSpot - previousUsedSpot) - 1;
    nrOfUnusedSpotsAllowed -= nrOfSkippedSpots;

    /* Break out of the loop if the current port is at or beyond the furthest
     * position it is allowed to occupy.
     */
    if(nrOfUnusedSpotsAllowed <= 0)
    {
      break;
    }
    previousUsedSpot = currentSpot;
  }

  /* If nrOfUnusedSpotsAllowed is negative, we need to move the current port
   * back by (-nrOfUnusedSpotsAllowed) places and put all following ports
   * adjacent to this one.
   */
  currentSpot -= -nrOfUnusedSpotsAllowed;

  for(;it != m_ports[edge].end();)
  {
    prevIt = it++;
    movePort(edge, prevIt->first, edge, currentSpot);
    currentSpot++;
  }
}
