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
#include "layout_port.h"
#include "vhdl_component.h"
#include "vhdl_instance.h"
#include "vhdl_port.h"

GuiInstance::GuiInstance(Glib::RefPtr<Clutter::Stage> pStage, LayoutInstance *pLayoutInstance):
  GuiBlock(pStage, pLayoutInstance)
{
  std::list<GuiPort *>::iterator it;
  VHDLComponent *pComponent;

  printf("GuiInstance (%p): constructor body\n", this);

  m_onLayoutPortAddedConnection = pLayoutInstance->port_added.connect(sigc::mem_fun(this, &GuiInstance::onLayoutPortAdded));

  pComponent = static_cast<VHDLInstance *>(pLayoutInstance->getAssociatedVHDLInstance())->getComponent();
  m_onVHDLPortAddedConnection = pComponent->port_added.connect(sigc::mem_fun(this, &GuiInstance::onVHDLPortAdded));

  /* Subscribe to "removed" signal of all layoutports for which there is a guiport.
   * I would have done this in GuiBlock where the ports are first added, but GuiComponent
   * is not interested in these signals
   */
  for(auto &pPort: m_ports)
  {
    auto pLayoutPort = pPort->getAssociatedLayoutPort();
    g_assert(pLayoutPort);
    m_onLayoutPortRemovedConnections[pLayoutPort] = pLayoutPort->removed.connect(sigc::mem_fun(this, &GuiInstance::onLayoutPortRemoved));
  }

  m_eventData.layoutEventReceived = false;
  m_eventData.vhdlEventReceived = false;
}

GuiInstance::~GuiInstance()
{
  m_onLayoutPortAddedConnection.disconnect();
  m_onVHDLPortAddedConnection.disconnect();
  for(auto &kv: m_onLayoutPortRemovedConnections)
  {
    kv.second.disconnect();
  }
}

bool GuiInstance::onBodyButtonPress(Clutter::ButtonEvent *pEvent)
{
  bool ret;
  unsigned int modifiers = pEvent->modifier_state & ALL_MODIFIERS_MASK;

  printf("GuiInstance::onBodyButtonPress\n");

  ret = GuiBlock::onBodyButtonPress(pEvent);
  g_assert(ret == HANDLED);

  /* Determine if this button press is the start of a move operation */
  //printf("instance onBodyButtonPress button = %d modstate = %d\n", pEvent->button, pEvent->modifier_state);
  m_dragIsMove = (pEvent->button == 1) &&
                 ( (modifiers == CLUTTER_CONTROL_MASK) ||
                   (modifiers == 0) );

  return HANDLED;
}

bool GuiInstance::onBodyDragged(Clutter::Event *pEvent)
{
  float handleX, handleY;

  //printf("GuiInstance::onBodyDragged\n");

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

void GuiInstance::onLayoutPortAdded(Edge edge, int position, LayoutPort *pLayoutPort)
{
  printf("GuiInstance::OnLayoutPortAdded\n");

  m_onLayoutPortRemovedConnections[pLayoutPort] = pLayoutPort->removed.connect(sigc::mem_fun(this, &GuiInstance::onLayoutPortRemoved));

  m_eventData.layoutEventReceived = true;
  m_eventData.edge = edge;
  m_eventData.position = position;
  m_eventData.pLayoutPort = pLayoutPort;

  handlePortAdded();
}

void GuiInstance::onVHDLPortAdded(VHDLPort *pVHDLPort)
{
  printf("GuiInstance::OnVHDLPortAdded\n");

  m_eventData.vhdlEventReceived = true;
  m_eventData.pVHDLPort = pVHDLPort;

  handlePortAdded();
}

void GuiInstance::onLayoutPortRemoved(Edge edge, int position, LayoutPort *pLayoutPort)
{
  printf("GuiInstance::OnLayoutPortRemoved(%s(%p))\n", pLayoutPort->getName().c_str(), pLayoutPort);

  m_eventData.edge = edge;
  m_eventData.position = position;

  m_onLayoutPortRemovedConnections[pLayoutPort].disconnect();

  removePort(m_eventData.edge, m_eventData.position);
}

