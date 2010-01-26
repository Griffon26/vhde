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

#include "vhdl_component.h"
#include "vhdl_instance.h"

/*
 * Public methods
 */

VHDLInstance::VHDLInstance(Glib::ustring name, VHDLComponent *pComponent):
  m_name(name),
  m_pComponent(pComponent)
{
  m_onPortRemovedConnection = m_pComponent->port_removed.connect(sigc::mem_fun(this, &VHDLInstance::onPortRemoved));
}

VHDLInstance::~VHDLInstance()
{
  m_onPortRemovedConnection.disconnect();
}

void VHDLInstance::associateSignalWithPort(VHDLSignal *pSignal, VHDLPort *pPort)
{
  m_portMap[pPort] = pSignal;
}

void VHDLInstance::disassociateSignalWithPort(VHDLSignal *pSignal, VHDLPort *pPort)
{
  std::map<VHDLPort *, VHDLSignal *>::iterator it;
  it = m_portMap.find(pPort);

  g_assert(it != m_portMap.end());
  g_assert(it->second == pSignal);
  m_portMap.erase(it);

  signal_disassociated.emit(pSignal, pPort);
}

void VHDLInstance::disassociateSignal(VHDLSignal *pSignal)
{
  std::map<VHDLPort *, VHDLSignal *>::iterator it, prevIt;
  VHDLPort *pPort;

  /* Remove any port associations with this signal */
  for(it = m_portMap.begin(); it != m_portMap.end();)
  {
    prevIt = it++;

    if(prevIt->second == pSignal)
    {
      pPort = prevIt->first;
      m_portMap.erase(prevIt);
      signal_disassociated.emit(pSignal, pPort);
    }
  }
}

bool VHDLInstance::write(FILE *pFile, int indent)
{
  std::list<VHDLPort *>::const_iterator pit;
  const std::list<VHDLPort *> &ports = m_pComponent->getPorts();
  std::map<VHDLPort *, VHDLSignal *>::const_iterator mit;

  fprintf(pFile, "%*s%s: %s\n"
                 "%*sport map (\n", indent, "", m_name.c_str(), m_pComponent->getName().c_str(),
                                    indent, "");

  for(pit = ports.begin(); pit != ports.end(); pit++)
  {
    mit = m_portMap.find(*pit);
    fprintf(pFile, "%s%*s%s => %s", (pit == ports.begin()) ? "" : ",\n", indent + 2, "",
                                    (*pit)->getName().c_str(),
                                    (mit == m_portMap.end()) ? "open" : mit->second->getName().c_str());
  }

  fprintf(pFile, "\n%*s);\n", indent, "");

  return true;
}

/*
 * Private methods
 */

void VHDLInstance::onPortRemoved(int actionId, VHDLPort *pPort)
{
  printf("VHDLInstance(%p)::onPortRemoved(%s)\n", this, pPort->getName().c_str());
  m_portMap.erase(pPort);
  port_removed.emit(actionId, pPort);
}
