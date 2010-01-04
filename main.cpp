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

#include "layout_instance.h"
#include "triangle_actor.h"
#include "vhdl_architecture.h"

#define UNHANDLED false
#define HANDLED false

#define STAGE_COLOR       Clutter::Color(0xF0, 0xF0, 0xF0, 0xFF)
#define COMPONENT_COLOR   Clutter::Color(0xAE, 0xFF, 0x7F, 0xFF)
#define PORT_COLOR        Clutter::Color(0x7F, 0xAE, 0xFF, 0xFF)

#define COMPONENT_WIDTH      200
#define COMPONENT_HEIGHT     300
#define COMPONENT_INITIAL_X  100
#define COMPONENT_INITIAL_Y  100

#define PORT_WIDTH    16
#define PORT_SPACING  8

#define BORDER_WIDTH  1

struct PortProperties
{
  gint x;
  gint y;
  gint orientation;
};

class DiagramElement
{
public:


};

/*
 * Any coordinates passed to this class should be relative to the position of
 * the body of a component.
 */
class PortList
{
public:
  /* Keep this in clock-wise order, because it is used to determine rotation of
   * port actors
   */
  typedef enum {
    LEFT,
    TOP,
    RIGHT,
    BOTTOM,
    NR_OF_PORTROWS
  } PortRowId;

private:
  struct PortData
  {
    PortData(bool horizontal, int *pRowCoord, int coord):
      horizontal(horizontal),
      pRowCoord(pRowCoord),
      portCoord(coord),
      pActor(NULL)
    {
    }

    bool horizontal;
    int *pRowCoord;
    int portCoord;
    Glib::RefPtr<Clutter::Actor> pActor;

    int x()
    {
      assert(pRowCoord != NULL);
      return horizontal ? portCoord : *pRowCoord;
    }

    int y()
    {
      assert(pRowCoord != NULL);
      return horizontal ? *pRowCoord : portCoord;
    }

  };

  struct PortRow
  {
    int row_coord;
    std::vector<PortData> ports;
  };

  PortRow portRows[NR_OF_PORTROWS];

  int m_width;
  int m_height;

private:

  static void resizePortVector(bool horizontal, PortRow *pPortRow, int newSize)
  {
    int i;

    if(newSize > pPortRow->ports.size())
    {
      /* Extend the vector with extra portData structures with the appropriate
       * coordinates
       */
      for(i = pPortRow->ports.size(); i < newSize; i++)
      {
        PortData portData(horizontal, &pPortRow->row_coord, PORT_SPACING + PORT_WIDTH / 2 + i * (PORT_SPACING + PORT_WIDTH));
        pPortRow->ports.push_back(portData);
      }
    }
    else if(newSize < pPortRow->ports.size())
    {
      /* Shift used ports closer together if needed */
      int freeSlots = 0;
      int i, j;

      /* First loop backwards to find enough unused places... */
      for(i = pPortRow->ports.size() - 1; i >= 0; i--)
      {
        if(!pPortRow->ports[i].pActor)
        {
          freeSlots++;
          if(freeSlots == pPortRow->ports.size() - newSize)
          {
            /* ... then loop forward from this point move everything closer together */
            for(j = i; j < pPortRow->ports.size(); j++)
            {
              if(pPortRow->ports[j].pActor)
              {
                pPortRow->ports[i].pActor = pPortRow->ports[j].pActor;
                pPortRow->ports[i].pActor->set_position(pPortRow->ports[i].x(), pPortRow->ports[i].y());
                i++;
              }
            }
          }
        }
      }
      assert(i != -1);
    }
  }

public:
  PortList():
    m_width(0),
    m_height(0)
  {
    int i;
    for(i = 0; i < NR_OF_PORTROWS; i++)
    {
      portRows[i].row_coord = -PORT_WIDTH/2;
    }
  }

  void addPort(int portRowId, int portIndex, Glib::RefPtr<Clutter::Actor> pActor)
  {
    float width, height;

    assert(portIndex < portRows[portRowId].ports.size());
    assert(!portRows[portRowId].ports[portIndex].pActor);
    portRows[portRowId].ports[portIndex].pActor = pActor;

    pActor->get_size(width, height);

    pActor->set_rotation(Clutter::Z_AXIS, portRowId * 90, 0, 0, 0);
    pActor->set_position(portRows[portRowId].ports[portIndex].x(),
                         portRows[portRowId].ports[portIndex].y());
  }

  Glib::RefPtr<Clutter::Actor> removePort(int portRowId, int portIndex)
  {
    Glib::RefPtr<Clutter::Actor> pNullActor, pOldActor;

    assert(portIndex < portRows[portRowId].ports.size());

    pOldActor = portRows[portRowId].ports[portIndex].pActor;
    assert(pOldActor);

    portRows[portRowId].ports[portIndex].pActor = pNullActor;

    return pOldActor;
  }

