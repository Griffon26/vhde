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

#include "layout_block.h"
#include "layout_port.h"

/*
 * Public methods
 */

void LayoutBlock::init_addPort(Edge edge, int position, LayoutPort *pLayoutPort)
{
  g_assert(m_init);
  g_assert(m_ports[edge].find(position) == m_ports[edge].end());
  pLayoutPort->setLocation(edge, position);
  m_ports[edge][position] = pLayoutPort;
}

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

void LayoutBlock::movePort(Edge oldEdge, int oldPosition, Edge newEdge, int newPosition)
{
  //printf("LayoutBlock::movePort (%s,%d) -> (%s,%d)\n", EDGE_TO_NAME(oldEdge), oldPosition, EDGE_TO_NAME(newEdge), newPosition);

  if(oldEdge != newEdge || oldPosition != newPosition)
  {
    g_assert(m_ports[oldEdge].find(oldPosition) != m_ports[oldEdge].end());
    g_assert(m_ports[newEdge].find(newPosition) == m_ports[newEdge].end());

    m_ports[newEdge][newPosition] = m_ports[oldEdge][oldPosition];
    m_ports[oldEdge].erase(oldPosition);

    m_ports[newEdge][newPosition]->setLocation(newEdge, newPosition);
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

const LayoutBlock::PortPositionMap *LayoutBlock::getPortPositionMaps()
{
  return m_ports;
}

void LayoutBlock::setPortPositionMaps(PortPositionMap *portPositionMaps)
{
  PortPositionMap::iterator mapIt;

  std::map<LayoutPort *, std::pair<Edge, int> > portToOldPositionMap;
  std::map<LayoutPort *, std::pair<Edge, int> >::iterator oldIt;

  /* First build up a map from port pointers to their old edge and position. */
  for(int edge = 0; edge < NR_OF_EDGES; edge++)
  {
    for(mapIt = m_ports[edge].begin(); mapIt != m_ports[edge].end(); mapIt++)
    {
      portToOldPositionMap[mapIt->second] = std::pair<Edge, int>((Edge)edge, mapIt->first);
    }
  }

  /* then record all new positions */
  for(int edge = 0; edge < NR_OF_EDGES; edge++)
  {
    m_ports[edge] = portPositionMaps[edge];
  }

  /* then look for any moved ports */
  for(int edge = 0; edge < NR_OF_EDGES; edge++)
  {
    for(mapIt = portPositionMaps[edge].begin(); mapIt != portPositionMaps[edge].end(); mapIt++)
    {
      oldIt = portToOldPositionMap.find(mapIt->second);

      /* Precondition 1: everything in the new portPositionMaps must be in the old ones as well */
      g_assert(oldIt != portToOldPositionMap.end());

      /* If the port has moved wrt its old position, emit a signal */
      if( (oldIt->second.first != edge) ||
          (oldIt->second.second != mapIt->first) )
      {
        mapIt->second->setLocation((Edge)edge, mapIt->first);
      }

      /* Remove the port from the map of old ports so we can check at the end
       * if we've had them all
       */
      portToOldPositionMap.erase(oldIt);
    }
  }

  /* Precondition 2: everything in the old portPositionMaps must be in the new ones as well */
  g_assert(portToOldPositionMap.size() == 0);
}

LayoutPort *LayoutBlock::findPortByName(Glib::ustring name, Edge *pEdge, int *pPosition)
{
  int edge;
  bool found = false;
  PortPositionMap::iterator it;
  LayoutPort *pLayoutPort;

  for(edge = 0; !found && edge < NR_OF_EDGES; edge++)
  {
    for(it = m_ports[edge].begin(); !found && it != m_ports[edge].end(); it++)
    {
      if(it->second->getName() == name)
      {
        *pEdge = (Edge)edge;
        *pPosition = it->first;
        pLayoutPort = it->second;
        found = true;
      }
    }
  }
  return found ? pLayoutPort : NULL;
}

bool LayoutBlock::findFreeSlotOnEdge(Edge edge, int preferredPosition, int *pFreePosition)
{
  int i, bestFreePosition, bestDistance;
  int maxNrOfPorts;

  maxNrOfPorts = calculateMaxNrOfPorts( (edge == EDGE_LEFT || edge == EDGE_RIGHT) ? m_size.height : m_size.width );

  bestDistance = -1;
  for(i = 0; i < maxNrOfPorts; i++)
  {
    /* Only consider free slots */
    if(m_ports[edge].find(i) == m_ports[edge].end())
    {
      /* Find the closest one */
      if( (bestDistance == -1) ||
          (abs(i - preferredPosition) < bestDistance) )
      {
        bestFreePosition = i;
        bestDistance = abs(i - preferredPosition);
      }
    }
  }

  if(bestDistance == -1)
  {
    return false;
  }
  else
  {
    *pFreePosition = bestFreePosition;
    return true;
  }
}

bool LayoutBlock::findFreeSlot(Edge preferredEdge, int preferredPosition, Edge *pFreeEdge, int *pFreePosition)
{
  bool found = false;
  int edge;

  /* Try to find something near the preferred position on the preferred edge */
  if(findFreeSlotOnEdge(preferredEdge, preferredPosition, pFreePosition))
  {
    *pFreeEdge = preferredEdge;
    found = true;
  }
  else
  {
    /* Otherwise find something near the beginning of the other edges */
    for(edge = 0; !found && edge < NR_OF_EDGES; edge++)
    {
      /* Skip the preferred edge, because we already checked it */
      if(edge == preferredEdge)
      {
        continue;
      }
      if(findFreeSlotOnEdge((Edge)edge, 0, pFreePosition))
      {
        *pFreeEdge = (Edge)edge;
        found = true;
      }
    }
  }
  printf("LayoutBlock(%p)::findFreeSlot() -> %s %s %d\n", this, found ? "true" : "false", EDGE_TO_NAME(*pFreeEdge), *pFreePosition);
  return found;
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

int LayoutBlock::calculateMinEdgeLength(int nrOfPorts)
{
  return nrOfPorts * (LayoutPort::SPACING + LayoutPort::WIDTH) + LayoutPort::SPACING;
}

/*
 * Protected methods
 */

LayoutBlock::LayoutBlock():
  m_init(true)
{
}

void LayoutBlock::addPort(Edge edge, int position, LayoutPort *pPort)
{
  printf("LayoutBlock(%p)::addPort(%s %d) -> layoutPort = %p\n", this, EDGE_TO_NAME(edge), position, pPort);
  g_assert(m_ports[edge].find(position) == m_ports[edge].end());
  m_ports[edge][position] = pPort;
  pPort->setLocation(edge, position);

  port_added.emit(edge, position, pPort);
}

void LayoutBlock::removePort(LayoutPort *pPort)
{
  int edge;
  bool found = false;
  PortPositionMap::iterator it;

  printf("LayoutBlock(%p)::removePort(%p)\n", this, pPort);

  for(edge = 0; !found && edge < NR_OF_EDGES; edge++)
  {
    for(it = m_ports[edge].begin(); !found && it != m_ports[edge].end(); it++)
    {
      if(it->second == pPort)
      {
        fprintf(stderr, "found at edge %s pos %d (map = %p, it = (%d,%p))\n", EDGE_TO_NAME(edge), it->first, &m_ports[edge], it->first, it->second);
        m_ports[edge].erase(it);
        found = true;
      }
    }
  }
  g_assert(found);
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
