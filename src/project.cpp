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

#include "layout_architecture.h"
#include "parse_layout.h"
#include "parse_vhdl.h"
#include "project.h"
#include "vhdl_architecture.h"
#include "vhdl_entity.h"

VHDLArchitecture *Project::readVHDLFromFile(std::string fileName)
{
  std::ifstream inFile(fileName);
  VHDLUnitList *pUnitList = ::parseVHDL(inFile);

  VHDLEntity *pEntity = nullptr;
  VHDLArchitecture *pArch = nullptr;

  for(auto &unit: *pUnitList)
  {
    switch(unit.type())
    {
    case UnitType::ENTITY:
      g_assert(!pEntity);
      pEntity = unit.getEntity();
      m_entityMap[pEntity->getName()] = pEntity;
      std::cout << "Entity " << pEntity->getName() << " is at " << pEntity << std::endl;
      break;
    case UnitType::ARCHITECTURE:
      g_assert(!pArch);
      pArch = unit.getArch();
      std::cout << "Architecture " << pArch->getName() << " is at " << pArch << std::endl;
      break;
    default:
      std::cerr << "Unknown entry in unit list" << std::endl;
      g_assert_not_reached();
      break;
    }
  }

  g_assert(pArch);
  return pArch;
}

LayoutArchitecture *Project::readLayoutFromFile(std::string fileName, LayoutResolverActions *pLayoutResolverActions)
{
  std::ifstream inFile(fileName);

  LayoutList *pLayoutList = ::parseLayout(inFile, *pLayoutResolverActions);

  LayoutArchitecture *pLayoutArch = new LayoutArchitecture();

  for(auto &layoutPtr: *pLayoutList)
  {
    switch(layoutPtr.type())
    {
    case LayoutType::COMPONENT:
      pLayoutArch->setComponent(layoutPtr.getComponent());
      break;
    case LayoutType::INSTANCE:
      pLayoutArch->init_addInstance(layoutPtr.getInstance());
      break;
    case LayoutType::SIGNAL:
      pLayoutArch->init_addSignal(layoutPtr.getSignal());
      break;
    default:
      std::cerr << "Unknown entry in layout list" << std::endl;
      g_assert_not_reached();
      break;
    }
  }

  return pLayoutArch;
}

void Project::addFile(std::string fileName)
{
  auto found = fileName.find_last_of(".");
  auto baseName = fileName.substr(0, found);

  auto pArch = readVHDLFromFile(fileName);
  m_fileToVHDLArchMap[baseName] = pArch;

  auto layoutFileName = baseName + ".layout";

  LayoutResolverActions *pResolver = new LayoutResolverActions();
  auto pLayoutArch = readLayoutFromFile(layoutFileName, pResolver);
  m_layoutResolverMap[baseName] = pResolver;
  m_fileToLayoutArchMap[baseName] = pLayoutArch;
}

void Project::resolveEntityReferences()
{
  for(auto &kv: m_fileToVHDLArchMap)
  {
    std::cout << "Resolving entity references for architecture in file " << kv.first << std::endl;
    kv.second->resolveEntityReferences(m_entityMap);
  }
}

void Project::resolveLayoutReferences()
{
  for(auto &kv: m_layoutResolverMap)
  {
    std::cout << "Resolving layout references to VHDL objects in file " << kv.first << std::endl;
    kv.second->run(m_fileToVHDLArchMap.at(kv.first));
  }
}

void Project::save()
{
  for(auto &kv: m_fileToVHDLArchMap)
  {
    std::cout << "Saving architecture " << kv.second->getName() << " to file " << kv.first << "2.*" << std::endl;

    std::ofstream outStream;
    
    outStream.open(kv.first + "2.vhd");
    kv.second->write(outStream, 0);
    outStream.close();

    outStream.open(kv.first + "2.layout");
    m_fileToLayoutArchMap.at(kv.first)->write(outStream);
    outStream.close();
  }
}

