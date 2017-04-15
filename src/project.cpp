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
#include "layout_component.h"
#include "layout_instance.h"
#include "layout_signal.h"
#include "parse_layout.h"
#include "parse_vhdl.h"
#include "project.h"
#include "vhdl_architecture.h"
#include "vhdl_entity.h"

static std::string getBaseName(std::string fileName)
{
  auto dotIndex = fileName.rfind(".");
  return fileName.substr(0, dotIndex);
}

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
  LayoutArchitecture *pLayoutArch = nullptr;
  std::ifstream inFile(fileName);

  if(inFile.good())
  {
    LayoutList *pLayoutList = ::parseLayout(inFile, *pLayoutResolverActions);

    pLayoutArch = new LayoutArchitecture();

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
  }

  return pLayoutArch;
}

void Project::createDefaultPorts(VHDLInterface *pVHDLInterface, LayoutBlock *pLayoutBlock)
{
  auto portList = pVHDLInterface->getPortList();
  int nrOfPortsToAddOnLeft = portList->size() / 2;
  int portIndex = 0;

  for(auto &pPort: *portList)
  {
    LayoutPort *pLayoutPort = new LayoutPort();
    pLayoutPort->associateVHDLPort(pPort);
    if(portIndex < nrOfPortsToAddOnLeft)
    {
      pLayoutBlock->init_addPort(EDGE_LEFT, portIndex, pLayoutPort);
    }
    else
    {
      pLayoutBlock->init_addPort(EDGE_RIGHT, portIndex - nrOfPortsToAddOnLeft, pLayoutPort);
    }
    portIndex++;
  }
  LayoutSize size;
  pLayoutBlock->getMinimumSize(&size);
  if(size.width < 200) size.width = 200;
  if(size.height < 250) size.height = 250;
  pLayoutBlock->setSize(size);
}

LayoutArchitecture *Project::createDefaultArchitectureLayout(VHDLArchitecture *pArch)
{
  std::cout << "Creating default layout for architecture " << pArch->getName() << std::endl;

  LayoutArchitecture *pLayoutArch = new LayoutArchitecture();

  LayoutComponent *pLayoutComponent = new LayoutComponent();
  createDefaultPorts(pArch->getEntity(), pLayoutComponent);

  pLayoutArch->setComponent(pLayoutComponent);

  std::map<VHDLSignal *, LayoutSignal *> vhdlToLayoutSignal;
  for(auto &pSignal: pArch->getSignals())
  {
    LayoutSignal *pLayoutSignal = new LayoutSignal();
    pLayoutSignal->associateSignal(pSignal);
    pLayoutArch->init_addSignal(pLayoutSignal);
    vhdlToLayoutSignal[pSignal] = pLayoutSignal;
  }

  for(auto &pInstance: pArch->getInstances())
  {
    LayoutInstance *pLayoutInstance = new LayoutInstance();
    createDefaultPorts(pInstance->getComponent(), pLayoutInstance);

    auto pPortsAndSignals = pInstance->getPortsAndSignals();
		for(auto &portAndSignal: *pPortsAndSignals)
		{
      Edge edge;
      int position;
      LayoutSignal *pLayoutSignal = vhdlToLayoutSignal.at(portAndSignal.second);
      pLayoutInstance->findPortByName(portAndSignal.first->getName(), &edge, &position);
      std::cout << "Connecting port " << portAndSignal.first->getName() << " to instance at edge " << edge << " position " << position << std::endl;
      pLayoutSignal->init_connect(pLayoutInstance, edge, position);
    }
    delete pPortsAndSignals;

    pLayoutInstance->init_done();
    pLayoutInstance->associateVHDLInstance(pInstance);
    pLayoutArch->init_addInstance(pLayoutInstance);
  }

  return pLayoutArch;
}

void Project::addFile(std::string fileName)
{
  auto baseName = getBaseName(fileName);

  auto pArch = readVHDLFromFile(fileName);
  m_fileToVHDLArchMap[baseName] = pArch;

  auto layoutFileName = baseName + ".layout";

  LayoutResolverActions *pResolver = new LayoutResolverActions();
  auto pLayoutArch = readLayoutFromFile(layoutFileName, pResolver);

  /* If we failed to read an architecture layout from file, we'll construct a suitable one here */
  if(!pLayoutArch)
  {
    pLayoutArch = createDefaultArchitectureLayout(pArch);
  }

  m_layoutResolverMap[baseName] = pResolver;
  m_fileToLayoutArchMap[baseName] = pLayoutArch;
}

void Project::resolveEntityReferences()
{
  for(auto &kv: m_fileToVHDLArchMap)
  {
    std::cout << "Resolving vhdl entity references for vhdl architecture in file " << kv.first << std::endl;
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

void Project::resolveLayoutComponentReferences()
{
  std::map<std::string, LayoutComponent *> componentMap;

  /* First create a map of names to components by getting the name from the associated VHDL entity */
  for(auto &kv: m_fileToLayoutArchMap)
  {
    LayoutComponent *pComponent = (LayoutComponent *)kv.second->getComponent();
    componentMap[pComponent->getAssociatedVHDLEntity()->getName()] = pComponent;
  }

  /* Now associate all layout instances with the appropriate layout components through the associated VHDL entity name */
  for(auto &kvarch: m_fileToLayoutArchMap)
  {
    for(auto &pInstance: kvarch.second->getInstances())
    {
      auto instanceName = pInstance->getAssociatedVHDLInstance()->getName();
      pInstance->associateLayoutComponent(componentMap.at(instanceName));
    }
  }
}

LayoutArchitecture *Project::getLayoutArchitecture(std::string fileName)
{
  return m_fileToLayoutArchMap.at(getBaseName(fileName));
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

