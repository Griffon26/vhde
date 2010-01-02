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

#include "vhdl_instance.h"

VHDLInstance::VHDLInstance(Glib::ustring name, VHDLInterface *pComponent):
  m_name(name),
  m_pComponent(pComponent)
{
}

void VHDLInstance::addPortMapSignal(VHDLSignal *pSignal)
{
  m_portMap.push_back(pSignal);
}

bool VHDLInstance::write(FILE *pFile, int indent)
{
  std::list<VHDLSignal *>::iterator it;

  fprintf(pFile, "%*s%s: %s port map (", indent, "", m_name.c_str(), m_pComponent->getName().c_str());

  for(it = m_portMap.begin(); it != m_portMap.end(); it++)
  {
    fprintf(pFile, "%s%s", (it == m_portMap.begin()) ? "" : ", ", (*it)->getName().c_str());
  }

  fprintf(pFile, ");\n");

  return true;
}

