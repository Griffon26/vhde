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

#ifndef _GUI_BLOCK_H
#define _GUI_BLOCK_H

#include "layout_block.h"
#include "clutter_block.h"
#include "clutter_port.h"

class GuiBlock
{
private:
  typedef struct
  {
    Edge edge;
    int position;
    Glib::RefPtr<ClutterPort> pActor;

    sigc::connection onPortButtonPressConnection;
    sigc::connection onPortMovedConnection;
    sigc::connection onPortRemovedConnection;
  } PortData;

  std::list<PortData *>   m_portDataList;

  Edge  m_dragFromEdge;
  int   m_dragFromPosition;


  sigc::connection        m_onPortAddedConnection;

protected:
  LayoutBlock            *m_pLayoutBlock;

  sigc::connection        m_onDragConnection;

  Glib::RefPtr<Clutter::Stage>      m_pStage;
  Glib::RefPtr<Clutter::Group>      m_pGroup;
  Glib::RefPtr<ClutterBlock>        m_pBody;

public:
  GuiBlock(Glib::RefPtr<Clutter::Stage> pStage, LayoutBlock *pLayoutBlock);
  virtual ~GuiBlock();

private:
  bool addPort(Edge edge, int position, LayoutPort *pLayoutPort);
  bool getClosestSlot(bool unusedOnly, int x, int y, Edge *pEdge, int *pPosition,
                      bool considerAdditionalSlot = false, Edge additionalSlotEdge = EDGE_LEFT, int additionalSlotPosition = 0);

  bool onPortDragged(Clutter::Event *pEvent, PortData *pPortData);
  void onPortAdded(Edge edge, int position, LayoutPort *pLayoutPort);

  bool onPortButtonPress(Clutter::ButtonEvent *pEvent, PortData *pPortData);
  void onPortMoved(Edge edge, int position, PortData *pPortData);
  void onPortRemoved(PortData *pPortData);

  ClutterPort::Orientation  edgeToOrientation(Edge edge);
  ClutterPort::Type         vhdlDirectionToGuiType(VHDLPort::Direction dir);
};

#endif /* _GUI_BLOCK_H */
