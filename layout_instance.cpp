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
LayoutInstance::LayoutInstance():
  m_pInstance(NULL)
{
}

LayoutInstance::~LayoutInstance()
{
  if(m_pInstance)
  {
    m_onSignalDisassociatedConnection.disconnect();
    m_onPortRemovedConnection.disconnect();
  }
}

void LayoutInstance::setPosition(const LayoutPosition &pos)
{
  m_position = pos;
  moved.emit(this, pos);
}

void LayoutInstance::associateInstance(VHDLInstance *pInstance)
{
  g_assert(m_pInstance == NULL);
  g_assert(pInstance != NULL);
  m_pInstance = pInstance;

  m_onSignalDisassociatedConnection = m_pInstance->signal_disassociated.connect(sigc::mem_fun(this, &LayoutInstance::onSignalDisassociated));
  m_onPortRemovedConnection = m_pInstance->port_removed.connect(sigc::mem_fun(this, &LayoutInstance::onPortRemoved));
}

VHDLInstance *LayoutInstance::getAssociatedInstance()
{
  return m_pInstance;
}

/*

position 100 200
size 200 300
ports {
  LEFT 0 "dinges"
  LEFT 1 "blaat"

  "dinges" LEFT 0
  "something" TOP 2
}

*/
void LayoutInstance::write(FILE *pFile)
{
  int edge;
  std::map<int, LayoutPort *>::iterator it;

  fprintf(pFile, "position %d %d\n", m_position.x, m_position.y);
  fprintf(pFile, "size %d %d\n", m_size.width, m_size.height);
  fprintf(pFile, "ports {\n");

  for(edge = 0; edge < NR_OF_EDGES; edge++)
  {
    for(it = m_ports[edge].begin(); it != m_ports[edge].end(); it++)
    {
      fprintf(pFile, "  %s %d \"%s\"\n", EDGE_TO_NAME(edge), it->first, it->second->getAssociatedPort()->getName().c_str());
    }
  }

  fprintf(pFile, "}\n");
}

/*
 * Private methods
 */
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

void LayoutInstance::onPortRemoved(VHDLPort *pPort)
{
  Edge edge;
  int position;
  bool portFound;

  portFound = findPort(pPort, &edge, &position);
  g_assert(portFound);

  printf("LayoutInstance(%p) removing port at edge %d position %d\n", this, edge, position);

  m_ports[edge][position]->disconnected.emit();
  m_ports[edge][position]->removed.emit();
  m_ports[edge].erase(position);
}
