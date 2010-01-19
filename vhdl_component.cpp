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
#include "vhdl_entity.h"

/*
 * Public methods
 */

VHDLComponent::VHDLComponent():
  m_pEntity(NULL)
{
}

VHDLComponent::~VHDLComponent()
{
  if(m_pEntity != NULL)
  {
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

  g_assert(m_pEntity);

  fprintf(pFile, "%*scomponent %s\n", indent, "", m_pEntity->getName().c_str());

  fprintf(pFile, "%*sport (\n", indent + 2, "");
  for(pit = m_ports.begin(); pit != m_ports.end(); pit++)
  {
    (*pit)->write(pFile, indent + 4);
  }
  fprintf(pFile, "%*s)\n", indent + 2, "");

  fprintf(pFile, "%*send component;\n\n", indent, "");

  return true;
}

void VHDLComponent::associateEntity(VHDLEntity *pEntity)
{
  g_assert(!m_init);
  g_assert(m_pEntity == NULL);
  m_pEntity = pEntity;

  m_onPortAddedConnection = m_pEntity->port_added.connect(sigc::mem_fun(this, &VHDLComponent::onPortAdded));
  m_onPortRemovedConnection = m_pEntity->port_removed.connect(sigc::mem_fun(this, &VHDLComponent::onPortRemoved));
}

VHDLEntity *VHDLComponent::getAssociatedEntity()
{
  return m_pEntity;
}

const Glib::ustring &VHDLComponent::getName()
{
  g_assert(m_pEntity);
  return m_pEntity->getName();
}

/*
 * Private methods
 */

void VHDLComponent::onPortAdded(int actionId, VHDLPort *pPort)
{
  VHDLPort *pNewPort = new VHDLPort(*pPort);
  addPort(actionId, pNewPort);
}

void VHDLComponent::onPortRemoved(int actionId, VHDLPort *pPort)
{
  removePort(actionId, findPortByName(pPort->getName()));
}
