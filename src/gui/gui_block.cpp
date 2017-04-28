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

#include "gui_block.h"
#include "gui_common.h"

/*
 * Public methods
 */

GuiBlock::GuiBlock(Glib::RefPtr<Clutter::Stage> pStage, LayoutBlock *pLayoutBlock):
  m_pLayoutBlock(pLayoutBlock),
  m_pStage(pStage)
{
  std::list<LayoutBlock::PortData> *pPortList;
  std::list<LayoutBlock::PortData>::iterator it;
  LayoutPosition position;
  LayoutSize size;

  printf("GuiBlock    (%p): constructor body\n", this);

  m_pLayoutBlock->getSize(&size);
  m_pLayoutBlock->getPosition(&position);

  m_pGroup = Clutter::Group::create();
  m_pGroup->set_position(position.x, position.y);
  m_pStage->add_actor(m_pGroup);

  /* Add the body */
  m_pBody = ClutterBlock::create(Clutter::Color(0xAE, 0xFF, 0x7F, 0xFF));
  m_pBody->set_size(size.width, size.height);
  m_pBody->set_position(0, 0);
  m_pBody->set_reactive();
  m_pGroup->add_actor(m_pBody);

  /* Add the ports */
  pPortList = m_pLayoutBlock->getPortList();
  for(it = pPortList->begin(); it != pPortList->end(); it++)
  {
    printf("GuiBlock    (%p): adding layout port %p at edge %s position %d\n", this, it->pLayoutPort, EDGE_TO_NAME(it->edge), it->position);
    addPort(it->edge, it->position, it->pLayoutPort);
  }

  /* Add text */
  m_pText = ClutterScalingText::create("ctrl-scrollwheel: zoom\n"
                                       "left drag       : move\n"
                                       "ctrl-left drag  : move\n"
                                       "ctrl-right drag : resize\n"
                                       "a               : add port\n"
                                       "d<edge><pos>    : del port\n"
                                       "  edge = L|R|T|B\n"
                                       "  pos  = 0..9");
  m_pText->set_position(10, 10);
  m_pText->set_line_alignment(Pango::ALIGN_LEFT);
  m_pText->set_reactive();
  m_pGroup->add_actor(m_pText);

  m_pGroup->show_all();

  /* Connect GUI signal handlers */
  m_onBodyButtonPressConnection = m_pBody->signal_button_press_event().connect(sigc::mem_fun(this, &GuiBlock::onBodyButtonPress));
  m_onTextButtonPressConnection = m_pText->signal_button_press_event().connect(sigc::mem_fun(*this, &GuiBlock::onTextButtonPress));

  /* Connect model signal handlers */
  m_onResizedConnection = pLayoutBlock->resized.connect(sigc::mem_fun(this, &GuiBlock::onResized));
}

GuiBlock::~GuiBlock()
{
  std::list<GuiPort *>::iterator it;
  for(it = m_portList.begin(); it != m_portList.end(); it++)
  {
    delete *it;
  }
  m_portList.clear();

  m_onBodyButtonPressConnection.disconnect();
  m_onTextButtonPressConnection.disconnect();

  m_onResizedConnection.disconnect();
}

bool GuiBlock::findFreeSlot(Edge preferredEdge, int preferredPosition, Edge *pFreeEdge, int *pFreePosition)
{
  return m_pLayoutBlock->findFreeSlot(preferredEdge, preferredPosition, pFreeEdge, pFreePosition);
}

/*
 * Private methods
 */

bool GuiBlock::getClosestSlot(bool unusedOnly, int x, int y, Edge *pEdge, int *pPosition, bool considerAdditionalSlot, Edge additionalSlotEdge, int additionalSlotPosition)
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
      if( (unusedOnly && !m_pLayoutBlock->getPort((Edge)edge, position)) ||
          (considerAdditionalSlot && edge == additionalSlotEdge && position == additionalSlotPosition) )
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

  if(minDistanceSquared != -1)
  {
    *pEdge = (Edge)bestMatchEdge;
    *pPosition = bestMatchPosition;
  }

  return (minDistanceSquared != -1);
}

