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
#include "vhdl_instance.h"

GuiInstance::GuiInstance(Glib::RefPtr<Clutter::Stage> pStage, LayoutInstance *pLayoutInstance):
  GuiBlock(pStage, pLayoutInstance),
  m_dragIsMove(false)
{
  std::list<GuiPort *>::iterator it;

  printf("GuiInstance (%p): constructor body\n", this);

  m_onLayoutPortAddedConnection = pLayoutInstance->port_added.connect(sigc::mem_fun(this, &GuiInstance::onLayoutPortAdded));

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
}

GuiInstance::~GuiInstance()
{
  m_onLayoutPortAddedConnection.disconnect();
  for(auto &kv: m_onLayoutPortRemovedConnections)
  {
    kv.second.disconnect();
  }
}

void GuiInstance::discard()
{
  LayoutInstance *pLayoutInstance = static_cast<LayoutInstance *>(m_pLayoutBlock);
  VHDLInstance *pVHDLInstance = static_cast<VHDLInstance *>(pLayoutInstance->getAssociatedVHDLInstance());

  /* This will cause the architectures that own the objects to delete it */
  pLayoutInstance->discard();
  pVHDLInstance->discard();
}

void GuiInstance::startMove()
{
  printf("GuiInstance::startMove\n");
  static_cast<LayoutInstance *>(m_pLayoutBlock)->getPosition(&m_initialPosition);
}

void GuiInstance::updateMove(int offsetX, int offsetY)
{
  printf("GuiInstance::updateMove\n");
  LayoutPosition newPosition(m_initialPosition.x + offsetX,
                             m_initialPosition.y + offsetY);
  static_cast<LayoutInstance *>(m_pLayoutBlock)->setPosition(newPosition);
  m_pGroup->set_position(newPosition.x, newPosition.y);
}

void GuiInstance::finishMove()
{
  printf("GuiInstance::finishMove\n");
}

void GuiInstance::onLayoutPortAdded(Edge edge, int position, LayoutPort *pLayoutPort)
{
  printf("GuiInstance::OnLayoutPortAdded\n");

  m_onLayoutPortRemovedConnections[pLayoutPort] = pLayoutPort->removed.connect(sigc::mem_fun(this, &GuiInstance::onLayoutPortRemoved));

  addPort(edge, position, pLayoutPort);
}

void GuiInstance::onLayoutPortRemoved(Edge edge, int position, LayoutPort *pLayoutPort)
{
  printf("GuiInstance::OnLayoutPortRemoved(%s(%p))\n", pLayoutPort->getName().c_str(), pLayoutPort);

  m_onLayoutPortRemovedConnections[pLayoutPort].disconnect();
  m_onLayoutPortRemovedConnections.erase(pLayoutPort);

  removePort(edge, position);
}

