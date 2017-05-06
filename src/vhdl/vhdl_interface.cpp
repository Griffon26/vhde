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

#include <algorithm>

#include "vhdl_fragment.h"
#include "vhdl_interface.h"
#include "vhdl_port.h"

/*
 * Public methods
 */

VHDLInterface::VHDLInterface():
  m_init(true),
  m_pGenerics(nullptr)
{
}

void VHDLInterface::init_addGenerics(std::unique_ptr<VHDLFragment> pFragment)
{
  g_assert(m_init);
  g_assert(!m_pGenerics);
  g_assert(pFragment);
  m_pGenerics = std::move(pFragment);
}

void VHDLInterface::init_addPort(std::unique_ptr<VHDLPort> pPort)
{
  g_assert(m_init);

  printf("VHDLInterface(%p)::init_addPort(%s(%p))\n", this, pPort->getName().c_str(), pPort.get());

  m_ports.push_back(std::move(pPort));
}

VHDLPort *VHDLInterface::findPortByName(const Glib::ustring &name)
{
  printf("VHDLInterface(%p)::findPortByName(%s)\n", this, name.c_str());

  for(auto &pPort: m_ports)
  {
    if(pPort->getName() == name)
    {
      return pPort.get();
    }
  }
  printf(".. not found\n");
  return NULL;
}

const std::vector<VHDLPort *> VHDLInterface::getPortList()
{
  return stripOwnership(m_ports);
}

void VHDLInterface::addPort(std::unique_ptr<VHDLPort> pPort)
{
  VHDLPort *pRawPort = pPort.get();
  printf("VHDLInterface(%p)::addPort(%s(%p))\n", this, pPort->getName().c_str(), pRawPort);
  g_assert(find_if(m_ports.begin(), m_ports.end(), [&](std::unique_ptr<VHDLPort> &p) { return p.get() == pRawPort; }) == m_ports.end());
  m_ports.push_back(std::move(pPort));
  port_added.emit(pRawPort);
}

void VHDLInterface::removePort(VHDLPort *pPort)
{
  printf("VHDLInterface(%p)::removePort(%s(%p))\n", this, pPort->getName().c_str(), pPort);
  auto it = find_if(m_ports.begin(), m_ports.end(), [&](std::unique_ptr<VHDLPort> &p) { return p.get() == pPort; });
  m_ports.erase(it);
}

