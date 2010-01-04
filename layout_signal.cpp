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

#include "layout_signal.h"

/*
 * Public methods
 */

LayoutSignal::LayoutSignal():
  m_pSignal(NULL)
{
}

void LayoutSignal::associateSignal(VHDLSignal *pSignal)
{
  g_assert(m_pSignal == NULL);
  g_assert(pSignal != NULL);
  m_pSignal = pSignal;
}

void LayoutSignal::write(FILE *pFile)
{

}

void LayoutSignal::connect(EndPointId endPointId, LayoutInstance *pLayoutInstance, Edge edge, int position)
{
  EndPoint &endPoint = (endPointId == BEGINNING) ? beginning : end;

  endPoint.isPort = true;
  endPoint.connected = true;
  endPoint.pLayoutInstance = pLayoutInstance;
  endPoint.edge = edge;
  endPoint.position = position;

  pLayoutInstance->calculatePortPosition(edge, position, &endPoint.offset_x, &endPoint.offset_y);

  pLayoutInstance->moved.connect(sigc::bind<EndPoint *>(sigc::mem_fun(this, &LayoutSignal::onInstanceMoved), &endPoint));
  pLayoutInstance->getPort(edge, position)->moved.connect(sigc::bind<EndPoint *>(sigc::mem_fun(this, &LayoutSignal::onPortMoved), &endPoint));
  pLayoutInstance->getPort(edge, position)->disconnected.connect(sigc::bind<EndPoint *>(sigc::mem_fun(this, &LayoutSignal::onPortDisconnected), &endPoint));
}

/*
 * Private methods
 */

void LayoutSignal::onInstanceMoved(LayoutInstance *pLayoutInstance, LayoutPosition pos, EndPoint *pEndPoint)
{
  LayoutPosition &endPointPosition = (pEndPoint == &beginning) ? m_corners.front() : m_corners.back();

  endPointPosition.x = pos.x + pEndPoint->offset_x;
  endPointPosition.y = pos.y + pEndPoint->offset_y;
}

void LayoutSignal::onPortMoved(Edge newEdge, int newPosition, EndPoint *pEndPoint)
{
  LayoutPosition *pEndPointPosition = (pEndPoint == &beginning) ? &m_corners.front() : &m_corners.back();
  LayoutPosition instancePosition   = pEndPoint->pLayoutInstance->getPosition();

  pEndPoint->pLayoutInstance->calculatePortPosition(newEdge, newPosition, &pEndPoint->offset_x, &pEndPoint->offset_y);
  pEndPointPosition->x = instancePosition.x + pEndPoint->offset_x;
  pEndPointPosition->y = instancePosition.y + pEndPoint->offset_y;
}

void LayoutSignal::onPortDisconnected(EndPoint *pEndPoint)
{
  pEndPoint->connected = false;
}
