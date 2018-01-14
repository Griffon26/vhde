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

#include <iostream>
#include "gui_common.h"
#include "move_and_resize_handler.h"

MoveAndResizeHandler::MoveAndResizeHandler(GuiSelection *pGuiSelection, Glib::RefPtr<Clutter::Stage> pStage):
  m_pGuiSelection(pGuiSelection),
  m_pStage(pStage),
  m_currentOperation(Operation::NONE)
{
}

void MoveAndResizeHandler::onDragStarted(Clutter::ButtonEvent *pButtonEvent, GuiDraggable *pGuiDraggable)
{
  unsigned int modifiers = pButtonEvent->modifier_state & ALL_MODIFIERS_MASK;

  std::cout << "MoveAndResizeHandler::onDragStarted\n";

  if(m_currentOperation == Operation::NONE)
  {
    if((pButtonEvent->button == 3) && (modifiers == CLUTTER_CONTROL_MASK))
    {
      m_currentOperation = Operation::RESIZE;
    }
    else if((pButtonEvent->button == 1) && ( (modifiers == CLUTTER_CONTROL_MASK) || (modifiers == 0) ))
    {
      m_currentOperation = Operation::MOVE;
    }

    m_pStage->transform_stage_point(pButtonEvent->x, pButtonEvent->y, m_initialHandleX, m_initialHandleY);

    switch(m_currentOperation)
    {
    case Operation::RESIZE:
      pGuiDraggable->startResize();
      break;
    case Operation::MOVE:
      pGuiDraggable->startMove();
      break;
    default:
      break;
    }

    if(m_currentOperation != Operation::NONE)
    {
      m_onDragConnection = m_pStage->signal_captured_event().connect(
          sigc::bind<GuiDraggable *>(sigc::mem_fun(this, &MoveAndResizeHandler::onStageEvent), pGuiDraggable));
    }
  }
}

bool MoveAndResizeHandler::onStageEvent(Clutter::Event *pEvent, GuiDraggable *pGuiDraggable)
{
  std::cout << "MoveAndResizeHandler::onStageEvent\n";

  if(pEvent->type == CLUTTER_MOTION)
  {
    m_draggedSinceButtonPress = true;

    float newX, newY;
    m_pStage->transform_stage_point(pEvent->motion.x, pEvent->motion.y, newX, newY);
    int offsetX = static_cast<int>(newX - m_initialHandleX);
    int offsetY = newY - m_initialHandleY;

    switch(m_currentOperation)
    {
    case Operation::RESIZE:
      pGuiDraggable->updateResize(offsetX, offsetY);
      break;
    case Operation::MOVE:
      pGuiDraggable->updateMove(offsetX, offsetY);
      break;
    default:
      g_assert(false);
    }
    return HANDLED;
  }
  else if(pEvent->type == CLUTTER_BUTTON_RELEASE)
  {
    m_onDragConnection.disconnect();
    switch(m_currentOperation)
    {
    case Operation::RESIZE:
      pGuiDraggable->finishResize();
      break;
    case Operation::MOVE:
      pGuiDraggable->finishMove();
      break;
    default:
      g_assert(false);
    }
    m_currentOperation = Operation::NONE;

    if(pEvent->button.button == 1 && !m_draggedSinceButtonPress)
    {
      pGuiDraggable->click(pEvent->button.modifier_state & ALL_MODIFIERS_MASK);
    }
    return HANDLED;
  }
  else
  {
    return UNHANDLED;
  }
}

void MoveAndResizeHandler::addDraggable(GuiDraggable *pGuiDraggable)
{
  pGuiDraggable->dragStarted.connect(sigc::mem_fun(*this, &MoveAndResizeHandler::onDragStarted));
  //pGuiDraggable->dragFinished.connect(sigc::mem_fun(*this, &MoveAndResizeHandler::onDragFinished));
  //pGuiDraggable->dragMotion.connect(sigc::mem_fun(*this, &MoveAndResizeHandler::onDragMotion));
}