  void setSize(int width, int height)
  {
    int i;
    int x_slots = (width - PORT_SPACING) / (PORT_WIDTH + PORT_SPACING);
    int y_slots = (height - PORT_SPACING) / (PORT_WIDTH + PORT_SPACING);

    m_width = width;
    m_height = height;

    resizePortVector(true, &portRows[TOP],    x_slots);
    resizePortVector(true, &portRows[BOTTOM], x_slots);
    portRows[BOTTOM].row_coord = m_height + PORT_WIDTH / 2;

    resizePortVector(false, &portRows[LEFT],   y_slots);
    resizePortVector(false, &portRows[RIGHT],  y_slots);
    portRows[RIGHT].row_coord = m_width + PORT_WIDTH / 2;
  }

  void calculateMinimumSize(int *pWidth, int *pHeight)
  {
    int i;
    int freeSlots1, freeSlots2, freeSlotsMin;

    if(pWidth)
    {
      freeSlots1 = freeSlots2 = 0;
      for(i = 0; i < portRows[TOP].ports.size(); i++)
      {
        if(!portRows[TOP].ports[i].pActor)    freeSlots1++;
        if(!portRows[BOTTOM].ports[i].pActor) freeSlots2++;
      }
      freeSlotsMin = MIN(freeSlots1, freeSlots2);

      *pWidth = PORT_SPACING + (portRows[TOP].ports.size() - freeSlotsMin) * (PORT_SPACING + PORT_WIDTH);
    }

    if(pHeight)
    {
      freeSlots1 = freeSlots2 = 0;
      for(i = 0; i < portRows[LEFT].ports.size(); i++)
      {
        if(!portRows[LEFT].ports[i].pActor)   freeSlots1++;
        if(!portRows[RIGHT].ports[i].pActor)  freeSlots2++;
      }
      freeSlotsMin = MIN(freeSlots1, freeSlots2);

      *pHeight = PORT_SPACING + (portRows[LEFT].ports.size() - freeSlotsMin) * (PORT_SPACING + PORT_WIDTH);
    }
  }

  bool getClosestPort(bool unusedOnly, int x, int y, int *pPortRowIndex, int *pRowIndex)
  {
    int i, pr;
    int distance, minDistance;
    int portX, portY;

    //printf("getClosestPort(%s, %d, %d, ...)\n", unusedOnly ? "true" : "false", x, y);

    minDistance = -1;

    for(pr = 0; pr < NR_OF_PORTROWS; pr++)
    {
      for(i = 0; i < portRows[pr].ports.size(); i++)
      {
        if(!unusedOnly || !portRows[pr].ports[i].pActor)
        {
          portX = portRows[pr].ports[i].x();
          portY = portRows[pr].ports[i].y();

          distance = (portX - x) * (portX - x) +
                     (portY - y) * (portY - y);

          if(distance < minDistance || minDistance == -1)
          {
            minDistance = distance;
            *pPortRowIndex = pr;
            *pRowIndex = i;
          }
        }
      }
    }

    return (minDistance != -1);
  }
};

class Component: public DiagramElement
{
private:
  PortList  m_portList;

  Glib::RefPtr<Clutter::Group> m_pGroup;
  Glib::RefPtr<Clutter::Rectangle> m_pBody;
  Glib::RefPtr<Clutter::Rectangle> m_pPort1;
  Glib::RefPtr<Clutter::Rectangle> m_pPort2;
  Glib::RefPtr<Clutter::Text> m_pText;
  Glib::RefPtr<Clutter::Stage> m_pStage;

  sigc::connection m_dragHandler;

  gint m_bodyHandleOffsetX;
  gint m_bodyHandleOffsetY;

