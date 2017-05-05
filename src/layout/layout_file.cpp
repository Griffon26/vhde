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
#include "layout_file.h"

LayoutFile::LayoutFile():
  m_pComponent(nullptr)
{
}

void LayoutFile::setComponent(std::unique_ptr<LayoutComponent> pComponent)
{
  g_assert(!m_pComponent);
  m_pComponent = std::move(pComponent);
}

void LayoutFile::addArchitecture(LayoutArchitecture *pLayoutArch)
{
  m_architectures.push_back(pLayoutArch);
}

const std::vector<LayoutArchitecture *> &LayoutFile::getArchitectures()
{
  return m_architectures;
}

void LayoutFile::write(std::ostream &stream, int indent)
{
  g_assert(m_pComponent);
  m_pComponent->write(stream, indent);

  for(auto &pArch: m_architectures)
  {
    pArch->write(stream, indent);
  }
}

