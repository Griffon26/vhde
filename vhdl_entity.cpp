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

#include "vhdl_entity.h"

VHDLEntity::VHDLEntity(Glib::ustring name):
  m_name(name)
{

}

void VHDLEntity::setName(Glib::ustring name)
{
  m_name = name;
  name_changed.emit(name);
}

const Glib::ustring &VHDLEntity::getName()
{
  return m_name;
}


VHDLPort *VHDLEntity::createPort(Direction dir, const Glib::ustring &name)
{
  printf("VHDLEntity::createPort(%s, %s)\n", DIR_TO_NAME(dir), name.c_str());
  VHDLPort *pVHDLPort = new VHDLPort(name);
  pVHDLPort->setDirection(dir);
  addPort(pVHDLPort);
  return pVHDLPort;
}

void VHDLEntity::destroyPort(VHDLPort *pPort)
{
  printf("VHDLEntity(%p)::destroyPort(%s)\n", this, pPort->getName().c_str());
  removePort(pPort);
  delete pPort;
}

/*

  entity INV is
    port (A: in STD_LOGIC;
    F: out STD_LOGIC);
  end INV;

*/
bool VHDLEntity::write(FILE *pFile, int indent)
{
  std::list<VHDLGeneric *>::iterator git;
  std::list<VHDLPort *>::iterator pit;

  fprintf(pFile, "%*sentity %s is\n", indent, "", m_name.c_str());

  if(m_ports.size() > 0)
  {
    fprintf(pFile, "%*sport (\n", indent + 2, "");
    for(pit = m_ports.begin(); pit != m_ports.end(); pit++)
    {
      if(pit != m_ports.begin())
      {
        fprintf(pFile, ";\n");
      }
      (*pit)->write(pFile, indent + 4);
    }
    fprintf(pFile, "\n%*s);\n", indent + 2, "");
  }

  fprintf(pFile, "%*send %s;\n\n", indent, "", m_name.c_str());

  return true;
}
