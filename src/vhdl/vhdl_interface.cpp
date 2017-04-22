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

void VHDLInterface::init_addGenerics(VHDLFragment *pFragment)
{
  g_assert(m_init);
  g_assert(!m_pGenerics);
  g_assert(pFragment);
  m_pGenerics = pFragment;
}

void VHDLInterface::init_addPort(VHDLPort *pPort)
{
  g_assert(m_init);

  printf("VHDLInterface(%p)::init_addPort(%s(%p))\n", this, pPort->getName().c_str(), pPort);

  m_ports.push_back(pPort);
}

VHDLPort *VHDLInterface::findPortByName(Glib::ustring name)
{
  std::list<VHDLPort *>::iterator it;

  printf("VHDLInterface(%p)::findPortByName(%s)\n", this, name.c_str());

  for(it = m_ports.begin(); it != m_ports.end(); it++)
  {
    if((*it)->getName() == name)
    {
      return *it;
    }
  }
  printf(".. not found\n");
  return NULL;
}

void VHDLInterface::addPort(VHDLPort *pPort)
{
  printf("VHDLInterface(%p)::addPort(%s(%p))\n", this, pPort->getName().c_str(), pPort);
  g_assert(find(m_ports.begin(), m_ports.end(), pPort) == m_ports.end());
  m_ports.push_back(pPort);
  port_added.emit(pPort);
}

void VHDLInterface::removePort(VHDLPort *pPort)
{
  printf("VHDLInterface(%p)::removePort(%s(%p))\n", this, pPort->getName().c_str(), pPort);
  g_assert(find(m_ports.begin(), m_ports.end(), pPort) != m_ports.end());
  m_ports.remove(pPort);
}
