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

#include <stdio.h>

#include "layout_signal.h"

#define ENDPOINT_TO_ENDPOINTID(pEndpoint) ((pEndPoint == &m_beginning) ? BEGINNING : END)

/*
 * Public methods
 */

LayoutSignal::LayoutSignal():
  m_pSignal(NULL)
{
  m_corners.push_back(LayoutPosition(0,0));
  m_corners.push_back(LayoutPosition(100,200));
  m_corners.push_back(LayoutPosition(200,200));
  m_corners.push_back(LayoutPosition(200,250));

  m_endPoints[BEGINNING].connected = false;
  m_endPoints[END].connected = false;
}

void LayoutSignal::associateSignal(INamedItem *pSignal)
{
  g_assert(m_pSignal == NULL);
  g_assert(pSignal != NULL);
  m_pSignal = pSignal;
}

/*
signal "mysignal" {
  from port "instance1" LEFT 0
  to   signal "signal2" 200 300
  corners {
    400 320
    800 900
  }
}
*/
void LayoutSignal::write(std::ostream &stream)
{
  std::list<LayoutPosition>::iterator it;

  stream << "signal \"" << m_pSignal->getName() << "\" {\n"
         << "  from ";
  writeEndPoint(stream, m_endPoints[BEGINNING]);
  stream << "  to   ";
  writeEndPoint(stream, m_endPoints[END]);
  stream << "  corners {\n";

  for(it = m_corners.begin(); it != m_corners.end(); it++)
  {
    stream << "    " << it->x << " " << it->y << "\n";
  }

  stream << "  }\n"
         << "}\n"
         << "\n";
}

void LayoutSignal::connect(EndPointId endPointId, LayoutInstance *pLayoutInstance, Edge edge, int position)
{
  LayoutPort *pLayoutPort;
  EndPoint &endPoint = m_endPoints[endPointId];

  endPoint.isPort = true;
  endPoint.connected = true;
  endPoint.pLayoutInstance = pLayoutInstance;
  endPoint.edge = edge;
  endPoint.position = position;

  pLayoutInstance->calculatePortPosition(edge, position, &endPoint.offset_x, &endPoint.offset_y);

  endPoint.onInstanceMovedConnection = pLayoutInstance->moved.connect(sigc::bind<EndPointId>(sigc::mem_fun(this, &LayoutSignal::onInstanceMoved), endPointId));
  endPoint.onInstanceResizedConnection = pLayoutInstance->resized.connect(sigc::bind<EndPointId>(sigc::mem_fun(this, &LayoutSignal::onInstanceResized), endPointId));

  pLayoutPort = pLayoutInstance->getPort(edge, position);
  g_assert(pLayoutPort != NULL);
  endPoint.onPortMovedConnection = pLayoutPort->moved.connect(sigc::bind<EndPointId>(sigc::mem_fun(this, &LayoutSignal::onPortMoved), endPointId));
  endPoint.onPortRemovedConnection = pLayoutPort->removed.connect(sigc::bind<EndPointId>(sigc::mem_fun(this, &LayoutSignal::onPortRemoved), endPointId));

  recalcEndPoint(endPointId);
}

const std::list<LayoutPosition> *LayoutSignal::getCorners()
{
  return &m_corners;
}

/*
 * Private methods
 */

void LayoutSignal::writeEndPoint(std::ostream &stream, const EndPoint &endPoint)
{
  if(endPoint.connected)
  {
    if(endPoint.isPort)
    {
      stream << "port \"" << endPoint.pLayoutInstance->getAssociatedVHDLInstance()->getName() << "\" " << EDGE_TO_NAME(endPoint.edge) << " " << endPoint.position << "\n";
    }
    else
    {
      stream << "signal \"" << endPoint.pSignal->getName() << "\" " << endPoint.x << " " << endPoint.y << "\n";
    }
  }
  else
  {
    stream << "unconnected\n";
  }
}

void LayoutSignal::recalcEndPoint(EndPointId endPointId)
{
  LayoutPosition instancePosition;
  LayoutPosition *pEndPointPosition;
  LayoutPosition previousPosition;

  /* Remember the previous end point position */
  pEndPointPosition = (endPointId == BEGINNING) ? &m_corners.front() : &m_corners.back();
  previousPosition = *pEndPointPosition;

  /* Get the instance position */
  m_endPoints[endPointId].pLayoutInstance->getPosition(&instancePosition);

  /* Get the offset of the port relative to the instance position */
  m_endPoints[endPointId].pLayoutInstance->calculatePortPosition(m_endPoints[endPointId].edge,
                                                                 m_endPoints[endPointId].position,
                                                                 &m_endPoints[endPointId].offset_x,
                                                                 &m_endPoints[endPointId].offset_y);

  pEndPointPosition->x = instancePosition.x + m_endPoints[endPointId].offset_x;
  pEndPointPosition->y = instancePosition.y + m_endPoints[endPointId].offset_y;

  /* Emit a signal if the endpoint has moved */
  if(previousPosition != (*pEndPointPosition))
  {
    endpoint_moved.emit(endPointId, *pEndPointPosition);
  }
}

void LayoutSignal::onInstanceMoved(LayoutPosition pos, EndPointId endPointId)
{
  LayoutPosition &endPointPosition = (endPointId == BEGINNING) ? m_corners.front() : m_corners.back();

  endPointPosition.x = pos.x + m_endPoints[endPointId].offset_x;
  endPointPosition.y = pos.y + m_endPoints[endPointId].offset_y;

  endpoint_moved.emit(endPointId, endPointPosition);
}

void LayoutSignal::onInstanceResized(LayoutSize size, EndPointId endPointId)
{
  recalcEndPoint(endPointId);
}

void LayoutSignal::onPortMoved(Edge newEdge, int newPosition, LayoutPort *pPort, EndPointId endPointId)
{
  m_endPoints[endPointId].edge = newEdge;
  m_endPoints[endPointId].position = newPosition;

  recalcEndPoint(endPointId);
}

void LayoutSignal::onPortRemoved(Edge edge, int position, LayoutPort *pLayoutPort, EndPointId endPointId)
{
  printf("LayoutSignal::onPortRemoved\n");
  m_endPoints[endPointId].onPortRemovedConnection.disconnect();

  m_endPoints[endPointId].connected = false;
  endpoint_disconnected.emit(endPointId);

  m_endPoints[endPointId].onInstanceMovedConnection.disconnect();
  m_endPoints[endPointId].onInstanceResizedConnection.disconnect();

  /* The following two aren't really necessary now that the port is being destroyed */
  m_endPoints[endPointId].onPortMovedConnection.disconnect();
  m_endPoints[endPointId].onPortRemovedConnection.disconnect();
}
