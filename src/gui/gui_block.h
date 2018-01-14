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

#ifndef _GUI_BLOCK_H
#define _GUI_BLOCK_H

#include "clutter_block.h"
#include "clutter_port.h"
#include "clutter_scaling_text.h"
#include "gui_port.h"
#include "gui_draggable.h"
#include "layout_block.h"

/**
 * A class that manages the GUI of a VHDL component interface.
 *
 * GuiBlock is meant to be used in a diagram of a component interface as
 * opposed to a diagram with component instances. It is therefore not possible
 * to change its position on the canvas.  Use GuiInstance instead if you need
 * component instance specific operations like that.
 */
class GuiBlock: public GuiDraggable
{
private:
  bool  m_dragIsResize;

  /* GUI signals */
  sigc::connection        m_onBodyButtonPressConnection;
  sigc::connection        m_onTextButtonPressConnection;
  sigc::connection        m_onDragConnection;

  /* Model signals */
  sigc::connection        m_onResizedConnection;

protected:
  bool  m_draggedSinceButtonPress;

  std::vector<std::unique_ptr<GuiPort>> m_ports;

  LayoutBlock            *m_pLayoutBlock;

  Glib::RefPtr<Clutter::Stage>      m_pStage;
  Glib::RefPtr<Clutter::Group>      m_pGroup;
  Glib::RefPtr<ClutterBlock>        m_pBody;
  Glib::RefPtr<ClutterScalingText>  m_pText;

  int                     m_bodyHandleOffsetX;
  int                     m_bodyHandleOffsetY;

  /* Used for dragging */
  float                         m_initialHandleX;
  float                         m_initialHandleY;
  LayoutSize                    m_initialSize;
  LayoutSize                    m_minimumSize;
  std::vector<int>              m_initialPortPositions[NR_OF_EDGES];

public:
  GuiBlock(Glib::RefPtr<Clutter::Stage> pStage, LayoutBlock *pLayoutBlock);
  virtual ~GuiBlock();

  GuiBlock(const GuiBlock&) = delete;
  GuiBlock& operator=(const GuiBlock&) = delete;

  void queueRedraw();

  bool findFreeSlot(Edge preferredEdge, int preferredPosition, Edge *pFreeEdge, int *pFreePosition);

  virtual void startResize() override;
  virtual void updateResize(int offsetx, int offsety) override;
  virtual void finishResize() override;

  virtual void click(int modifiers) override;

private:
  bool getClosestSlot(bool unusedOnly, int x, int y, Edge *pEdge, int *pPosition,
                      bool considerAdditionalSlot = false, Edge additionalSlotEdge = EDGE_LEFT, int additionalSlotPosition = 0);

  bool onPortDragged(Clutter::Event *pEvent, GuiPort *pGuiPort);
  bool onPortButtonPress(Clutter::ButtonEvent *pEvent, GuiPort *pGuiPort);
  bool onTextButtonPress(Clutter::ButtonEvent* pEvent);
  bool onTextDragged(Clutter::Event *pEvent);

  void onResized(LayoutSize layoutSize);

protected:
  void addPort(Edge edge, int position, LayoutPort *pLayoutPort);
  void removePort(Edge edge, int position);

  virtual bool onBodyButtonPress(Clutter::ButtonEvent *pEvent);
  //virtual bool onBodyDragged(Clutter::Event *pEvent);

};

#endif /* _GUI_BLOCK_H */
