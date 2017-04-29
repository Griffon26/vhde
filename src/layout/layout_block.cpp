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

#include <stdio.h>
#include <iostream>

#include "common.h"
#include "layout_block.h"
#include "layout_port.h"

/*
 * Public methods
 */

void LayoutBlock::init_addPort(Edge edge, int position, LayoutPort *pLayoutPort)
{
  g_assert(m_init);
  g_assert(m_ports[edge].find(position) == m_ports[edge].end());
  printf("LayoutBlock(%p)::init_addPort(%s, %d, %p)\n", this, EDGE_TO_NAME(edge), position, pLayoutPort);
  pLayoutPort->setLocation(edge, position);
  m_ports[edge][position] = pLayoutPort;
  m_portOrder.push_back(pLayoutPort);
}

void LayoutBlock::init_done()
{
  g_assert(m_init);
  g_assert(m_size.width != 0);
  g_assert(m_size.height != 0);
  m_init = false;
}

void LayoutBlock::getPosition(LayoutPosition *pLayoutPosition)
{
  *pLayoutPosition = m_position;
}

void LayoutBlock::setSize(const LayoutSize &size)
{
  //printf("LayoutBlock::setSize, max ports in x: %d, max ports in y: %d\n", calculateMaxNrOfPorts(size.width), calculateMaxNrOfPorts(size.height));

  std::vector<int> originalPortPositions[NR_OF_EDGES];
  for(int edge = 0; edge < NR_OF_EDGES; edge++)
  {
    originalPortPositions[edge] = getPortPositions((Edge)edge);
  }

  m_size = size;

  for(int edge = 0; edge < NR_OF_EDGES; edge++)
  {
    setPortPositions((Edge)edge, originalPortPositions[edge]);
  }

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

const std::vector<int> LayoutBlock::getPortPositions(Edge edge)
{
  return getKeysFromMap(m_ports[edge]);
}

void LayoutBlock::setPortPositions(Edge edge, const std::vector<int> &newPortPositions)
{
  g_assert(newPortPositions.size() == m_ports[edge].size());

  int maxNrOfPorts = calculateMaxNrOfPorts( (edge == EDGE_LEFT || edge == EDGE_RIGHT) ? m_size.height : m_size.width );
  g_assert(newPortPositions.size() <= (unsigned int)maxNrOfPorts);
  int lastFreePosition = maxNrOfPorts - 1;

  PortPositionMap newMap;
  auto oldPortPositions = getKeysFromMap(m_ports[edge]);

  /* Condense the new positions to fit on the edge */
  for(int posIndex = newPortPositions.size() - 1; posIndex >= 0; posIndex--)
  {
    int newPosition = newPortPositions[posIndex];
    if(newPosition > lastFreePosition)
    {
      newPosition = lastFreePosition--;
    }

    newMap[newPosition] = m_ports[edge][oldPortPositions[posIndex]];
    newMap[newPosition]->setLocation(edge, newPosition);
  }

  m_ports[edge] = newMap;
}

LayoutPort *LayoutBlock::findPortByName(const Glib::ustring &name, Edge *pEdge, int *pPosition)
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
  int i, bestFreePosition = -1;
  int bestDistance;
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
  printf("LayoutBlock(%p)::findFreeSlot() -> %s %s %d\n", this, found ? "true" : "false",
                                                          found ? EDGE_TO_NAME(*pFreeEdge) : "n.a.",
                                                          found ? *pFreePosition : -1);
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
  m_portOrder.push_back(pPort);
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
  auto foundIt = std::find(std::begin(m_portOrder),
                           std::end(m_portOrder),
                           pPort);
  g_assert(foundIt != std::end(m_portOrder));
  m_portOrder.erase(foundIt);

  g_assert(found);
}

