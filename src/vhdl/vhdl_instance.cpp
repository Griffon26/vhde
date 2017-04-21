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
#include "vhdl_port.h"
#include "vhdl_signal.h"

/*
 * Public methods
 */

VHDLInstance::VHDLInstance(Glib::ustring name, VHDLComponent *pComponent):
  m_name(name),
  m_pComponent(pComponent)
{
}

VHDLInstance::~VHDLInstance()
{
}

void VHDLInstance::connectSignalToPort(VHDLSignal *pSignal, VHDLPort *pPort)
{
  MapEntry mapEntry;
  g_assert(pSignal != nullptr);
  g_assert(pPort != nullptr);
  mapEntry.onPortRemovedConnection = pPort->removed.connect(sigc::mem_fun(this, &VHDLInstance::onPortRemoved));
  mapEntry.onSignalRemovedConnection = pSignal->removed.connect(sigc::bind<VHDLPort *>(sigc::mem_fun(this, &VHDLInstance::onSignalRemoved), pPort));
  mapEntry.pSignal = pSignal;

  m_portMap[pPort] = mapEntry;
}

void VHDLInstance::disconnectSignalFromPort(VHDLSignal *pSignal, VHDLPort *pPort)
{
  std::map<VHDLPort *, MapEntry>::iterator it;
  it = m_portMap.find(pPort);

  g_assert(it != m_portMap.end());
  g_assert(it->second.pSignal == pSignal);

  it->second.onPortRemovedConnection.disconnect();
  it->second.onSignalRemovedConnection.disconnect();

  m_portMap.erase(it);
}

#if 0 // not needed?
void VHDLInstance::disconnectSignal(VHDLSignal *pSignal)
{
  std::map<VHDLPort *, MapEntry>::iterator it, prevIt;

  /* Remove any port associations with this signal */
  for(it = m_portMap.begin(); it != m_portMap.end();)
  {
    prevIt = it++;

    if(prevIt->second.pSignal == pSignal)
    {
      prevIt->second.onPortRemovedConnection.disconnect();
      prevIt->second.onSignalRemovedConnection.disconnect();
      m_portMap.erase(prevIt);
    }
  }
}
#endif

bool VHDLInstance::write(std::ostream &outStream, int indent)
{
  std::list<VHDLPort *>::const_iterator pit;
  const std::list<VHDLPort *> *pPorts = m_pComponent->getPortList();
  std::map<VHDLPort *, MapEntry>::const_iterator mit;
  std::string indentString(indent, ' ');

  outStream << indentString << m_name << ": " << m_pComponent->getName() << "\n"
            << indentString << "port map (\n";

  for(pit = pPorts->begin(); pit != pPorts->end(); pit++)
  {
    mit = m_portMap.find(*pit);
    if(pit != pPorts->begin()) outStream << ",\n";
    outStream << indentString << "  " << (*pit)->getName() << " => ";
    if(mit == m_portMap.end())
    {
      outStream << "open";
    }
    else
    {
      outStream << mit->second.pSignal->getName();
    }
  }

  outStream << "\n" << indentString << ");\n";

  return true;
}

std::vector<std::pair<VHDLPort *, VHDLSignal *>> *VHDLInstance::getPortsAndSignals()
{
  auto pPortsAndSignals = new std::vector<std::pair<VHDLPort *, VHDLSignal *>>();

  for(auto &kv: m_portMap)
  {
    pPortsAndSignals->push_back(std::make_pair(kv.first, kv.second.pSignal));
  }
  return pPortsAndSignals;
}

/*
 * Private methods
 */

void VHDLInstance::onPortRemoved(VHDLPort *pPort)
{
  printf("VHDLInstance(%p)::onPortRemoved(%s)\n", this, pPort->getName().c_str());
  m_portMap[pPort].onSignalRemovedConnection.disconnect();
  m_portMap[pPort].onPortRemovedConnection.disconnect();  // not really needed, because the port is being destroyed
  m_portMap.erase(pPort);
}

void VHDLInstance::onSignalRemoved(VHDLSignal *pSignal, VHDLPort *pPort)
{
  printf("VHDLInstance(%p)::onSignalRemoved(%s)\n", this, pSignal->getName().c_str());
  g_assert(m_portMap[pPort].pSignal == pSignal);
  m_portMap[pPort].onSignalRemovedConnection.disconnect();  // not really needed, because the signal is being destroyed
  m_portMap[pPort].onPortRemovedConnection.disconnect();
  m_portMap.erase(pPort);
}
