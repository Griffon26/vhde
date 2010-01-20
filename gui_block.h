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

#include "clutter_block.h"
#include "clutter_port.h"
#include "gui_port.h"
#include "layout_block.h"

/**
 * A class that manages the GUI of a VHDL component interface.
 *
 * GuiBlock is meant to be used in a diagram of a component interface as
 * opposed to a diagram with component instances. It is therefore not possible
 * to change its position on the canvas.  Use GuiInstance instead if you need
 * component instance specific operations like that.
 */
class GuiBlock
{
private:
  std::list<GuiPort *>   m_portList;

  bool  m_dragIsResize;

  /* GUI signals */
  sigc::connection        m_onBodyButtonPressConnection;
  sigc::connection        m_onDragConnection;

  /* Model signals */
  sigc::connection        m_onResizedConnection;

protected:
  LayoutBlock            *m_pLayoutBlock;

  Glib::RefPtr<Clutter::Stage>      m_pStage;
  Glib::RefPtr<Clutter::Group>      m_pGroup;
  Glib::RefPtr<ClutterBlock>        m_pBody;
  Glib::RefPtr<Clutter::Text>       m_pText;

  int                     m_bodyHandleOffsetX;
  int                     m_bodyHandleOffsetY;

  /* Used for dragging */
  float                         m_initialHandleX;
  float                         m_initialHandleY;
  LayoutSize                    m_initialSize;
  LayoutSize                    m_minimumSize;
  LayoutBlock::PortPositionMap  m_initialPortPositionMaps[NR_OF_EDGES];


public:
  GuiBlock(Glib::RefPtr<Clutter::Stage> pStage, LayoutBlock *pLayoutBlock);
  virtual ~GuiBlock();

  bool findFreeSlot(Edge preferredEdge, int preferredPosition, Edge *pFreeEdge, int *pFreePosition);

private:
  bool getClosestSlot(bool unusedOnly, int x, int y, Edge *pEdge, int *pPosition,
                      bool considerAdditionalSlot = false, Edge additionalSlotEdge = EDGE_LEFT, int additionalSlotPosition = 0);
  void resizeEdge(const LayoutBlock::PortPositionMap &oldPortPositionMap, LayoutBlock::PortPositionMap *pNewPortPositionMap, int newSize);

  bool onPortDragged(Clutter::Event *pEvent, GuiPort *pGuiPort);
  bool onPortButtonPress(Clutter::ButtonEvent *pEvent, GuiPort *pGuiPort);

  void onResized(LayoutSize layoutSize);

protected:
  void addPort(Edge edge, int position, LayoutPort *pLayoutPort);
  void removePort(Edge edge, int position);

  virtual bool onBodyButtonPress(Clutter::ButtonEvent *pEvent);
  virtual bool onBodyDragged(Clutter::Event *pEvent);

};

#endif /* _GUI_BLOCK_H */
