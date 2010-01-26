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

  m_beginning.connected = false;
  m_end.connected = false;
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
void LayoutSignal::write(FILE *pFile)
{
  std::list<LayoutPosition>::iterator it;

  fprintf(pFile, "signal \"%s\" {\n", m_pSignal->getName().c_str());
  fprintf(pFile, "  from ");
  writeEndPoint(pFile, m_beginning);
  fprintf(pFile, "  to   ");
  writeEndPoint(pFile, m_end);
  fprintf(pFile, "  corners {\n");

  for(it = m_corners.begin(); it != m_corners.end(); it++)
  {
    fprintf(pFile, "    %d %d\n", it->x, it->y);
  }

  fprintf(pFile, "  }\n"
                 "}\n"
                 "\n");
}

void LayoutSignal::connect(EndPointId endPointId, LayoutInstance *pLayoutInstance, Edge edge, int position)
{
  LayoutPort *pLayoutPort;
  EndPoint &endPoint = (endPointId == BEGINNING) ? m_beginning : m_end;

  endPoint.isPort = true;
  endPoint.connected = true;
  endPoint.pLayoutInstance = pLayoutInstance;
  endPoint.edge = edge;
  endPoint.position = position;

  pLayoutInstance->calculatePortPosition(edge, position, &endPoint.offset_x, &endPoint.offset_y);

  m_onInstanceMovedConnection = pLayoutInstance->moved.connect(sigc::bind<EndPoint *>(sigc::mem_fun(this, &LayoutSignal::onInstanceMoved), &endPoint));
  m_onInstanceResizedConnection = pLayoutInstance->resized.connect(sigc::bind<EndPoint *>(sigc::mem_fun(this, &LayoutSignal::onInstanceResized), &endPoint));

  pLayoutPort = pLayoutInstance->getPort(edge, position);
  g_assert(pLayoutPort != NULL);
  m_onPortMovedConnections[endPointId] = pLayoutPort->moved.connect(sigc::bind<EndPoint *>(sigc::mem_fun(this, &LayoutSignal::onPortMoved), &endPoint));
  m_onPortDisconnectedConnections[endPointId] = pLayoutPort->disconnected.connect(sigc::bind<EndPoint *>(sigc::mem_fun(this, &LayoutSignal::onPortDisconnected), &endPoint));

  recalcEndPoint(&endPoint);
}

const std::list<LayoutPosition> *LayoutSignal::getCorners()
{
  return &m_corners;
}

/*
 * Private methods
 */

void LayoutSignal::writeEndPoint(FILE *pFile, const EndPoint &endPoint)
{
  if(endPoint.connected)
  {
    if(endPoint.isPort)
    {
      fprintf(pFile, "port \"%s\" %s %d\n", endPoint.pLayoutInstance->getAssociatedVHDLInstance()->getName().c_str(),
                                            EDGE_TO_NAME(endPoint.edge),
                                            endPoint.position);
    }
    else
    {
      fprintf(pFile, "signal \"%s\" %d %d\n", endPoint.pSignal->getName().c_str(),
                                              endPoint.x,
                                              endPoint.y);
    }
  }
  else
  {
    fprintf(pFile, "unconnected\n");
  }
}

void LayoutSignal::recalcEndPoint(EndPoint *pEndPoint)
{
  LayoutPosition instancePosition;
  LayoutPosition *pEndPointPosition;
  LayoutPosition previousPosition;

  /* Remember the previous end point position */
  pEndPointPosition = (pEndPoint == &m_beginning) ? &m_corners.front() : &m_corners.back();
  previousPosition = *pEndPointPosition;

  /* Get the instance position */
  pEndPoint->pLayoutInstance->getPosition(&instancePosition);

  /* Get the offset of the port relative to the instance position */
  pEndPoint->pLayoutInstance->calculatePortPosition(pEndPoint->edge, pEndPoint->position, &pEndPoint->offset_x, &pEndPoint->offset_y);

  pEndPointPosition->x = instancePosition.x + pEndPoint->offset_x;
  pEndPointPosition->y = instancePosition.y + pEndPoint->offset_y;

  /* Emit a signal if the endpoint has moved */
  if(previousPosition != (*pEndPointPosition))
  {
    endpoint_moved.emit(ENDPOINT_TO_ENDPOINTID(pEndPoint), *pEndPointPosition);
  }
}

void LayoutSignal::onInstanceMoved(LayoutPosition pos, EndPoint *pEndPoint)
{
  LayoutPosition &endPointPosition = (pEndPoint == &m_beginning) ? m_corners.front() : m_corners.back();

  endPointPosition.x = pos.x + pEndPoint->offset_x;
  endPointPosition.y = pos.y + pEndPoint->offset_y;

  endpoint_moved.emit(ENDPOINT_TO_ENDPOINTID(pEndPoint), endPointPosition);
}

void LayoutSignal::onInstanceResized(LayoutSize size, EndPoint *pEndPoint)
{
  recalcEndPoint(pEndPoint);
}

void LayoutSignal::onPortMoved(Edge newEdge, int newPosition, EndPoint *pEndPoint)
{
  pEndPoint->edge = newEdge;
  pEndPoint->position = newPosition;

  recalcEndPoint(pEndPoint);
}

void LayoutSignal::onPortDisconnected(EndPoint *pEndPoint)
{
  m_onPortDisconnectedConnections[ENDPOINT_TO_ENDPOINTID(pEndPoint)].disconnect();

  pEndPoint->connected = false;
  endpoint_disconnected.emit(ENDPOINT_TO_ENDPOINTID(pEndPoint));
}
