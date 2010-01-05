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

#include "gui_block.h"
#include "gui_common.h"

/*
 * Public methods
 */

GuiBlock::GuiBlock(Glib::RefPtr<Clutter::Stage> pStage, LayoutBlock *pLayoutBlock):
  m_pLayoutBlock(pLayoutBlock)
{
  std::list<LayoutBlock::PortData> *pPortList;
  std::list<LayoutBlock::PortData>::iterator it;
  LayoutPosition position;
  LayoutSize size;

  m_onPortAddedConnection = pLayoutBlock->port_added.connect(sigc::mem_fun(this, &GuiBlock::onPortAdded));

  m_pLayoutBlock->getSize(&size);
  m_pLayoutBlock->getPosition(&position);

  m_pGroup = Clutter::Group::create();
  m_pGroup->set_position(position.x, position.y);
  pStage->add_actor(m_pGroup);

  m_pBody = Clutter::Rectangle::create(Clutter::Color(0xAE, 0xFF, 0x7F, 0xFF));
  m_pBody->set_size(size.width, size.height);
  m_pBody->set_position(0, 0);
  m_pBody->set_border_width(BORDER_WIDTH);
  m_pBody->set_reactive();
  m_onBodyButtonPressConnection = m_pBody->signal_button_press_event().connect(sigc::mem_fun(*this, &GuiBlock::onBodyButtonPress));
  m_pGroup->add_actor(m_pBody);

  pPortList = m_pLayoutBlock->getPortList();
  for(it = pPortList->begin(); it != pPortList->end(); it++)
  {
    addPort(it->edge, it->position, it->pLayoutPort);
  }
}

GuiBlock::~GuiBlock()
{
  m_onPortAddedConnection.disconnect();
}

/*
 * Private methods
 */

bool GuiBlock::addPort(Edge edge, int position, LayoutPort *pLayoutPort)
{
  int x, y;

  PortData *pPortData = new PortData();
  pPortData->edge = edge;
  pPortData->position = position;
  pPortData->pActor = Tutorial::Triangle::create(PORT_COLOR);

  pPortData->pActor->set_anchor_point(LayoutPort::WIDTH / 2, LayoutPort::WIDTH / 2);
  pPortData->pActor->set_size(LayoutPort::WIDTH, LayoutPort::WIDTH);
  m_pLayoutBlock->calculatePortPosition(edge, position, &x, &y);
  pPortData->pActor->set_position(x, y);
  pPortData->pActor->set_reactive();
  pPortData->onPortButtonPressConnection = pPortData->pActor->signal_button_press_event().connect(sigc::bind<PortData *>(sigc::mem_fun(*this, &GuiBlock::onPortButtonPress), pPortData));
  m_pGroup->add_actor(pPortData->pActor);

  // If the only way to move a port is through GuiBlock, then we shouldn't subscribe to this
  //pLayoutPort->moved.connect(sigc::bind<PortData *>(sigc::mem_fun(this, &GuiBlock::onPortMoved), pPortData));
  pPortData->onPortRemovedConnection = pLayoutPort->removed.connect(sigc::bind<PortData *>(sigc::mem_fun(this, &GuiBlock::onPortRemoved), pPortData));

  m_portDataList.push_back(pPortData);
}

/*
 * Private methods
 */

bool GuiBlock::getClosestPort(bool unusedOnly, int x, int y, Edge *pEdge, int *pPosition)
{
  LayoutSize layoutSize;
  LayoutPosition layoutPosition;
  int maxNrOfPorts;
  int edge, position;
  int portX, portY;
  int distanceSquared, minDistanceSquared = -1;
  int bestMatchEdge, bestMatchPosition;

  /* Make coords relative to component position */
  m_pLayoutBlock->getPosition(&layoutPosition);
  x -= layoutPosition.x;
  y -= layoutPosition.y;

  m_pLayoutBlock->getSize(&layoutSize);

  for(edge = 0; edge < NR_OF_EDGES; edge++)
  {
    maxNrOfPorts = LayoutBlock::calculateMaxNrOfPorts( (edge == EDGE_LEFT || edge == EDGE_RIGHT) ? layoutSize.height : layoutSize.width );
    for(position = 0; position < maxNrOfPorts; position++)
    {
      if(unusedOnly && !m_pLayoutBlock->getPort((Edge)edge, position))
      {
        m_pLayoutBlock->calculatePortPosition((Edge)edge, position, &portX, &portY);
        distanceSquared = (portX - x) * (portX - x) + (portY - y) * (portY - y);
        if(distanceSquared < minDistanceSquared || minDistanceSquared == -1)
        {
          minDistanceSquared = distanceSquared;
          bestMatchEdge = edge;
          bestMatchPosition = position;
        }
      }
    }
  }

  *pEdge = (Edge)bestMatchEdge;
  *pPosition = bestMatchPosition;

  return (minDistanceSquared != -1);
}

bool GuiBlock::onBodyButtonPress(Clutter::ButtonEvent *pEvent)
{
  /* TODO: implement */
  return true;
}

void GuiBlock::onPortAdded(Edge edge, int position, LayoutPort *pLayoutPort)
{
  addPort(edge, position, pLayoutPort);
}

bool GuiBlock::onPortButtonPress(Clutter::ButtonEvent* pEvent, PortData *pPortData)
{
  /* TODO: implement */
  return true;
}

void GuiBlock::onPortMoved(Edge edge, int position, PortData *pPortData)
{
  /* TODO: implement */
}

void GuiBlock::onPortRemoved(PortData *pPortData)
{
  m_pGroup->remove_actor(pPortData->pActor);
  m_portDataList.remove(pPortData);
  delete pPortData;
}
