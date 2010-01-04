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

#include <algorithm>

#include "vhdl_interface.h"

VHDLInterface::VHDLInterface(Glib::ustring name):
  m_name(name)
{

}

void VHDLInterface::setName(Glib::ustring name)
{
  m_name = name;
  name_changed.emit(name);
}

void VHDLInterface::addPort(VHDLPort *pPort)
{
  printf("port %s added to interface %s %p\n", pPort->getName().c_str(), m_name.c_str(), this);
  g_assert(find(m_ports.begin(), m_ports.end(), pPort) == m_ports.end());
  m_ports.push_back(pPort);
  port_added.emit(pPort);
}

void VHDLInterface::removePort(VHDLPort *pPort)
{
  printf("port %s removed from interface %s %p\n", pPort->getName().c_str(), m_name.c_str(), this);
  g_assert(find(m_ports.begin(), m_ports.end(), pPort) != m_ports.end());
  m_ports.remove(pPort);
  port_removed.emit(pPort);
}

VHDLPort *VHDLInterface::findPortByName(Glib::ustring name)
{
  std::list<VHDLPort *>::iterator it;

  printf("findPortByName %s\n", name.c_str());

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

