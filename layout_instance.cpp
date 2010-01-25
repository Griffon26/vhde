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

#include "layout_instance.h"

/*
 * Public methods
 */
LayoutInstance::LayoutInstance(LayoutComponent *pComponent):
  m_pComponent(pComponent),
  m_pVHDLInstance(NULL)
{
  m_onPortAddedConnection = m_pComponent->port_added.connect(sigc::mem_fun(this, &LayoutInstance::onPortAdded));
  m_onPortRemovedConnection = m_pComponent->port_removed.connect(sigc::mem_fun(this, &LayoutInstance::onPortRemoved));
}

LayoutInstance::~LayoutInstance()
{
  m_onPortAddedConnection.disconnect();
  m_onPortRemovedConnection.disconnect();
}

void LayoutInstance::setPosition(const LayoutPosition &pos)
{
  m_position = pos;
  moved.emit(pos);
}

void LayoutInstance::associateInstance(INamedItem *pVHDLInstance)
{
  g_assert(!m_init);
  g_assert(m_pVHDLInstance == NULL);
  g_assert(pVHDLInstance != NULL);
  m_pVHDLInstance = pVHDLInstance;
}

INamedItem *LayoutInstance::getAssociatedVHDLInstance()
{
  return m_pVHDLInstance;
}

/*
instance "blaat" {
  position 100 200
  size 200 300
  ports {
    LEFT 0 "dinges"
    LEFT 1 "blaat"
  }
}

*/
void LayoutInstance::write(FILE *pFile)
{
  int edge;
  std::map<int, LayoutPort *>::iterator it;

  fprintf(pFile, "instance \"%s\" {\n", m_pVHDLInstance->getName().c_str());
  fprintf(pFile, "  position %d %d\n", m_position.x, m_position.y);
  fprintf(pFile, "  size %d %d\n", m_size.width, m_size.height);
  fprintf(pFile, "  ports {\n");

  for(edge = 0; edge < NR_OF_EDGES; edge++)
  {
    for(it = m_ports[edge].begin(); it != m_ports[edge].end(); it++)
    {
      fprintf(pFile, "    %s %d \"%s\"\n", EDGE_TO_NAME(edge), it->first, it->second->getAssociatedVHDLPort()->getName().c_str());
    }
  }

  fprintf(pFile, "  }\n"
                 "}\n"
                 "\n");
}

/*
 * Private methods
 */
#if 0
bool LayoutInstance::findPort(const VHDLPort *pPort, Edge *pEdge, int *pPosition)
{
  int edge;
  std::map<int, LayoutPort *>::iterator it;
  bool portFound;

  portFound = false;
  for(edge = 0; !portFound && edge < NR_OF_EDGES; edge++)
  {
    for(it = m_ports[edge].begin(); !portFound && it != m_ports[edge].end(); it++)
    {
      if(it->second->getAssociatedPort() == pPort)
      {
        *pEdge = (Edge)edge;
        *pPosition = it->first;
        portFound = true;
      }
    }
  }
  return portFound;
}

void LayoutInstance::onSignalDisassociated(VHDLSignal *pSignal, VHDLPort *pPort)
{
  Edge edge;
  int position;
  bool portFound;


  portFound = findPort(pPort, &edge, &position);
  g_assert(portFound);

  printf("LayoutInstance(0x%p) disconnecting signal at edge %d position %d\n", this, edge, position);

  m_ports[edge][position]->disconnected.emit();
}
#endif

void LayoutInstance::onPortAdded(int actionId, Edge edge, int position, LayoutPort *pLayoutPort)
{
  Edge freeEdge;
  int freePosition;

  printf("LayoutInstance::onPortAdded\n");
  LayoutPort *pNewLayoutPort = new LayoutPort();
  if(!findFreeSlot(edge, position, &freeEdge, &freePosition))
  {
    /* Resize the instance in vertical direction to make room for at least one more set of ports */
    LayoutSize layoutSize = m_size;

    layoutSize.height = calculateMinEdgeLength(m_ports[EDGE_LEFT].size() + 1);

    setSize(layoutSize);
  }

  if(findFreeSlot(edge, position, &freeEdge, &freePosition))
  {
    addPort(actionId, freeEdge, freePosition, pNewLayoutPort);
  }
}

void LayoutInstance::onPortRemoved(int actionId, Edge edge, int position, LayoutPort *pLayoutPort)
{
  /* Replace the input data about the port of the component with data about our port */
  pLayoutPort = findPortByName(pLayoutPort->getName(), &edge, &position);

  printf("found port %p at edge %s pos %d\n", pLayoutPort, EDGE_TO_NAME(edge), position);
  printf("LayoutInstance(%p)::onPortRemoved(%p)\n", this, pLayoutPort);
  g_assert(pLayoutPort != NULL);
  removePort(actionId, pLayoutPort);
  delete pLayoutPort;
}
