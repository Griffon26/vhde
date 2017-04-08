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

#include "vhdl_interface.h"

/*
 * Public methods
 */

VHDLInterface::VHDLInterface():
  m_init(true)
{
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
    printf(".. %s\n", (*it)->getName().c_str());
    if((*it)->getName() == name)
    {
      return *it;
    }
  }
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
