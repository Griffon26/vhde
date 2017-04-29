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

#include "i_named_item.h"
#include "layout_component.h"
#include "layout_instance.h"
#include "layout_port.h"

/*
 * Public methods
 */
LayoutInstance::LayoutInstance():
  m_pComponent(NULL),
  m_pVHDLInstance(NULL)
{
}

LayoutInstance::~LayoutInstance()
{
  m_onPortAddedConnection.disconnect();
}

void LayoutInstance::setPosition(const LayoutPosition &pos)
{
  m_position = pos;
  moved.emit(pos);
}

void LayoutInstance::associateLayoutComponent(LayoutComponent *pComponent)
{
  const std::list<PortData> *pPortList;
  std::list<PortData>::const_iterator it;

  g_assert(m_pComponent == NULL);
  m_pComponent = pComponent;

  pPortList = m_pComponent->getPortList();
  g_assert(pPortList->size() == (m_ports[EDGE_LEFT].size() +
                                 m_ports[EDGE_RIGHT].size() +
                                 m_ports[EDGE_TOP].size() +
                                 m_ports[EDGE_BOTTOM].size()));

  for(it = pPortList->begin(); it != pPortList->end(); it++)
  {
    Edge edge;
    int position;
    LayoutPort *pOurPort = findPortByName(it->pLayoutPort->getName(), &edge, &position);

    /* Surely there's a matching port in this instance */
    g_assert(pOurPort != NULL);

    /* No need to remember the connection for "removed" signals, because the
     * only reason we would no longer want to receive them is when the
     * corresponding port is destroyed (so it won't emit any further signals).
     */
    it->pLayoutPort->removed.connect(sigc::bind<LayoutPort *>(sigc::mem_fun(this, &LayoutInstance::onPortRemoved), pOurPort));
  }

  m_onPortAddedConnection = m_pComponent->port_added.connect(sigc::mem_fun(this, &LayoutInstance::onPortAdded));
}

void LayoutInstance::associateVHDLInstance(INamedItem *pVHDLInstance)
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
void LayoutInstance::write(std::ostream &stream, int indent)
{
  std::map<int, LayoutPort *>::iterator it;
  Glib::ustring indentString(indent, ' ');

  stream << indentString << "instance \"" << m_pVHDLInstance->getName() << "\" {\n"
         << indentString << "  position " << m_position.x << " " << m_position.y << "\n"
         << indentString << "  size " << m_size.width << " " << m_size.height << "\n"
         << indentString << "  ports {\n";

  for(auto pPort: m_portOrder)
  {
    auto edgeAndPosition = pPort->getLocation();
    stream << indentString << "    \"" << pPort->getAssociatedVHDLPort()->getName() << "\" " << EDGE_TO_NAME(edgeAndPosition.first) << " " << edgeAndPosition.second << "\n";
  }

  stream << indentString << "  }\n"
         << indentString << "}\n"
         << "\n";
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

void LayoutInstance::onPortAdded(Edge edge, int position, LayoutPort *pLayoutPort)
{
  Edge freeEdge;
  int freePosition;

  printf("LayoutInstance::onPortAdded\n");

  auto pOurLayoutPort = std::make_unique<LayoutPort>();

  pLayoutPort->removed.connect(sigc::bind<LayoutPort *>(sigc::mem_fun(this, &LayoutInstance::onPortRemoved), pOurLayoutPort.get()));

  if(!findFreeSlot(edge, position, &freeEdge, &freePosition))
  {
    /* Resize the instance in vertical direction to make room for at least one more set of ports */
    LayoutSize layoutSize = m_size;

    layoutSize.height = calculateMinEdgeLength(m_ports[EDGE_LEFT].size() + 1);

    setSize(layoutSize);
  }

  if(findFreeSlot(edge, position, &freeEdge, &freePosition))
  {
    addPort(freeEdge, freePosition, std::move(pOurLayoutPort));
  }
}

void LayoutInstance::onPortRemoved(Edge edge, int position, LayoutPort *pComponentLayoutPort, LayoutPort *pOurLayoutPort)
{
  printf("LayoutInstance(%p)::onPortRemoved(%p)\n", this, pOurLayoutPort);
  removePort(pOurLayoutPort);
}