void GuiBlock::resizeEdge(const LayoutBlock::PortPositionMap &oldPortPositionMap, LayoutBlock::PortPositionMap *pNewPortPositionMap, int newSize)
{
  LayoutBlock::PortPositionMap::const_iterator it;

  int nrOfUnusedSpotsAllowed = m_pLayoutBlock->calculateMaxNrOfPorts(newSize) - oldPortPositionMap.size();
  g_assert(nrOfUnusedSpotsAllowed >= 0);

  int previousUsedSpot = -1;
  int nrOfSkippedSpots;
  int currentSpot;
  for(it = oldPortPositionMap.begin(); it != oldPortPositionMap.end(); it++)
  {
    currentSpot = it->first;
    nrOfSkippedSpots = (currentSpot - previousUsedSpot) - 1;
    nrOfUnusedSpotsAllowed -= nrOfSkippedSpots;

    /* Break out of the loop if the current port is at or beyond the furthest
     * position it is allowed to occupy.
     */
    if(nrOfUnusedSpotsAllowed <= 0)
    {
      break;
    }

    /* Copy this port as is, because it doesn't need to be moved */
    (*pNewPortPositionMap)[it->first] = it->second;

    previousUsedSpot = currentSpot;
  }

  /* If nrOfUnusedSpotsAllowed is negative, we need to move the current port
   * back by (-nrOfUnusedSpotsAllowed) places and put all following ports
   * adjacent to this one.
   */
  currentSpot -= -nrOfUnusedSpotsAllowed;

  for(;it != oldPortPositionMap.end(); it++)
  {
    (*pNewPortPositionMap)[currentSpot] = it->second;
    currentSpot++;
  }
}

bool GuiBlock::onPortButtonPress(Clutter::ButtonEvent* pEvent, GuiPort *pGuiPort)
{
  /* Register for motion and button release events from the stage */
  m_onDragConnection = m_pStage->signal_captured_event().connect(sigc::bind<GuiPort *>(sigc::mem_fun(*this, &GuiBlock::onPortDragged), pGuiPort));
  return HANDLED;
}

bool GuiBlock::onPortDragged(Clutter::Event *pEvent, GuiPort *pGuiPort)
{
  Edge dragToEdge;
  int dragToPosition;
  float handleX, handleY;

  if(pEvent->type == CLUTTER_MOTION)
  {
    m_pGroup->get_stage()->transform_stage_point(pEvent->motion.x, pEvent->motion.y, handleX, handleY);

    if(getClosestSlot(true, handleX, handleY, &dragToEdge, &dragToPosition, true, pGuiPort->getEdge(), pGuiPort->getPosition()) &&
       ((pGuiPort->getEdge() != dragToEdge) || (pGuiPort->getPosition() != dragToPosition)) )
    {
      m_pLayoutBlock->movePort(pGuiPort->getEdge(), pGuiPort->getPosition(), dragToEdge, dragToPosition);
    }

    return HANDLED;
  }
  else if(pEvent->type == CLUTTER_BUTTON_RELEASE)
  {
    m_onDragConnection.disconnect();
    return HANDLED;
  }
  else
  {
    return UNHANDLED;
  }
}

bool GuiBlock::onTextButtonPress(Clutter::ButtonEvent* pEvent)
{
  printf("GuiBlock::onTextButtonPress\n");
  return onBodyButtonPress(pEvent);
  return UNHANDLED;
  /* Register for motion and button release events from the stage */
  m_onDragConnection = m_pStage->signal_captured_event().connect(sigc::mem_fun(this, &GuiBlock::onTextDragged));
  return HANDLED;
}

bool GuiBlock::onTextDragged(Clutter::Event *pEvent)
{
#if 0
  float handleX, handleY;

  if(pEvent->type == CLUTTER_MOTION)
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
#endif
  {
    return UNHANDLED;
  }
}

void GuiBlock::onResized(LayoutSize layoutSize)
{
  std::list<GuiPort *>::iterator it;

  m_pBody->set_size(layoutSize.width, layoutSize.height);

  /* Update port positions */
  for(it = m_portList.begin(); it != m_portList.end(); it++)
  {
    (*it)->updatePosition();
  }
}

/*
 * Protected methods
 */

