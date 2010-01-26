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

#include "gui_port.h"
#include "vhdl_port.h"

/*
 * Public methods
 */

GuiPort::GuiPort(Glib::RefPtr<Clutter::Group> pGroup, Edge edge, int position, LayoutBlock *pLayoutBlock, LayoutPort *pLayoutPort):
  m_pGroup(pGroup),
  m_edge(edge),
  m_position(position),
  m_pLayoutBlock(pLayoutBlock)
{
  m_pActor = ClutterPort::create(PORT_COLOR);
  m_pActor->setDirection(static_cast<VHDLPort *>(pLayoutPort->getAssociatedVHDLPort())->getDirection());

  m_pActor->set_anchor_point(LayoutPort::WIDTH / 2, LayoutPort::WIDTH / 2);
  m_pActor->set_size(LayoutPort::WIDTH, LayoutPort::WIDTH);
  m_pActor->set_reactive();
  m_pGroup->add_actor(m_pActor);

  updatePosition();

  m_onPortButtonPressConnection = m_pActor->signal_button_press_event().connect(sigc::mem_fun(this, &GuiPort::onPortButtonPress));

  m_onPortMovedConnection = pLayoutPort->moved.connect(sigc::mem_fun(this, &GuiPort::onPortMoved));
  m_onBlockResizedConnection = pLayoutBlock->resized.connect(sigc::mem_fun(this, &GuiPort::onBlockResized));
}

GuiPort::~GuiPort()
{
  m_onPortButtonPressConnection.disconnect();

  m_onPortMovedConnection.disconnect();
  m_onBlockResizedConnection.disconnect();

  m_pGroup->remove_actor(m_pActor);
  printf("GuiPort::~GuiPort()\n");
}

void GuiPort::updatePosition()
{
  int x, y;
  m_pLayoutBlock->calculatePortPosition(m_edge, m_position, &x, &y);
  m_pActor->set_position(x, y);
  m_pActor->setOrientation(edgeToOrientation(m_edge));
}

/*
 * Private methods
 */

bool GuiPort::onPortButtonPress(Clutter::ButtonEvent* pEvent)
{
  button_pressed.emit(pEvent, this);
  return HANDLED;
}

void GuiPort::onPortMoved(Edge edge, int position)
{
  m_edge = edge;
  m_position = position;

  updatePosition();
}

void GuiPort::onBlockResized(LayoutSize layoutSize)
{
  updatePosition();
}

ClutterPort::Orientation GuiPort::edgeToOrientation(Edge edge)
{
  return (edge == EDGE_LEFT)  ? ClutterPort::EAST :
         (edge == EDGE_TOP)   ? ClutterPort::SOUTH :
         (edge == EDGE_RIGHT) ? ClutterPort::WEST :
                                ClutterPort::NORTH;
}
