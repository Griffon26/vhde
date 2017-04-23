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

#include "vhdl_architecture.h"
#include "vhdl_entity.h"
#include "vhdl_fragment.h"
#include "vhdl_file.h"

VHDLFile::VHDLFile(VHDLFile::Mode mode):
  m_mode(mode),
  m_pContext(nullptr),
  m_pEntity(nullptr)
{
}

void VHDLFile::setName(const Glib::ustring &name)
{
  m_name = name;
}

void VHDLFile::setContext(VHDLFragment *pFragment)
{
  g_assert(!m_pContext);
  g_assert(pFragment);
  m_pContext = pFragment;
}

void VHDLFile::setEntity(VHDLEntity *pEntity)
{
  g_assert(!m_pEntity);
  m_pEntity = pEntity;
}

void VHDLFile::addArchitecture(VHDLArchitecture *pArch)
{
  g_assert(m_mode == GRAPHICAL);
  m_architectures.push_back(pArch);
}

const std::vector<VHDLArchitecture *> &VHDLFile::getArchitectures()
{
  g_assert(m_mode == GRAPHICAL);
  return m_architectures;
}

VHDLArchitecture *VHDLFile::findArchitectureByName(Glib::ustring name)
{
  g_assert(m_mode == GRAPHICAL);
  for(auto pArch: m_architectures)
  {
    if(pArch->getName() == name)
    {
      return pArch;
    }
  }
  return nullptr;
}

void VHDLFile::addOtherFragment(VHDLFragment *pFragment)
{
  g_assert(m_mode == TEXT);
  m_otherFragments.push_back(pFragment);
}

const std::vector<VHDLFragment *> &VHDLFile::getFragments()
{
  g_assert(m_mode == TEXT);
  return m_otherFragments;
}

bool VHDLFile::write(std::ostream &outStream, int indent)
{
  g_assert(m_name != "");
  std::string indentString(indent, ' ');

  if(m_pContext)
  {
    outStream << indentString << m_pContext->getText() << "\n";
  }

  m_pEntity->write(outStream, indent);

  if(m_mode == GRAPHICAL)
  {
    for(auto pArch: m_architectures)
    {
      pArch->write(outStream, indent);
    }
  }
  else
  {
    for(auto pFragment: m_otherFragments)
    {
      pFragment->write(outStream, indent);
    }
  }

  return true;
}
