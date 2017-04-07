/* vim: ts=2:sw=2:expandtab
 *
 * VHDE - The VHDL Diagram Editor
 * Copyright (C) 2017  Maurice van der Pot <griffon26@kfk4ever.com>
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

#ifndef _PARSER_H
#define _PARSER_H

#include <assert.h>
#include <iostream>
#include <vector>

class VHDLEntity;
class VHDLArchitecture;

enum class UnitType
{
  ENTITY,
  CONFIGURATION,  // Not yet implemented
  PACKAGE,        // Not yet implemented
  ARCHITECTURE,
  PACKAGE_BODY    // Not yet implemented
};

/* Use this class for now until std::variant is more widely supported */
class VHDLUnitPtr
{
private:
  UnitType m_type;
  union {
    VHDLEntity *m_pEntity;
    VHDLArchitecture *m_pArch;
  };
public:
  VHDLUnitPtr(VHDLEntity *pEntity):
    m_pEntity(pEntity),
    m_type(UnitType::ENTITY)
  {
  }

  VHDLUnitPtr(VHDLArchitecture *pArch):
    m_pArch(pArch),
    m_type(UnitType::ARCHITECTURE)
  {
  }

  UnitType type()
  {
    return m_type;
  }

  VHDLEntity *getEntity()
  {
    assert(m_type == UnitType::ENTITY);
    return m_pEntity;
  }

  VHDLArchitecture *getArch()
  {
    assert(m_type == UnitType::ARCHITECTURE);
    return m_pArch;
  }
};

typedef std::vector<VHDLUnitPtr> VHDLUnitList;

class Parser
{
public:
   VHDLUnitList parseVHDL(std::istream &stream) const;
};

#endif

