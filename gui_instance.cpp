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

#include "gui_common.h"
#include "gui_instance.h"
#include "vhdl_component.h"
#include "vhdl_instance.h"

GuiInstance::GuiInstance(Glib::RefPtr<Clutter::Stage> pStage, LayoutInstance *pLayoutInstance):
  GuiBlock(pStage, pLayoutInstance)
{
  VHDLComponent *pComponent;

  printf("GuiInstance (%p): constructor body\n", this);

  m_onLayoutPortAddedConnection = pLayoutInstance->port_added.connect(sigc::mem_fun(this, &GuiInstance::onLayoutPortAdded));
  m_onLayoutPortRemovedConnection = pLayoutInstance->port_removed.connect(sigc::mem_fun(this, &GuiInstance::onLayoutPortRemoved));

  pComponent = static_cast<VHDLInstance *>(pLayoutInstance->getAssociatedVHDLInstance())->getComponent();

  m_onVHDLPortAddedConnection = pComponent->port_added.connect(sigc::mem_fun(this, &GuiInstance::onVHDLPortAdded));
  m_onVHDLPortRemovedConnection = pComponent->port_removed.connect(sigc::mem_fun(this, &GuiInstance::onVHDLPortRemoved));

  m_eventData.actionId = 0;
  m_eventData.layoutEventReceived = false;
  m_eventData.vhdlEventReceived = false;
}

GuiInstance::~GuiInstance()
{
  m_onLayoutPortAddedConnection.disconnect();
  m_onLayoutPortRemovedConnection.disconnect();
}

bool GuiInstance::onBodyButtonPress(Clutter::ButtonEvent *pEvent)
{
  bool ret;

  ret = GuiBlock::onBodyButtonPress(pEvent);
  g_assert(ret == HANDLED);

  /* Determine if this button press is the start of a move operation */
  //printf("instance onBodyButtonPress button = %d modstate = %d\n", pEvent->button, pEvent->modifier_state);
  m_dragIsMove = (pEvent->button == 1) &&
                 ( (pEvent->modifier_state == CLUTTER_CONTROL_MASK) ||
                   (pEvent->modifier_state == 0) );

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

void GuiInstance::handlePortAdded()
{
  if(m_eventData.layoutEventReceived && m_eventData.vhdlEventReceived)
  {
    m_eventData.pLayoutPort->associateVHDLPort(m_eventData.pVHDLPort);

    addPort(m_eventData.edge, m_eventData.position, m_eventData.pLayoutPort);

    m_eventData.layoutEventReceived = false;
    m_eventData.vhdlEventReceived = false;
  }
}

void GuiInstance::onLayoutPortAdded(int actionId, Edge edge, int position, LayoutPort *pLayoutPort)
{
  g_assert(!m_eventData.vhdlEventReceived || m_eventData.actionId == actionId);

  printf("GuiInstance::OnLayoutPortAdded\n");

  m_eventData.actionId = actionId;
  m_eventData.layoutEventReceived = true;
  m_eventData.edge = edge;
  m_eventData.position = position;
  m_eventData.pLayoutPort = pLayoutPort;

  handlePortAdded();
}

void GuiInstance::onVHDLPortAdded(int actionId, VHDLPort *pVHDLPort)
{
  g_assert(!m_eventData.layoutEventReceived || m_eventData.actionId == actionId);

  printf("GuiInstance::OnVHDLPortAdded\n");

  m_eventData.actionId = actionId;
  m_eventData.vhdlEventReceived = true;
  m_eventData.pVHDLPort = pVHDLPort;

  handlePortAdded();
}

void GuiInstance::handlePortRemoved()
{
  if(m_eventData.layoutEventReceived && m_eventData.vhdlEventReceived)
  {
    printf("GuiInstance(%p)::handlePortRemoved(%s %d)\n", this, EDGE_TO_NAME(m_eventData.edge), m_eventData.position);
    removePort(m_eventData.edge, m_eventData.position);

    m_eventData.layoutEventReceived = false;
    m_eventData.vhdlEventReceived = false;
  }
}

void GuiInstance::onLayoutPortRemoved(int actionId, Edge edge, int position, LayoutPort *pLayoutPort)
{
  g_assert(!m_eventData.vhdlEventReceived || m_eventData.actionId == actionId);

  printf("GuiInstance::OnLayoutPortRemoved(%s(%p))\n", pLayoutPort->getName().c_str(), pLayoutPort);

  m_eventData.actionId = actionId;
  m_eventData.layoutEventReceived = true;
  m_eventData.edge = edge;
  m_eventData.position = position;

  handlePortRemoved();
}

void GuiInstance::onVHDLPortRemoved(int actionId, VHDLPort *pVHDLPort)
{
  g_assert(!m_eventData.layoutEventReceived || m_eventData.actionId == actionId);

  printf("GuiInstance::OnVHDLPortRemoved\n");

  m_eventData.actionId = actionId;
  m_eventData.vhdlEventReceived = true;
  m_eventData.pVHDLPort = pVHDLPort;

  handlePortRemoved();
}
