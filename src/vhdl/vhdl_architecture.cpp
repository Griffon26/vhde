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
#include <set>

#include "vhdl_architecture.h"
#include "vhdl_component.h"
#include "vhdl_entity.h"
#include "vhdl_instance.h"
#include "vhdl_signal.h"

/*
 * Public methods
 */

VHDLArchitecture::VHDLArchitecture(const Glib::ustring &name):
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

void VHDLArchitecture::init_addComponent(std::unique_ptr<VHDLComponent> pComponent)
{
  g_assert(m_init);
  m_components.push_back(std::move(pComponent));
}

void VHDLArchitecture::init_addSignal(std::unique_ptr<VHDLSignal> pSignal)
{
  g_assert(m_init);
  m_signals.push_back(std::move(pSignal));
}

void VHDLArchitecture::init_addInstance(std::unique_ptr<VHDLInstance> pInstance)
{
  g_assert(m_init);
  pInstance->deleteRequested.connect(sigc::mem_fun(*this, &VHDLArchitecture::removeInstance));
  m_instances.push_back(std::move(pInstance));
}

void VHDLArchitecture::removeInstance(VHDLInstance *pInstance)
{
  g_assert(!m_init);
  m_instances.erase(std::remove_if(m_instances.begin(),
                                   m_instances.end(),
                                   [&](std::unique_ptr<VHDLInstance> &p) { return p.get() == pInstance; }),
                    m_instances.end());
}

const std::vector<VHDLInstance *> VHDLArchitecture::getInstances()
{
  return stripOwnership(m_instances);
}

const std::vector<VHDLSignal *> VHDLArchitecture::getSignals()
{
  return stripOwnership(m_signals);
}

void VHDLArchitecture::setEntity(VHDLEntity *pEntity)
{
  g_assert(m_pEntity == NULL);
  m_pEntity = pEntity;
}

VHDLComponent *VHDLArchitecture::findComponentByName(const Glib::ustring &name)
{
  for(auto &pComponent: m_components)
  {
    if(pComponent->getName() == name)
    {
      return pComponent.get();
    }
  }
  return NULL;
}

VHDLSignal *VHDLArchitecture::findSignalByName(const Glib::ustring &name)
{
  for(auto &pSignal: m_signals)
  {
    if(pSignal->getName() == name)
    {
      return pSignal.get();
    }
  }
  return NULL;
}

VHDLInstance *VHDLArchitecture::findInstanceByName(const Glib::ustring &name)
{
  for(auto &pInstance: m_instances)
  {
    if(pInstance->getName() == name)
    {
      return pInstance.get();
    }
  }
  return NULL;
}

bool VHDLArchitecture::write(std::ostream &outStream, int indent)
{
  Glib::ustring indentString(indent, ' ');

  outStream << indentString << "architecture " << m_name << " of " << m_pEntity->getName() << " is\n\n";

  std::set<VHDLComponent *> components;
  for(auto iit = m_instances.begin(); iit != m_instances.end(); iit++)
  {
    components.insert((*iit)->getComponent());
  }
  for(auto &pComp: components)
  {
    pComp->write(outStream, indent + 2);
  }

  for(auto sit = m_signals.begin(); sit != m_signals.end(); sit++)
  {
    (*sit)->write(outStream, indent + 2);
  }
  outStream << "\n";

  outStream << indentString << "begin\n";
  for(auto iit = m_instances.begin(); iit != m_instances.end(); iit++)
  {
    (*iit)->write(outStream, indent + 2);
  }
  outStream << indentString << "end;\n\n";

  return true;
}

/* Associate each VHDL component declaration with a VHDL entity definition from another file */
void VHDLArchitecture::resolveEntityReferences(const std::map<const Glib::ustring, VHDLEntity *> &entityMap)
{
  for(auto &pComp: m_components)
  {
    if(!pComp->getAssociatedEntity())
    {
      pComp->associateEntity(entityMap.at(pComp->getName()));
    }
  }
}

/*
 * Private methods
 */

