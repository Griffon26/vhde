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

#ifndef _GUI_INSTANCE_H
#define _GUI_INSTANCE_H

#include "gui_block.h"
#include "layout_instance.h"

/**
 * A class that manages the GUI of a VHDL instance.
 *
 * GuiInstance extends GuiBlock with operations that are specific to component
 * instances, such as changing the position on the canvas.
 */
class GuiInstance: public GuiBlock
{
private:
  LayoutPosition          m_initialPosition;
  bool                    m_dragIsMove;

  /* Model signals */
  sigc::connection        m_onLayoutPortAddedConnection;
  std::map<LayoutPort *, sigc::connection> m_onLayoutPortRemovedConnections;
public:
  GuiInstance(Glib::RefPtr<Clutter::Stage> pStage, LayoutInstance *pLayoutInstance);
  virtual ~GuiInstance();

  GuiInstance(const GuiInstance&) = delete;
  GuiInstance& operator=(const GuiInstance&) = delete;

  /* Inherited from GuiSelectable */
  void discard();

  /* Inherited from GuiDraggable */
  virtual void startMove() override;
  virtual void updateMove(int offsetx, int offsety) override;
  virtual void finishMove() override;

private:
  void handlePortAdded();

  void onLayoutPortAdded(Edge edge, int position, LayoutPort *pLayoutPort);
  void onLayoutPortRemoved(Edge edge, int position, LayoutPort *pLayoutPort);
};

#endif

