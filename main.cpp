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

#include <assert.h>

#include <cluttermm.h>
#include <iostream>

#include "gui_instance.h"
#include "gui_signal.h"
#include "layout_instance.h"
#include "layout_signal.h"
#include "vhdl_architecture.h"

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
      double scale_x, scale_y;
      if(pEvent->scroll.direction == CLUTTER_SCROLL_UP)
      {
        pStage->get_scale(scale_x, scale_y);
        pStage->set_scale(scale_x * FACTOR, scale_y * FACTOR);
        return HANDLED;
      }
      else if(pEvent->scroll.direction == CLUTTER_SCROLL_DOWN)
      {
        pStage->get_scale(scale_x, scale_y);
        pStage->set_scale(scale_x / FACTOR, scale_y / FACTOR);
        return HANDLED;
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


int main(int argc, char** argv)
{
  Clutter::init(&argc, &argv);
  Clutter::set_motion_events_enabled(false);

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
  VHDLEntity externalEntity("externalentity");
  pPort = new VHDLPort("myport1");
  pPort->setDirection(DIR_IN);
  externalEntity.init_addPort(pPort);

  pPort = new VHDLPort("myport2");
  pPort->setDirection(DIR_OUT);
  externalEntity.init_addPort(pPort);

  externalEntity.init_done();





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

  arch.addComponent(pComponent);
  pComponent->associateEntity(&externalEntity);

  VHDLSignal *pSignal = new VHDLSignal("mysignal1");
  arch.addSignal(pSignal);
  pSignal = new VHDLSignal("mysignal2");
  arch.addSignal(pSignal);

  VHDLInstance *pVHDLInstance = new VHDLInstance("myinstance1", arch.findComponentByName("externalentity"));
  arch.addInstance(pVHDLInstance);

  pPort = pVHDLInstance->getComponent()->findPortByName("myport1");
  pVHDLInstance->associateSignalWithPort(arch.findSignalByName("mysignal2"), pPort);





  /* read layout instance */
  /* find associated vhdl instance by name */
  /* loop through loaded ports and associate with ports in the vhdl instance by name */
  LayoutPort *pLayoutPort;


  LayoutComponent layoutComponent;
  layoutComponent.associateEntity(&externalEntity);

  LayoutInstance layoutInstance(&layoutComponent);
  layoutInstance.setPosition(LayoutPosition(300,200));
  layoutInstance.setSize(LayoutSize(200, 300));

  pVHDLInstance = arch.findInstanceByName("myinstance1");

  pPort = pVHDLInstance->getComponent()->findPortByName("myport1");
  pLayoutPort = new LayoutPort(pPort);
  layoutInstance.init_addPort(EDGE_LEFT, 0, pLayoutPort);

  pPort = pVHDLInstance->getComponent()->findPortByName("myport2");
  pLayoutPort = new LayoutPort(pPort);
  layoutInstance.init_addPort(EDGE_RIGHT, 8, pLayoutPort);

  layoutInstance.init_done();

  layoutInstance.associateInstance(arch.findInstanceByName("myinstance1"));

  externalEntity.setName("blaat");


  LayoutSignal layoutSignal;
  layoutSignal.associateSignal(arch.findSignalByName("mysignal2"));
  layoutSignal.connect(LayoutSignal::END, &layoutInstance, EDGE_LEFT, 0);

  GuiSignal guiSignal(stage, &layoutSignal);


  Edge edge;
  int position;
  int x, y;
  GuiInstance guiInstance(stage, &layoutInstance);

  stage->signal_captured_event().connect(sigc::bind(&on_my_captured_event, stage));
  stage->show();
  Clutter::main();

  FILE *pFile = fopen("dinges.layout", "w+b");
  layoutInstance.write(pFile);
  layoutSignal.write(pFile);
  fclose(pFile);

  pFile = fopen("dinges.vhd", "w+b");
  arch.write(pFile, 0);
  fclose(pFile);

  return 0;
}
