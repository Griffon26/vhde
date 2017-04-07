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

#include <fstream>

#include "parser.h"
#include "project.h"
#include "vhdl_architecture.h"
#include "vhdl_entity.h"

void Project::addFile(std::string fileName)
{
  std::ifstream inFile(fileName);
  VHDLUnitList unitList = m_pParser->parseVHDL(inFile);

  for(auto &unit: unitList)
  {
    VHDLEntity *pEntity = nullptr;
    VHDLArchitecture *pArch = nullptr;

    switch(unit.type())
    {
    case UnitType::ENTITY:
      pEntity = unit.getEntity();
      std::cout << "Entity " << pEntity->getName() << " is at " << pEntity << std::endl;
      break;
    case UnitType::ARCHITECTURE:
      pArch = unit.getArch();
      std::cout << "Architecture " << pArch->getName() << " is at " << pArch << std::endl;
      break;
    default:
      std::cout << "Unknown entry in unit list" << std::endl;
      break;
    }
  }
}

