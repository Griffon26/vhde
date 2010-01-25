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
#include "layout_port.h"

class LayoutBlock
{
public:
  typedef struct
  {
    Edge edge;
    int position;
    LayoutPort *pLayoutPort;
  } PortData;

  typedef std::map<int, LayoutPort *> PortPositionMap;

protected:
  bool                        m_init;
  LayoutPosition              m_position;
  LayoutSize                  m_size;
  PortPositionMap             m_ports[NR_OF_EDGES];

public:
  /* Signals */
  sigc::signal<void, const LayoutSize &>            resized;
  sigc::signal<void, int, Edge, int, LayoutPort *>  port_added;
  sigc::signal<void, int, Edge, int, LayoutPort *>  port_removed;

  void init_addPort(Edge edge, int position, LayoutPort *pPort);
  void init_done() { m_init = false; }

  void getPosition(LayoutPosition *pLayoutPosition);

  void setSize(const LayoutSize &size);
  void getSize(LayoutSize *pLayoutSize);
  void getMinimumSize(LayoutSize *pLayoutSize);

  void movePort(Edge oldEdge, int oldPosition, Edge newEdge, int newPosition);
  LayoutPort *getPort(Edge edge, int position);

  std::list<PortData> *getPortList();

  const PortPositionMap *getPortPositionMaps();
  void setPortPositionMaps(PortPositionMap *portPositionMap);

  LayoutPort *findPortByName(Glib::ustring name, Edge *pEdge, int *pPosition);

  bool findFreeSlotOnEdge(Edge edge, int preferredPosition, int *pFreePosition);
  bool findFreeSlot(Edge preferredEdge, int preferredPosition, Edge *pFreeEdge, int *pFreePosition);
  void calculatePortPosition(Edge edge, int position, int *pX, int *pY) const;
  static int calculateMaxNrOfPorts(int edgeLength);
  static int calculateMinEdgeLength(int nrOfPorts);

protected:
  LayoutBlock();

  /* This method assumes ownership of the port */
  void addPort(int actionId, Edge edge, int position, LayoutPort *pPort);
  void removePort(int actionId, LayoutPort *pPort);

private:
  void resizeEdge(Edge edge, int newSize);
};

#endif /* _LAYOUT_BLOCK_H */
