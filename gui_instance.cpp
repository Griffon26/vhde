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

#include "gui_common.h"
#include "gui_instance.h"

GuiInstance::GuiInstance(Glib::RefPtr<Clutter::Stage> pStage, LayoutInstance *pLayoutInstance):
  GuiBlock(pStage, pLayoutInstance)
{
  m_onBodyButtonPressConnection = m_pBody->signal_button_press_event().connect(sigc::mem_fun(*this, &GuiInstance::onBodyButtonPress));
}

bool GuiInstance::onBodyButtonPress(Clutter::ButtonEvent *pEvent)
{
  // Remember the point within the object where it was picked up
  float actorX, actorY, handleX, handleY;
  m_pGroup->get_position(actorX, actorY);

  m_pGroup->get_stage()->transform_stage_point(pEvent->x, pEvent->y, handleX, handleY);

  m_bodyHandleOffsetX = handleX - actorX;
  m_bodyHandleOffsetY = handleY - actorY;

  // Register for motion and button release events from the stage
  m_onDragConnection = m_pStage->signal_captured_event().connect(sigc::mem_fun(this, &GuiInstance::onBodyDragged));

  return HANDLED;
}

bool GuiInstance::onBodyDragged(Clutter::Event *pEvent)
{
  float handleX, handleY;
  if(pEvent->type == CLUTTER_MOTION)
  {
    LayoutPosition pos;

    m_pGroup->get_stage()->transform_stage_point(pEvent->motion.x, pEvent->motion.y, handleX, handleY);

    pos.x = handleX - m_bodyHandleOffsetX;
    pos.y = handleY - m_bodyHandleOffsetY;

    m_pGroup->set_position(pos.x, pos.y);
    static_cast<LayoutInstance *>(m_pLayoutBlock)->setPosition(pos);
    return HANDLED;
  }
  else if(pEvent->type == CLUTTER_BUTTON_RELEASE)
  {
    m_onDragConnection.disconnect();
    return HANDLED;
  }
  else
  {
    return UNHANDLED;
  }
}
