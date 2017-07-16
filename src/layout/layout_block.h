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

#ifndef _LAYOUT_BLOCK_H
#define _LAYOUT_BLOCK_H

#include <glibmm.h>

#include "layout_port.h"
#include "layout_types.h"

class LayoutBlock
{
public:
  typedef struct
  {
    Edge edge;
    int position;
    LayoutPort *pLayoutPort;
  } PortData;

  typedef std::map<int, std::unique_ptr<LayoutPort>> PortPositionMap;

protected:
  bool                        m_init;
  LayoutPosition              m_position;
  LayoutSize                  m_size;
  PortPositionMap             m_ports[NR_OF_EDGES];

  /* This vector holds ports in the order they were added to the block and is
   * only used to remember this order, regardless of where ports are physically
   * located. The purpose of this is to minimize the diff between .layout files
   * when ports are renamed or moved.
   */
  std::vector<LayoutPort *>   m_portOrder;

public:
  /* Signals */
  sigc::signal<void, const LayoutSize &>       resized;
  sigc::signal<void, Edge, int, LayoutPort *>  port_added;

  /* This method assumes ownership of the port */
  void init_addPort(Edge edge, int position, std::unique_ptr<LayoutPort> pPort);
  void init_done();

  void getPosition(LayoutPosition *pLayoutPosition);

  void setSize(const LayoutSize &size);
  void getSize(LayoutSize *pLayoutSize);
  void getMinimumSize(LayoutSize *pLayoutSize);

  void movePort(Edge oldEdge, int oldPosition, Edge newEdge, int newPosition);
  LayoutPort *getPort(Edge edge, int position);

  std::vector<PortData> getPortList();

  const std::vector<int> getPortPositions(Edge edge);
  void setPortPositions(Edge edge, const std::vector<int> &portPositions);

  LayoutPort *findPortByName(const Glib::ustring &name, Edge *pEdge, int *pPosition);

  bool findFreeSlotOnEdge(Edge edge, int preferredPosition, int *pFreePosition);
  bool findFreeSlot(Edge preferredEdge, int preferredPosition, Edge *pFreeEdge, int *pFreePosition);
  void calculatePortPosition(Edge edge, int position, int *pX, int *pY) const;
  static int calculateMaxNrOfPorts(int edgeLength);
  static int calculateMinEdgeLength(int nrOfPorts);

protected:
  LayoutBlock();
  LayoutBlock(const LayoutBlock&) = delete;
  LayoutBlock& operator=(const LayoutBlock&) = delete;

  void addPort(Edge edge, int position, std::unique_ptr<LayoutPort> pPort);
  void removePort(LayoutPort *pPort);
};

#endif /* _LAYOUT_BLOCK_H */
