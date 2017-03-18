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

#include <assert.h>

#include <cluttermm.h>
#include <iostream>

#include "gui_component.h"
#include "gui_instance.h"
#include "gui_signal.h"
#include "layout_instance.h"
#include "layout_signal.h"
#include "vhdl_architecture.h"
#include "vhdl_port.h"

#define STAGE_COLOR       Clutter::Color(0xF0, 0xF0, 0xF0, 0xFF)
#define COMPONENT_COLOR   Clutter::Color(0xAE, 0xFF, 0x7F, 0xFF)
#define PORT_COLOR        Clutter::Color(0x7F, 0xAE, 0xFF, 0xFF)

#define COMPONENT_WIDTH      200
#define COMPONENT_HEIGHT     300
#define COMPONENT_INITIAL_X  100
#define COMPONENT_INITIAL_Y  100


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
  stage->set_size(1100, 700);
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

  /* Try out the model classes */
  VHDLPort *pPort;

  /* Create one entity out of a package */
  /* TODO: put the entity in a store, accessible by name */
  VHDLEntity externalEntity("externalentity");
  pPort = new VHDLPort("myport1");
  pPort->setDirection(DIR_IN);
  externalEntity.init_addPort(pPort);

  pPort = new VHDLPort("myport2");
  pPort->setDirection(DIR_INOUT);
  externalEntity.init_addPort(pPort);

  externalEntity.init_done();



  printf("externalEntity: %s\n", externalEntity.getName().c_str());



  VHDLArchitecture arch("myarch");
  VHDLEntity entity("myentity");
  arch.setEntity(&entity);


  /* Create a component declaration for the external entity */
  VHDLComponent *pComponent = new VHDLComponent();

  pPort = new VHDLPort("myport1");
  pPort->setDirection(DIR_IN);
  pComponent->init_addPort(pPort);

  pPort = new VHDLPort("myport2");
  pPort->setDirection(DIR_INOUT);
  pComponent->init_addPort(pPort);

  pComponent->init_done();

  arch.init_addComponent(pComponent);

  /* TODO: get the VHDLEntity to be associated from a store by name */
  /* TODO: check consistency with entity and return failure if incorrect
   *       The caller can then ask the user how to resolve it and try again
   */
  pComponent->associateEntity(&externalEntity);

  printf("vhdlcomponent: %s\n", pComponent->getName().c_str());


  VHDLSignal *pSignal = new VHDLSignal("mysignal1");
  arch.init_addSignal(pSignal);
  pSignal = new VHDLSignal("mysignal2");
  arch.init_addSignal(pSignal);

  VHDLInstance *pVHDLInstance = new VHDLInstance("myinstance1", arch.findComponentByName("externalentity"));
  arch.init_addInstance(pVHDLInstance);

  pPort = pVHDLInstance->getComponent()->findPortByName("myport1");
  pVHDLInstance->connectSignalToPort(arch.findSignalByName("mysignal2"), pPort);

  arch.init_done();





  /* read layout instance */
  /* find associated vhdl instance by name */
  /* loop through loaded ports and associate with ports in the vhdl instance by name */
  LayoutPort *pLayoutPort;

  VHDLEntity *pVHDLEntity;

  LayoutComponent layoutComponent;
  layoutComponent.setSize(LayoutSize(150, 400));

  /* TODO: Create a LayoutComponent as it is read from the layout file.
   *       Find the corresponding VHDL entity by name from the store.
   */
  pVHDLEntity = &externalEntity;

  layoutComponent.associateEntity(pVHDLEntity);

  pPort = pVHDLEntity->findPortByName("myport1");
  pLayoutPort = new LayoutPort();
  pLayoutPort->associateVHDLPort(pPort);
  layoutComponent.init_addPort(EDGE_LEFT, 0, pLayoutPort);

  pPort = pVHDLEntity->findPortByName("myport2");
  pLayoutPort = new LayoutPort();
  pLayoutPort->associateVHDLPort(pPort);
  layoutComponent.init_addPort(EDGE_RIGHT, 8, pLayoutPort);

  layoutComponent.init_done();




  /* TODO: Create a LayoutInstance as it is read from the layout file.
   *       Find the corresponding VHDL instance by name from the store (or arch?)
   *       Find the corresponding LayoutComponent by name (VHDLInstance->getComponent()->getName()) from the store
   */
  LayoutInstance layoutInstance;
  layoutInstance.setPosition(LayoutPosition(300,200));
  layoutInstance.setSize(LayoutSize(200, 300));

  pVHDLInstance = arch.findInstanceByName("myinstance1");

  pPort = pVHDLInstance->getComponent()->findPortByName("myport1");
  pLayoutPort = new LayoutPort();
  pLayoutPort->associateVHDLPort(pPort);
  layoutInstance.init_addPort(EDGE_LEFT, 0, pLayoutPort);

  pPort = pVHDLInstance->getComponent()->findPortByName("myport2");
  pLayoutPort = new LayoutPort();
  pLayoutPort->associateVHDLPort(pPort);
  layoutInstance.init_addPort(EDGE_RIGHT, 8, pLayoutPort);

  layoutInstance.init_done();

  layoutInstance.associateLayoutComponent(&layoutComponent);
  layoutInstance.associateVHDLInstance(pVHDLInstance);


  LayoutSignal layoutSignal;
  layoutSignal.associateSignal(arch.findSignalByName("mysignal2"));
  layoutSignal.connect(LayoutSignal::END, &layoutInstance, EDGE_LEFT, 0);

  GuiSignal guiSignal(stage, &layoutSignal);


  GuiInstance guiInstance(stage, &layoutInstance);
  GuiComponent guiComponent(stage, &layoutComponent);


  stage->signal_captured_event().connect(sigc::bind(&on_my_captured_event, stage));
  stage->signal_key_press_event().connect(sigc::bind(&on_key_pressed, &guiComponent));
  stage->show();
  Clutter::main();

  FILE *pFile = fopen("dinges.layout", "w+b");
  layoutComponent.write(pFile);
  layoutInstance.write(pFile);
  layoutSignal.write(pFile);
  fclose(pFile);

  pFile = fopen("dinges.vhd", "w+b");
  arch.write(pFile, 0);
  fclose(pFile);

  pFile = fopen("externalentity.vhd", "w+b");
  externalEntity.write(pFile, 0);
  fclose(pFile);

  return 0;
}
