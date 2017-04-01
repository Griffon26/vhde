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

#include <iostream>

#include "vhdl_architecture.h"

/*
 * Public methods
 */

VHDLArchitecture::VHDLArchitecture(Glib::ustring name):
  m_init(true),
  m_name(name),
  m_pEntity(NULL)
{
  std::cout << "Creating architecture with name " << m_name << " at address " << this << std::endl;
}

VHDLArchitecture::~VHDLArchitecture()
{
  std::cout << "Destroying architecture with name " << m_name << " at address " << this << std::endl;
}

void VHDLArchitecture::init_addSignal(VHDLSignal *pSignal)
{
  g_assert(m_init);
  m_signals.push_back(pSignal);
}

void VHDLArchitecture::init_addInstance(VHDLInstance *pInstance)
{
  g_assert(m_init);
  m_instances.push_back(pInstance);
}

void VHDLArchitecture::init_addComponent(VHDLComponent *pComponent)
{
  g_assert(m_init);
  m_components.push_back(pComponent);
}

void VHDLArchitecture::setEntity(VHDLEntity *pEntity)
{
  g_assert(m_pEntity == NULL);
  m_pEntity = pEntity;
}

VHDLComponent *VHDLArchitecture::findComponentByName(Glib::ustring name)
{
  std::list<VHDLComponent *>::iterator it;

  for(it = m_components.begin(); it != m_components.end(); it++)
  {
    if((*it)->getName() == name)
    {
      return *it;
    }
  }
  return NULL;
}

VHDLSignal *VHDLArchitecture::findSignalByName(Glib::ustring name)
{
  std::list<VHDLSignal *>::iterator it;

  for(it = m_signals.begin(); it != m_signals.end(); it++)
  {
    if((*it)->getName() == name)
    {
      return *it;
    }
  }
  return NULL;
}

VHDLInstance *VHDLArchitecture::findInstanceByName(Glib::ustring name)
{
  std::list<VHDLInstance *>::iterator it;

  for(it = m_instances.begin(); it != m_instances.end(); it++)
  {
    if((*it)->getName() == name)
    {
      return *it;
    }
  }
  return NULL;
}

bool VHDLArchitecture::write(FILE *pFile, int indent)
{
  std::list<VHDLSignal *>::iterator sit;
  std::list<VHDLInstance *>::iterator iit;

  m_pEntity->write(pFile, indent);

  fprintf(pFile, "%*sarchitecture %s of %s is\n\n", indent, "", m_name.c_str(), m_pEntity->getName().c_str());

  for(iit = m_instances.begin(); iit != m_instances.end(); iit++)
  {
    (*iit)->getComponent()->write(pFile, indent + 2);
  }

  for(sit = m_signals.begin(); sit != m_signals.end(); sit++)
  {
    (*sit)->write(pFile, indent + 2);
  }
  fprintf(pFile, "\n");

  fprintf(pFile, "%*sbegin\n", indent, "");
  for(iit = m_instances.begin(); iit != m_instances.end(); iit++)
  {
    (*iit)->write(pFile, indent + 2);
  }
  fprintf(pFile, "%*send;\n\n", indent, "");

  return true;
}

/*
 * Private methods
 */

