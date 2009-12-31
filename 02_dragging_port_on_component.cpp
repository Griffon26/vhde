/*
 * Copyright (c) 2008 Openismus GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <cluttermm.h>
#include <iostream>

#define UNHANDLED false
#define HANDLED false

#define STAGE_COLOR Clutter::Color(0xF0, 0xF0, 0xF0, 0xFF)

#define BODY_INITIAL_X  200
#define BODY_INITIAL_Y  100

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

class Component: public DiagramElement
{
public:
  Component(Glib::RefPtr<Clutter::Stage> pStage)
  {
    m_pGroup = Clutter::Group::create();
    m_pGroup->set_position(100,0);
    pStage->add_actor(m_pGroup);

    m_pBody = Clutter::Rectangle::create(Clutter::Color(0xAE, 0xFF, 0x7F, 0xFF));
    m_pBody->set_size(200, 300);
    m_pBody->set_position(BODY_INITIAL_X, BODY_INITIAL_Y);
    m_pBody->set_border_width(BORDER_WIDTH);
    m_pBody->set_reactive();
    m_pBody->signal_button_press_event().connect(sigc::mem_fun(*this, &Component::on_body_button_press_event));
    m_pGroup->add_actor(m_pBody);

    m_pPort1 = Clutter::Rectangle::create(Clutter::Color(0x7F, 0xAE, 0xFF, 0xFF));
    m_pPort1->set_size(16, 16);
    m_pPort1->set_position(BODY_INITIAL_X - (PORT_WIDTH - BORDER_WIDTH), BODY_INITIAL_Y + PORT_SPACING);
    m_pPort1->set_border_width(BORDER_WIDTH);
    m_pPort1->set_reactive();
    m_pPort1->signal_button_press_event().connect(sigc::mem_fun(*this, &Component::on_port_button_press_event));
    m_pGroup->add_actor(m_pPort1);

    m_pPort2 = Clutter::Rectangle::create(Clutter::Color(0x7F, 0xAE, 0xFF, 0xFF));
    m_pPort2->set_size(16, 16);
    m_pPort2->set_position(BODY_INITIAL_X - (PORT_WIDTH - BORDER_WIDTH), BODY_INITIAL_Y + PORT_SPACING * 2 + PORT_WIDTH);
    m_pPort2->set_border_width(BORDER_WIDTH);
    m_pGroup->add_actor(m_pPort2);

    m_pGroup->show_all();

    m_pStage = pStage;
  }
/*    printf("Body %s event\n", (pEvent->type == CLUTTER_MOTION) ? "motion" :
                                (pEvent->type == CLUTTER_BUTTON_RELEASE) ? "button_release" :
                                (pEvent->type == CLUTTER_BUTTON_PRESS) ? "button_press" : 
                                (pEvent->type == CLUTTER_ENTER) ? "enter" :
                                (pEvent->type == CLUTTER_LEAVE) ? "leave" : "other");*/

private:
  bool get_port_from_coordinate(gint x, gint y, PortProperties *pPort)
  {
    float body_x1, body_y1, body_x2, body_y2;
    float offset_x, offset_y;
    float body_width, body_height;
    gint port_index;
    gint side_edge_distance, topbottom_edge_distance;

    m_pBody->get_position(body_x1, body_y1);
    m_pGroup->get_position(offset_x, offset_y);
    body_x1 += offset_x;
    body_y1 += offset_y;
    m_pBody->get_size(body_width, body_height);

    body_x2 = body_x1 + body_width;
    body_y2 = body_y1 + body_height;

    if(x < (body_x1 + body_x2) / 2)
    {
      pPort->x = body_x1 - (PORT_WIDTH - BORDER_WIDTH);
      side_edge_distance = x - body_x1;
    }
    else
    {
      pPort->x = body_x2 - BORDER_WIDTH;
      side_edge_distance = body_x2 - x;
    }

    if(y < (body_y1 + body_y2) / 2)
    {
      pPort->y = body_y1 - (PORT_WIDTH - BORDER_WIDTH);
      topbottom_edge_distance = y - body_y1;
    }
    else
    {
      pPort->y = body_y2 - BORDER_WIDTH;
      topbottom_edge_distance = body_y2 - y;
    }

    if(side_edge_distance < topbottom_edge_distance)
    {
      if(y < body_y1 + PORT_SPACING)
      {
        y = body_y1 + PORT_SPACING;
      }
      else if(y > body_y2 - (PORT_WIDTH + PORT_SPACING))
      {
        y = body_y2 - (PORT_WIDTH + PORT_SPACING);
      }

      port_index = ((y - (body_y1 + PORT_SPACING / 2)) / (PORT_WIDTH + PORT_SPACING));
      pPort->y = body_y1 + port_index * (PORT_SPACING + PORT_WIDTH) + PORT_SPACING;
    }
    else
    {
      if(x < body_x1 + PORT_SPACING)
      {
        x = body_x1 + PORT_SPACING;
      }
      else if(x > body_x2 - (PORT_WIDTH + PORT_SPACING))
      {
        x = body_x2 - (PORT_WIDTH + PORT_SPACING);
      }

      port_index = ((x - (body_x1 + PORT_SPACING / 2)) / (PORT_WIDTH + PORT_SPACING));
      pPort->x = body_x1 + port_index * (PORT_SPACING + PORT_WIDTH) + PORT_SPACING;
    }

    return true;
  }

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
    // Register for motion and button release events from the stage
    m_dragHandler = m_pStage->signal_captured_event().connect(sigc::mem_fun(*this, &Component::on_port_drag_event));

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
    if(pEvent->type == CLUTTER_MOTION)
    {
      PortProperties props;
      if(get_port_from_coordinate(pEvent->motion.x, pEvent->motion.y, &props))
      {
        float offset_x, offset_y;
        m_pGroup->get_position(offset_x, offset_y);
        m_pPort1->set_position(props.x - offset_x, props.y - offset_y);
      }
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

  Glib::RefPtr<Clutter::Group> m_pGroup;
  Glib::RefPtr<Clutter::Rectangle> m_pBody;
  Glib::RefPtr<Clutter::Rectangle> m_pPort1;
  Glib::RefPtr<Clutter::Rectangle> m_pPort2;
  Glib::RefPtr<Clutter::Stage> m_pStage;

  sigc::connection m_dragHandler;

  gint m_bodyHandleOffsetX;
  gint m_bodyHandleOffsetY;
};


int main(int argc, char** argv)
{
  Clutter::init(&argc, &argv);
  Clutter::set_motion_events_enabled(false);

  // Get the stage and set its size and color:
  const Glib::RefPtr<Clutter::Stage> stage = Clutter::Stage::get_default();
  stage->set_size(1200, 800);
  stage->set_color(STAGE_COLOR);

  Component *pComp = new Component(stage);

  stage->show();

  // Start the main loop, so we can respond to events:
  Clutter::main();

  return 0;
}

