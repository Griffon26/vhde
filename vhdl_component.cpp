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

#include "vhdl_component.h"

/*
 * Public methods
 */

VHDLComponent::VHDLComponent(Glib::ustring name):
  VHDLInterface(name)
{
}

VHDLComponent::~VHDLComponent()
{
  if(m_pEntity != NULL)
  {
    m_onNameChangedConnection.disconnect();
    m_onPortRemovedConnection.disconnect();
    m_onPortAddedConnection.disconnect();
  }
}

/*
  component INV
    port (A: in STD_LOGIC;
    F: out STD_LOGIC);
  end component;

*/
bool VHDLComponent::write(FILE *pFile, int indent)
{
  std::list<VHDLGeneric *>::iterator git;
  std::list<VHDLPort *>::iterator pit;

  fprintf(pFile, "%*scomponent %s\n", indent, "", m_name.c_str());

  fprintf(pFile, "%*sport (\n", indent + 2, "");
  for(pit = m_ports.begin(); pit != m_ports.end(); pit++)
  {
    (*pit)->write(pFile, indent + 4);
  }
  fprintf(pFile, "%*s)\n", indent + 2, "");

  fprintf(pFile, "%*send component\n\n", indent, "");

  return true;
}

void VHDLComponent::associateEntity(VHDLEntity *pEntity)
{
  g_assert(m_pEntity == NULL);
  m_pEntity = pEntity;

  m_onNameChangedConnection = m_pEntity->name_changed.connect(sigc::mem_fun(this, &VHDLComponent::onNameChanged));
  m_onPortAddedConnection = m_pEntity->port_added.connect(sigc::mem_fun(this, &VHDLComponent::onPortAdded));
  m_onPortRemovedConnection = m_pEntity->port_removed.connect(sigc::mem_fun(this, &VHDLComponent::onPortRemoved));
}

/*
 * Private methods
 */

void VHDLComponent::onNameChanged(Glib::ustring newName)
{
  m_name = newName;
}

void VHDLComponent::onPortAdded(VHDLPort *pPort)
{
  VHDLPort *pNewPort = new VHDLPort(*pPort);
  addPort(pNewPort);
}

void VHDLComponent::onPortRemoved(VHDLPort *pPort)
{
  removePort(findPortByName(pPort->getName()));
}