  Glib::RefPtr<Clutter::Actor> m_pDraggedActor;
  int m_dragFromRow;
  int m_dragFromPort;

public:
  Component(Glib::RefPtr<Clutter::Stage> pStage, int nrOfPorts = 0)
  {
    int i;

    m_pGroup = Clutter::Group::create();
    m_pGroup->set_position(COMPONENT_INITIAL_X, COMPONENT_INITIAL_Y);
    pStage->add_actor(m_pGroup);

    m_pBody = Clutter::Rectangle::create(Clutter::Color(0xAE, 0xFF, 0x7F, 0xFF));
    m_pBody->set_size(COMPONENT_WIDTH, COMPONENT_HEIGHT);
    m_pBody->set_position(0, 0);
    m_pBody->set_border_width(BORDER_WIDTH);
    m_pBody->set_reactive();
    m_pBody->signal_button_press_event().connect(sigc::mem_fun(*this, &Component::on_body_button_press_event));
    m_pGroup->add_actor(m_pBody);
    m_portList.setSize(COMPONENT_WIDTH, COMPONENT_HEIGHT);

    for(i = 0; i < nrOfPorts; i++)
    {
      Glib::RefPtr<Tutorial::Triangle> pPort = Tutorial::Triangle::create(PORT_COLOR);

      pPort->set_anchor_point(PORT_WIDTH / 2, PORT_WIDTH / 2);
      pPort->set_size(PORT_WIDTH, PORT_WIDTH);
      pPort->set_reactive();
      pPort->signal_button_press_event().connect(sigc::mem_fun(*this, &Component::on_port_button_press_event));
      m_pGroup->add_actor(pPort);

      m_portList.addPort(PortList::LEFT, i, pPort);
    }

    Glib::RefPtr<Tutorial::Triangle> pPort = Tutorial::Triangle::create(PORT_COLOR);
    pPort->set_anchor_point(PORT_WIDTH / 2, PORT_WIDTH / 2);
    pPort->set_size(PORT_WIDTH, PORT_WIDTH);
    pPort->set_reactive();
    pPort->signal_button_press_event().connect(sigc::mem_fun(*this, &Component::on_port_button_press_event));
    m_pGroup->add_actor(pPort);
    m_portList.addPort(PortList::TOP, 0, pPort);

    pPort = Tutorial::Triangle::create(PORT_COLOR);
    pPort->set_anchor_point(PORT_WIDTH / 2, PORT_WIDTH / 2);
    pPort->set_size(PORT_WIDTH, PORT_WIDTH);
    pPort->set_reactive();
    pPort->signal_button_press_event().connect(sigc::mem_fun(*this, &Component::on_port_button_press_event));
    m_pGroup->add_actor(pPort);
    m_portList.addPort(PortList::RIGHT, 0, pPort);

    pPort = Tutorial::Triangle::create(PORT_COLOR);
    pPort->set_anchor_point(PORT_WIDTH / 2, PORT_WIDTH / 2);
    pPort->set_size(PORT_WIDTH, PORT_WIDTH);
    pPort->set_reactive();
    pPort->signal_button_press_event().connect(sigc::mem_fun(*this, &Component::on_port_button_press_event));
    m_pGroup->add_actor(pPort);
    m_portList.addPort(PortList::BOTTOM, 0, pPort);

    m_pText = Clutter::Text::create("normal 10", "Use ctrl-scrollwheel\nto zoom");
    m_pText->set_position(30, 100);
    m_pText->set_line_alignment(Pango::ALIGN_CENTER);
    m_pGroup->add_actor(m_pText);

    m_pGroup->show_all();

    m_pStage = pStage;
  }
/*    printf("Body %s event\n", (pEvent->type == CLUTTER_MOTION) ? "motion" :
                                (pEvent->type == CLUTTER_BUTTON_RELEASE) ? "button_release" :
                                (pEvent->type == CLUTTER_BUTTON_PRESS) ? "button_press" :
                                (pEvent->type == CLUTTER_ENTER) ? "enter" :
                                (pEvent->type == CLUTTER_LEAVE) ? "leave" : "other");*/

private:
  bool on_body_button_press_event(Clutter::ButtonEvent* pEvent)
  {
    // Remember the point within the object where it was picked up
    float x, y;
    m_pGroup->get_position(x, y);
    m_bodyHandleOffsetX = pEvent->x - x;
    m_bodyHandleOffsetY = pEvent->y - y;

    // Register for motion and button release events from the stage
    m_dragHandler = m_pStage->signal_captured_event().connect(sigc::mem_fun(*this, &Component::on_body_drag_event));

    return HANDLED;
  }

  bool on_port_button_press_event(Clutter::ButtonEvent* pEvent)
  {
    float x, y, offsetX, offsetY;
    bool ret;

    m_pGroup->get_position(x, y);
    offsetX = pEvent->x - x;
    offsetY = pEvent->y - y;

    // Register for motion and button release events from the stage
    m_dragHandler = m_pStage->signal_captured_event().connect(sigc::mem_fun(*this, &Component::on_port_drag_event));

    ret = m_portList.getClosestPort(false, offsetX, offsetY, &m_dragFromRow, &m_dragFromPort);
    assert(ret);
    m_pDraggedActor = m_portList.removePort(m_dragFromRow, m_dragFromPort);

    return HANDLED;
  }

  bool on_body_drag_event(Clutter::Event *pEvent)
  {
    if(pEvent->type == CLUTTER_MOTION)
    {
      m_pGroup->set_position(pEvent->motion.x - m_bodyHandleOffsetX, pEvent->motion.y - m_bodyHandleOffsetY);
      return HANDLED;
    }
    else if(pEvent->type == CLUTTER_BUTTON_RELEASE)
    {
      m_dragHandler.disconnect();
      return HANDLED;
    }
    else
    {
      return UNHANDLED;
    }
  }

