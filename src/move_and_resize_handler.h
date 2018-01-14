/* vim: ts=2:sw=2:expandtab
 *
 * VHDE - The VHDL Diagram Editor
 * Copyright (C) 2009-2017  Maurice van der Pot <griffon26@kfk4ever.com>
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

#ifndef _MOVE_AND_RESIZE_HANDLER_H
#define _MOVE_AND_RESIZE_HANDLER_H

#include "gui_draggable.h"
#include "gui_selection.h"

class MoveAndResizeHandler
{
public:
  MoveAndResizeHandler(GuiSelection *pGuiSelection, Glib::RefPtr<Clutter::Stage> pStage);
  void addDraggable(GuiDraggable *pGuiDraggable);

private:
  void onDragStarted(Clutter::ButtonEvent *pButtonEvent, GuiDraggable *pGuiDraggable);
  bool onStageEvent(Clutter::Event *pEvent, GuiDraggable *pGuiDraggable);

private:
  enum class Operation
  {
    NONE,
    MOVE,
    RESIZE
  };

  GuiSelection *m_pGuiSelection;
  Glib::RefPtr<Clutter::Stage> m_pStage;
  Operation m_currentOperation;

  float m_initialHandleX;
  float m_initialHandleY;
  bool m_draggedSinceButtonPress;

  sigc::connection m_onDragConnection;
};

#endif

