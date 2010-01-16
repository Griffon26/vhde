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
}

bool GuiInstance::onBodyButtonPress(Clutter::ButtonEvent *pEvent)
{
  bool ret;

  ret = GuiBlock::onBodyButtonPress(pEvent);
  g_assert(ret == HANDLED);

  /* Determine if this button press is the start of a move operation */
  //printf("instance onBodyButtonPress button = %d modstate = %d\n", pEvent->button, pEvent->modifier_state);
  m_dragIsMove = (pEvent->button == 1) ||
                 (pEvent->button == 1 && (pEvent->modifier_state == CLUTTER_CONTROL_MASK));

  return HANDLED;
}

bool GuiInstance::onBodyDragged(Clutter::Event *pEvent)
{
  float handleX, handleY;

  if((pEvent->type == CLUTTER_MOTION) && m_dragIsMove)
  {
    LayoutPosition pos;

    m_pStage->transform_stage_point(pEvent->motion.x, pEvent->motion.y, handleX, handleY);

    pos.x = handleX - m_bodyHandleOffsetX;
    pos.y = handleY - m_bodyHandleOffsetY;

    m_pGroup->set_position(pos.x, pos.y);
    static_cast<LayoutInstance *>(m_pLayoutBlock)->setPosition(pos);
    return HANDLED;
  }
  else
  {
    return GuiBlock::onBodyDragged(pEvent);
  }
}