  bool on_port_drag_event(Clutter::Event *pEvent)
  {
    int portRowId, portIndex;
    float offset_x, offset_y;

    m_pGroup->get_position(offset_x, offset_y);

    if(pEvent->type == CLUTTER_MOTION)
    {
      if(m_portList.getClosestPort(true, pEvent->motion.x - offset_x, pEvent->motion.y - offset_y, &portRowId, &portIndex) &&
         ((m_dragFromRow != portRowId) || (m_dragFromPort != portIndex)) )
      {
        m_portList.addPort(portRowId, portIndex, m_pDraggedActor);
        m_portList.removePort(portRowId, portIndex);
        m_dragFromRow = portRowId;
        m_dragFromPort = portIndex;
      }

      return HANDLED;
    }
    else if(pEvent->type == CLUTTER_BUTTON_RELEASE)
    {
      m_portList.addPort(m_dragFromRow, m_dragFromPort, m_pDraggedActor);

      m_dragHandler.disconnect();
      return HANDLED;
    }
    else
    {
      return UNHANDLED;
    }
  }

};

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
  /* Try out the model classes */
  VHDLArchitecture arch("myarch");
  VHDLEntity entity("myentity");
  arch.setEntity(&entity);

  VHDLComponent *pComponent = new VHDLComponent("mycomponent1");
  arch.addComponent(pComponent);

  VHDLPort *pPort = new VHDLPort("myport1");
  pPort->setDirection(VHDLPort::DIR_IN);
  pComponent->addPort(pPort);

  pPort = new VHDLPort("myport2");
  pPort->setDirection(VHDLPort::DIR_OUT);
  pComponent->addPort(pPort);

  VHDLSignal *pSignal = new VHDLSignal("mysignal1");
  arch.addSignal(pSignal);
  pSignal = new VHDLSignal("mysignal2");
  arch.addSignal(pSignal);

  VHDLInstance *pInstance = new VHDLInstance("myinstance1", arch.findComponentByName("mycomponent1"));
  arch.addInstance(pInstance);

  pPort = pInstance->getComponent()->findPortByName("myport1");
  pInstance->associateSignalWithPort(arch.findSignalByName("mysignal2"), pPort);



  VHDLEntity externalEntity("entity2");
  pPort = new VHDLPort("myport1");
  pPort->setDirection(VHDLPort::DIR_IN);
  externalEntity.addPort(pPort);

  pPort = new VHDLPort("myport2");
  pPort->setDirection(VHDLPort::DIR_OUT);
  externalEntity.addPort(pPort);

  pComponent->associateEntity(&externalEntity);



  /* read layout instance */
  /* find associated vhdl instance by name */
  /* loop through loaded ports and associate with ports in the vhdl instance by name */
  LayoutPort *pLayoutPort;
  LayoutInstance layoutInstance;
  layoutInstance.setPosition(LayoutPosition(300,200));
  layoutInstance.setSize(LayoutSize(200, 300));

  pInstance = arch.findInstanceByName("myinstance1");

  pPort = pInstance->getComponent()->findPortByName("myport1");
  pLayoutPort = new LayoutPort(pPort);
  layoutInstance.addPort(EDGE_LEFT, 1, pLayoutPort);

  pPort = pInstance->getComponent()->findPortByName("myport2");
  pLayoutPort = new LayoutPort(pPort);
  layoutInstance.addPort(EDGE_RIGHT, 2, pLayoutPort);

  layoutInstance.associateInstance(arch.findInstanceByName("myinstance1"));


  externalEntity.setName("blaat");

  FILE *pFile = fopen("dinges.layout", "w+b");
  layoutInstance.write(pFile);
  fclose(pFile);

  pFile = fopen("dinges.vhd", "w+b");
  arch.write(pFile, 0);
  fclose(pFile);

  printf("main: removing port myport2\n");
  externalEntity.removePort(externalEntity.findPortByName("myport2"));

  printf("main: removing signal mysignal2\n");
  arch.removeSignal(arch.findSignalByName("mysignal2"));

  exit(0);



  Clutter::init(&argc, &argv);
  Clutter::set_motion_events_enabled(false);

  // Get the stage and set its size and color:
  const Glib::RefPtr<Clutter::Stage> stage = Clutter::Stage::get_default();
  stage->set_size(1100, 700);
  stage->set_color(STAGE_COLOR);

  Component *pComp = new Component(stage, 3);

  stage->signal_captured_event().connect(sigc::bind(&on_my_captured_event, stage));
  stage->show();

  // Start the main loop, so we can respond to events:
  Clutter::main();

  return 0;
}
