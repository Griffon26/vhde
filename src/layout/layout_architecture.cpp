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

#include "layout_architecture.h"
#include "layout_component.h"
#include "layout_instance.h"
#include "layout_signal.h"

LayoutArchitecture::LayoutArchitecture():
  m_pComponent(nullptr)
{
}

LayoutArchitecture::~LayoutArchitecture()
{
  /* TODO: release all owned resources */
}

void LayoutArchitecture::setComponent(LayoutComponent *pComponent)
{
  g_assert(!m_pComponent);
  m_pComponent = pComponent;
}

void LayoutArchitecture::init_addInstance(LayoutInstance *pInstance)
{
  g_assert(m_init);
  m_instances.push_back(pInstance);
}

void LayoutArchitecture::init_addSignal(LayoutSignal *pSignal)
{
  g_assert(m_init);
  m_signals.push_back(pSignal);
}

void LayoutArchitecture::write(std::ostream &stream)
{
  g_assert(m_pComponent);
  m_pComponent->write(stream);

  for(auto &pInstance: m_instances)
  {
    pInstance->write(stream);
  }
  for(auto &pSignal: m_signals)
  {
    pSignal->write(stream);
  }
}

