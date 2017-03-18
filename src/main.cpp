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

#include <cluttermm.h>
#include <fstream>
#include <iostream>

#include "gui_component.h"
#include "gui_instance.h"
#include "gui_signal.h"
#include "layout_architecture.h"
#include "layout_instance.h"
#include "layout_signal.h"
#include "project.h"
#include "vhdl_architecture.h"
#include "vhdl_port.h"

#define STAGE_COLOR       Clutter::Color(0xF0, 0xF0, 0xF0, 0xFF)
#define COMPONENT_COLOR   Clutter::Color(0xAE, 0xFF, 0x7F, 0xFF)
#define PORT_COLOR        Clutter::Color(0x7F, 0xAE, 0xFF, 0xFF)

#define COMPONENT_WIDTH      200
#define COMPONENT_HEIGHT     300
#define COMPONENT_INITIAL_X  100
#define COMPONENT_INITIAL_Y  100

#define WINDOW_WIDTH (100)
#define WINDOW_HEIGHT (70)

#define FACTOR  1.2

static bool on_my_captured_event(Clutter::Event* pEvent, Glib::RefPtr<Clutter::Stage> pStage)
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

enum State
{
  NORMAL,
  WAITING_FOR_EDGE,
  WAITING_FOR_INDEX
};

static bool on_key_pressed(Clutter::KeyEvent *pEvent, GuiComponent *pGuiComponent)
{
  int position;
  static Edge edge;
  static State state = NORMAL;

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
      Clutter::main_quit();
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


int main(int argc, char** argv)
{
  Clutter::init(&argc, &argv);
  Clutter::set_motion_events_enabled(false);
  //Clutter::set_font_flags(Clutter::get_font_flags() & ~Clutter::FONT_MIPMAPPING);

  /* Get the stage and set its size and color */
  const Glib::RefPtr<Clutter::Stage> stage = Clutter::Stage::get_default();
  stage->set_size(WINDOW_WIDTH, WINDOW_HEIGHT);
  stage->set_user_resizable(TRUE);
  stage->set_color(STAGE_COLOR);


  const Glib::RefPtr<Clutter::Rectangle> pActor = Clutter::Rectangle::create();
  stage->add_actor(pActor);

  fprintf(stderr, "Checking reference counting on stage. If this crashes you need a fix for cluttermm.\n");
  fprintf(stderr, "Here we go...\n");
  fprintf(stderr, "going once... ");
  pActor->get_stage();
  fprintf(stderr, "going twice... ");
  pActor->get_stage();
  fprintf(stderr, "sold to the man in the blue hat!\n");
  pActor->get_stage();


  /*
   * Read in some example files to create a model
   */
  Project project;
  project.addFile("test/top_entity.vhd");
  project.addFile("test/used_entity.vhd");
  project.resolveEntityReferences();
  project.resolveLayoutReferences();

  /* TODO: resolve the conflicts between the VHDL and the layout files, such as
   * absence of a .layout file
   */

  /*
   * Create GUI objects from some of the layout classes to show them on the screen
   */
  auto pLayoutArch = project.getLayoutArchitecture("test/top_entity.vhd");

  GuiSignal guiSignal(stage, pLayoutArch->getSignal(0));
  GuiInstance guiInstance(stage, pLayoutArch->getInstance(0));
  GuiComponent guiComponent(stage, pLayoutArch->getComponent());

  /*
   * Allow the user to interact with the diagram
   */
  stage->signal_captured_event().connect(sigc::bind(&on_my_captured_event, stage));
  stage->signal_key_press_event().connect(sigc::bind(&on_key_pressed, &guiComponent));
  stage->show();
  Clutter::main();

  /*
   * Write the model back out to files, suffixing each filename with a '2'
   * (to avoid making changes in versioned example files)
   */
  project.save();

  return 0;
}