void GuiBlock::addPort(Edge edge, int position, LayoutPort *pLayoutPort)
{
  GuiPort *pGuiPort = new GuiPort(m_pGroup, edge, position, m_pLayoutBlock, pLayoutPort);

  printf("GuiBlock(%p)::addPort -> %p\n", this, pGuiPort);

  /* No need to remember the connections, because deleting GuiPort will make sure no more signals will be delivered */
  pGuiPort->button_pressed.connect(sigc::mem_fun(*this, &GuiBlock::onPortButtonPress));

  m_portList.push_back(pGuiPort);
}

void GuiBlock::removePort(Edge edge, int position)
{
  std::list<GuiPort *>::iterator it;

  for(it = m_portList.begin(); it != m_portList.end(); it++)
  {
    GuiPort *pGuiPort = *it;

    if( (pGuiPort->getEdge() == edge) &&
        (pGuiPort->getPosition() == position) )
    {
      printf("GuiBlock(%p)::removePort: %p\n", this, pGuiPort);
      delete pGuiPort;
      m_portList.erase(it);
      break;
    }
  }
}


bool GuiBlock::onBodyButtonPress(Clutter::ButtonEvent *pEvent)
{
  printf("GuiBlock::onBodyButtonPress\n");

  /* Remember the point within the object where it was picked up */
  float actorX, actorY;
  m_pGroup->get_position(actorX, actorY);

  m_pStage->transform_stage_point(pEvent->x, pEvent->y, m_initialHandleX, m_initialHandleY);

  m_bodyHandleOffsetX = m_initialHandleX - actorX;
  m_bodyHandleOffsetY = m_initialHandleY - actorY;

  m_pLayoutBlock->getSize(&m_initialSize);
  m_pLayoutBlock->getMinimumSize(&m_minimumSize);

  /* Register for motion and button release events from the stage */
  m_onDragConnection = m_pStage->signal_captured_event().connect(sigc::mem_fun(this, &GuiBlock::onBodyDragged));

  m_dragIsResize = (pEvent->button == 3) &&
                   ((pEvent->modifier_state & ALL_MODIFIERS_MASK) == CLUTTER_CONTROL_MASK);

  if(m_dragIsResize)
  {
    const LayoutBlock::PortPositionMap *pPortPositionMaps = m_pLayoutBlock->getPortPositionMaps();

    /* Make a copy of the initial port position maps, because as we resize the
     * component we always want to keep the ports as close as possible to their
     * original position. So if we resize down first shifting ports closer
     * together, resizing back up should spread them out again.
     */
    for(int edge = 0; edge < NR_OF_EDGES; edge++)
    {
      m_initialPortPositionMaps[edge] = pPortPositionMaps[edge];
    }
  }

  return HANDLED;
}

bool GuiBlock::onBodyDragged(Clutter::Event *pEvent)
{
  float handleX, handleY;

  if(pEvent->type == CLUTTER_MOTION && m_dragIsResize)
  {
    LayoutBlock::PortPositionMap newPortPositionMaps[NR_OF_EDGES];
    LayoutSize size;
    m_pLayoutBlock->getSize(&size);

    m_pStage->transform_stage_point(pEvent->motion.x, pEvent->motion.y, handleX, handleY);

    size.width  = MAX(m_minimumSize.width, m_initialSize.width  + (handleX - m_initialHandleX));
    size.height = MAX(m_minimumSize.height, m_initialSize.height + (handleY - m_initialHandleY));

    resizeEdge(m_initialPortPositionMaps[EDGE_TOP],    &newPortPositionMaps[EDGE_TOP], size.width);
    resizeEdge(m_initialPortPositionMaps[EDGE_BOTTOM], &newPortPositionMaps[EDGE_BOTTOM], size.width);

    resizeEdge(m_initialPortPositionMaps[EDGE_LEFT],  &newPortPositionMaps[EDGE_LEFT], size.height);
    resizeEdge(m_initialPortPositionMaps[EDGE_RIGHT], &newPortPositionMaps[EDGE_RIGHT], size.height);

    m_pLayoutBlock->setPortPositionMaps(newPortPositionMaps);

    m_pLayoutBlock->setSize(size);

    return HANDLED;
  }
  else if(pEvent->type == CLUTTER_BUTTON_RELEASE)
  {
    m_onDragConnection.disconnect();
    return HANDLED;
  }
  else
  {
    return UNHANDLED;
  }
}
