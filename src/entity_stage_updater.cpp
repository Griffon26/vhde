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

#include <glib.h>
#include <iostream>

#include "entity_stage_updater.h"
#include "layout_component.h"

#define FACTOR  1.2

EntityStageUpdater::EntityStageUpdater():
  m_pEntity(nullptr),
  m_pGuiComponent(nullptr)
{
  std::cout << "EntityStageUpdater()::EntityStageUpdater()\n";
}

EntityStageUpdater::~EntityStageUpdater()
{
  std::cout << "EntityStageUpdater()::~EntityStageUpdater()\n";
  m_key_press_connection.disconnect();
  m_capture_connection.disconnect();
}

void EntityStageUpdater::setEntity(LayoutComponent *pEntity)
{
  g_assert(m_pEntity == nullptr);
  m_pEntity = pEntity;
}

void EntityStageUpdater::setStage(Glib::RefPtr<Clutter::Stage> pStage)
{
  g_assert(!m_pStage);
  g_assert(m_pEntity != nullptr);
  m_pStage = pStage;

  m_pGuiComponent = std::make_unique<GuiComponent>(m_pStage, m_pEntity);
  m_capture_connection = m_pStage->signal_captured_event().connect(sigc::bind(sigc::mem_fun(*this, &EntityStageUpdater::on_my_captured_event), m_pStage));
}

enum State
{
  NORMAL,
  WAITING_FOR_EDGE,
  WAITING_FOR_INDEX
};

bool EntityStageUpdater::on_my_captured_event(Clutter::Event* pEvent, Glib::RefPtr<Clutter::Stage> pStage)
{
  if(pEvent->type == CLUTTER_SCROLL)
  {
    if(pEvent->scroll.modifier_state & CLUTTER_CONTROL_MASK)
    {
      int scaleDirection;

      switch(pEvent->scroll.direction)
      {
      case CLUTTER_SCROLL_UP:
        scaleDirection = 1;
        break;
      case CLUTTER_SCROLL_DOWN:
        scaleDirection = -1;
        break;
      case CLUTTER_SCROLL_SMOOTH:
        printf("Support for CLUTTER_SCROLL_SMOOTH events has not yet been implemented.\n");
        scaleDirection = 0;
        break;
      default:
        scaleDirection = 0;
        break;
      }

      if(scaleDirection != 0)
      {
        double scale_x, scale_y;
        printf("scaling\n");

        if(scaleDirection == 1)
        {
          pStage->get_scale(scale_x, scale_y);
          pStage->set_scale(scale_x * FACTOR, scale_y * FACTOR);
          return HANDLED;
        }
        else
        {
          pStage->get_scale(scale_x, scale_y);
          pStage->set_scale(scale_x / FACTOR, scale_y / FACTOR);
          return HANDLED;
        }
      }
      else
      {
        return UNHANDLED;
      }
    }
    else
    {
      printf("scrolling\n");
      return UNHANDLED;
    }
  }
  else
  {
    return UNHANDLED;
  }
}

bool EntityStageUpdater::onKeyPressEvent(GdkEventKey *pEvent)
{
  int position;
  static Edge edge;
  static State state = NORMAL;

  printf("EntityStageUpdater::onKeyPressEvent\n");

  /* Handle escape */
  if(pEvent->keyval == CLUTTER_KEY_Escape)
  {
    printf("Escaped\n");
    state = NORMAL;
    return HANDLED;
  }

  switch(state)
  {
  case NORMAL:
    if(pEvent->keyval == 'a')
    {
      if(m_pGuiComponent->findFreeSlot(EDGE_BOTTOM, 2, &edge, &position))
      {
        printf("Adding port at %s %d\n", EDGE_TO_NAME(edge), position);
        m_pGuiComponent->createPort(edge, position, DIR_OUT, "port3");
      }
      else
      {
        printf("Can't add any more ports\n");
      }
      return HANDLED;
    }
    else if(pEvent->keyval == 'd')
    {
      state = WAITING_FOR_EDGE;
      printf("Enter edge of port to delete (L, R, T, B)\n");
      return HANDLED;
    }
    break;
  case WAITING_FOR_EDGE:
    state = WAITING_FOR_INDEX;
    switch(pEvent->keyval)
    {
    case 'l':
      edge = EDGE_LEFT;
      break;
    case 'r':
      edge = EDGE_RIGHT;
      break;
    case 't':
      edge = EDGE_TOP;
      break;
    case 'b':
      edge = EDGE_BOTTOM;
      break;
    default:
      state = WAITING_FOR_EDGE;
      break;
    }
    if(state == WAITING_FOR_INDEX)
    {
      printf("edge = %s\n", EDGE_TO_NAME(edge));
      printf("Enter position of port to be deleted (0-9)\n");
      return HANDLED;
    }
    break;
  case WAITING_FOR_INDEX:
    if(isdigit(pEvent->keyval))
    {
      int position = pEvent->keyval - '0';
      printf("Destroying port at edge %s pos %d\n", EDGE_TO_NAME(edge), position);
      m_pGuiComponent->destroyPort(edge, position);
      state = NORMAL;
      return HANDLED;
    }
    break;
  default:
    g_assert_not_reached();
  }

  printf("  key not handled\n");

  return UNHANDLED;
}

