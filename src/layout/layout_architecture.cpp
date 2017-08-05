/* vim: ts=2:sw=2:expandtab
 *
 * VHDE - The VHDL Diagram Editor
 * Copyright (C) 2009-2017  Maurice van der Pot <griffon26@kfk4ever.com>
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

#include "common.h"
#include "i_named_item.h"
#include "layout_architecture.h"
#include "layout_component.h"
#include "layout_instance.h"
#include "layout_signal.h"

LayoutArchitecture::LayoutArchitecture():
  m_init(true),
  m_pVHDLArchitecture(nullptr)
{
}

LayoutArchitecture::~LayoutArchitecture()
{
  /* TODO: release all owned resources */
}

void LayoutArchitecture::init_addInstance(std::unique_ptr<LayoutInstance> pInstance)
{
  g_assert(m_init);
  pInstance->deleteRequested.connect(sigc::mem_fun(*this, &LayoutArchitecture::removeInstance));
  m_instances.push_back(std::move(pInstance));
}

void LayoutArchitecture::init_addSignal(std::unique_ptr<LayoutSignal> pSignal)
{
  g_assert(m_init);
  m_signals.push_back(std::move(pSignal));
}

void LayoutArchitecture::removeInstance(LayoutInstance *pInstance)
{
  g_assert(!m_init);
  remove_unique_ptr(m_instances, pInstance);
}

void LayoutArchitecture::associateVHDLArchitecture(INamedItem *pVHDLArchitecture)
{
  g_assert(!m_pVHDLArchitecture);
  g_assert(pVHDLArchitecture);
  m_pVHDLArchitecture = pVHDLArchitecture;
}

const Glib::ustring &LayoutArchitecture::getName()
{
  g_assert(m_pVHDLArchitecture);
  return m_pVHDLArchitecture->getName();
}

const std::vector<LayoutInstance *> LayoutArchitecture::getInstances()
{
  return stripOwnership(m_instances);
}

const std::vector<LayoutSignal *> LayoutArchitecture::getSignals()
{
  return stripOwnership(m_signals);
}

void LayoutArchitecture::write(std::ostream &stream, int indent)
{
  stream << "architecture \"" << m_pVHDLArchitecture->getName() << "\" {\n";

  for(auto &pInstance: m_instances)
  {
    pInstance->write(stream, indent + 2);
  }
  for(auto &pSignal: m_signals)
  {
    pSignal->write(stream, indent + 2);
  }

  stream << "}\n";
}

