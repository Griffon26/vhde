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

EntityStageUpdater::EntityStageUpdater():
  m_pEntity(nullptr),
  m_pGuiComponent(nullptr)
{
}

EntityStageUpdater::~EntityStageUpdater()
{
  m_key_press_connection.disconnect();
  m_pGuiComponent.reset(nullptr);
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
  m_key_press_connection = m_pStage->signal_key_press_event().connect(sigc::bind(sigc::mem_fun(*this, &EntityStageUpdater::on_key_pressed), m_pGuiComponent.get()));
}

enum State
{
  NORMAL,
  WAITING_FOR_EDGE,
  WAITING_FOR_INDEX
};

bool EntityStageUpdater::on_idle_request_quit()
{
  quit_requested.emit();
  return false;
}

bool EntityStageUpdater::on_key_pressed(Clutter::KeyEvent *pEvent, GuiComponent *pGuiComponent)
{
  int position;
  static Edge edge;
  static State state = NORMAL;

  if(m_pStage)
  {
    std::cout << "Stage points to something valid" << std::endl;
  }
  else
  {
    std::cout << "Stage !!DOES NOT!! point to something valid" << std::endl;
  }

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
      if(pGuiComponent->findFreeSlot(EDGE_BOTTOM, 2, &edge, &position))
      {
        printf("Adding port at %s %d\n", EDGE_TO_NAME(edge), position);
        pGuiComponent->createPort(edge, position, DIR_OUT, "port3");
      }
      else
      {
        printf("Can't add any more ports\n");
      }
    }
    if(pEvent->keyval == 'd')
    {
      state = WAITING_FOR_EDGE;
      printf("Enter edge of port to delete (L, R, T, B)\n");
    }
    if(pEvent->keyval == 'q')
    {
      printf("Exiting...\n");

      /* Send this signal on idle instead of calling emit() here to avoid a
       * deadlock with a second clutter signal (when the window is destroyed)
       * being emitted from within this handler of the first.
       */
      Glib::signal_idle().connect(sigc::mem_fun(*this, &EntityStageUpdater::on_idle_request_quit));
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
    }
    break;
  case WAITING_FOR_INDEX:
    if(isdigit(pEvent->keyval))
    {
      int position = pEvent->keyval - '0';
      printf("Destroying port at edge %s pos %d\n", EDGE_TO_NAME(edge), position);
      pGuiComponent->destroyPort(edge, position);
      state = NORMAL;
    }
    break;
  default:
    g_assert_not_reached();
  }

  return HANDLED;
}

