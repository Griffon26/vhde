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
#include "vhdl_entity.h"

/*
 * Public methods
 */

VHDLComponent::VHDLComponent(Glib::ustring entityName):
  m_pEntity(NULL),
  m_unresolvedName(entityName)
{
  printf("VHDLComponent(%p)::VHDLComponent(%s)\n", this, entityName.c_str());
}

VHDLComponent::~VHDLComponent()
{
  if(m_pEntity != NULL)
  {
    m_onPortAddedConnection.disconnect();
  }
}

/*
  component INV
    port (A: in STD_LOGIC;
    F: out STD_LOGIC);
  end component;

*/
bool VHDLComponent::write(std::ostream &outStream, int indent)
{
  std::list<VHDLGeneric *>::iterator git;
  std::list<VHDLPort *>::iterator pit;
  std::string indentString(indent, ' ');

  g_assert(m_pEntity);

  outStream << indentString << "component " << m_pEntity->getName() << "\n";

  outStream << indentString << "  port (\n";
  for(pit = m_ports.begin(); pit != m_ports.end(); pit++)
  {
    if(pit != m_ports.begin())
    {
      outStream << ";\n";
    }
    (*pit)->write(outStream, indent + 4);
  }
  outStream << "\n" << indentString << "  );\n";

  outStream << indentString << "end component;\n\n";

  return true;
}

void VHDLComponent::associateEntity(VHDLEntity *pEntity)
{
  const std::list<VHDLPort *> *pPortList;
  std::list<VHDLPort *>::const_iterator it;

  g_assert(!m_init);
  g_assert(m_pEntity == NULL);
  m_pEntity = pEntity;

  g_assert(m_unresolvedName == m_pEntity->getName());

  pPortList = m_pEntity->getPortList();
  g_assert(pPortList->size() == m_ports.size());

  for(it = pPortList->begin(); it != pPortList->end(); it++)
  {
    VHDLPort *pOurPort = findPortByName((*it)->getName());

    /* Surely there's a matching port in this component */
    g_assert(pOurPort != NULL);

    /* No need to remember the connection for "removed" signals, because the
     * only reason we would no longer want to receive them is when the
     * corresponding port is destroyed (so it won't emit any further signals).
     */
    (*it)->removed.connect(sigc::bind<VHDLPort *>(sigc::mem_fun(this, &VHDLComponent::onPortRemoved), pOurPort));
  }

  m_onPortAddedConnection = m_pEntity->port_added.connect(sigc::mem_fun(this, &VHDLComponent::onPortAdded));
}

VHDLEntity *VHDLComponent::getAssociatedEntity()
{
  return m_pEntity;
}

const Glib::ustring &VHDLComponent::getName()
{
  if(!m_init)
  {
    return m_unresolvedName;
  }
  else
  {
    g_assert(m_pEntity);
    return m_pEntity->getName();
  }
}

/*
 * Private methods
 */

void VHDLComponent::onPortAdded(VHDLPort *pEntityPort)
{
  printf("VHDLComponent::onPortAdded\n");
  VHDLPort *pOurPort = new VHDLPort(*pEntityPort);

  pEntityPort->removed.connect(sigc::bind<VHDLPort *>(sigc::mem_fun(this, &VHDLComponent::onPortRemoved), pOurPort));
  addPort(pOurPort);
}

void VHDLComponent::onPortRemoved(VHDLPort *pEntityPort, VHDLPort *pOurPort)
{
  printf("VHDLComponent(%p)::onPortRemoved(%s(%p))\n", this, pOurPort->getName().c_str(), pOurPort);
  removePort(pOurPort);
  delete pOurPort;
}
