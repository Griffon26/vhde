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
#include "layout_file.h"
#include "layout_component.h"
#include "layout_instance.h"
#include "layout_port.h"
#include "layout_signal.h"
#include "parse_layout.h"
#include "parse_vhdl.h"
#include "project.h"
#include "vhdl_architecture.h"
#include "vhdl_component.h"
#include "vhdl_entity.h"
#include "vhdl_file.h"
#include "vhdl_instance.h"
#include "vhdl_port.h"
#include "vhdl_signal.h"

static Glib::ustring getBaseName(const Glib::ustring &fileName)
{
  auto dotIndex = fileName.rfind(".");
  return fileName.substr(0, dotIndex);
}

std::unique_ptr<VHDLFile> Project::readVHDLFromFile(const Glib::ustring &fileName, VHDLFile::Mode mode, std::map<const Glib::ustring, VHDLEntity *> &entityMap)
{
  std::cout << "Opening file " << fileName << "..." << std::endl;
  std::ifstream inFile(fileName);
  auto pVHDLFile = ::parseVHDL(inFile, mode);

  VHDLEntity *pEntity = pVHDLFile->getEntity();
  entityMap[pEntity->getName()] = pEntity;

  pVHDLFile->setName(fileName);

  return pVHDLFile;
}

std::unique_ptr<LayoutFile> Project::readLayoutFromFile(const Glib::ustring &fileName, LayoutResolverActions *pLayoutResolverActions)
{
  std::cout << "Opening file " << fileName << "..." << std::endl;
  std::unique_ptr<LayoutFile> pLayoutFile;
  std::ifstream inFile(fileName);

  if(inFile.good())
  {
    pLayoutFile = ::parseLayout(inFile, *pLayoutResolverActions);
  }

  return pLayoutFile;
}

void Project::createDefaultPorts(VHDLInterface *pVHDLInterface, LayoutBlock *pLayoutBlock)
{
  auto portList = pVHDLInterface->getPortList();
  int nrOfPortsToAddOnLeft = portList.size() / 2;
  int portIndex = 0;

  for(auto &pPort: portList)
  {
    auto pLayoutPort = std::make_unique<LayoutPort>();
    pLayoutPort->associateVHDLPort(pPort);
    if(portIndex < nrOfPortsToAddOnLeft)
    {
      pLayoutBlock->init_addPort(EDGE_LEFT, portIndex, std::move(pLayoutPort));
    }
    else
    {
      pLayoutBlock->init_addPort(EDGE_RIGHT, portIndex - nrOfPortsToAddOnLeft, std::move(pLayoutPort));
    }
    portIndex++;
  }
  LayoutSize size;
  pLayoutBlock->getMinimumSize(&size);
  if(size.width < 200) size.width = 200;
  if(size.height < 250) size.height = 250;
  pLayoutBlock->setSize(size);
}

std::unique_ptr<LayoutArchitecture> Project::createDefaultArchitectureLayout(VHDLArchitecture *pArch)
{
  std::cout << "Creating default layout for architecture " << pArch->getName() << std::endl;

  auto pLayoutArch = std::make_unique<LayoutArchitecture>();

  pLayoutArch->associateVHDLArchitecture(pArch);

  std::map<VHDLSignal *, LayoutSignal *> vhdlToLayoutSignal;
  for(auto &pSignal: pArch->getSignals())
  {
    auto pLayoutSignal = std::make_unique<LayoutSignal>();
    pLayoutSignal->associateSignal(pSignal);
    vhdlToLayoutSignal[pSignal] = pLayoutSignal.get();
    pLayoutArch->init_addSignal(std::move(pLayoutSignal));
  }

  for(auto &pInstance: pArch->getInstances())
  {
    auto pLayoutInstance = std::make_unique<LayoutInstance>();
    createDefaultPorts(pInstance->getComponent(), pLayoutInstance.get());

    auto portsAndSignals = pInstance->getPortsAndSignals();
		for(auto &portAndSignal: portsAndSignals)
		{
      Edge edge;
      int position;
      LayoutSignal *pLayoutSignal = vhdlToLayoutSignal.at(portAndSignal.second);
      pLayoutInstance->findPortByName(portAndSignal.first->getName(), &edge, &position);
      std::cout << "Connecting port " << portAndSignal.first->getName() << " to instance at edge " << edge << " position " << position << std::endl;
      pLayoutSignal->init_connect(pLayoutInstance.get(), edge, position);
    }

    pLayoutInstance->init_done();
    pLayoutInstance->associateVHDLInstance(pInstance);
    pLayoutArch->init_addInstance(std::move(pLayoutInstance));
  }

  return pLayoutArch;
}

std::unique_ptr<LayoutFile> Project::createDefaultFileLayout(VHDLFile *pVHDLFile)
{
  std::cout << "Creating default layout for new file" << std::endl;

  auto pLayoutFile = std::make_unique<LayoutFile>();

  auto pLayoutComponent = std::make_unique<LayoutComponent>();
  createDefaultPorts(pVHDLFile->getEntity(), pLayoutComponent.get());

  pLayoutFile->setComponent(std::move(pLayoutComponent));

  if(pVHDLFile->getMode() == VHDLFile::GRAPHICAL)
  {
    for(auto &pVHDLArch: pVHDLFile->getArchitectures())
    {
      pLayoutFile->addArchitecture(createDefaultArchitectureLayout(pVHDLArch));
    }
  }

  return pLayoutFile;
}

void Project::addFile(const Glib::ustring &fileName, VHDLFile::Mode mode)
{
  auto baseName = getBaseName(fileName);

  auto pVHDLFile = readVHDLFromFile(fileName, mode, m_entityMap);

  auto layoutFileName = baseName + ".layout";

  std::unique_ptr<LayoutResolverActions> pResolver = std::make_unique<LayoutResolverActions>();
  auto pLayoutFile = readLayoutFromFile(layoutFileName, pResolver.get());

  /* If we failed to read an architecture layout from file, we'll construct a suitable one here */
  if(!pLayoutFile)
  {
    pLayoutFile = createDefaultFileLayout(pVHDLFile.get());
  }

  m_fileToVHDLFileMap[baseName] = std::move(pVHDLFile);
  m_fileToLayoutFileMap[baseName] = std::move(pLayoutFile);

  m_layoutResolverMap[baseName] = std::move(pResolver);
}

void Project::resolveEntityReferences()
{
  for(auto &kv: m_fileToVHDLFileMap)
  {
    if(kv.second->getMode() == VHDLFile::GRAPHICAL)
    {
      std::cout << "Resolving vhdl entity references for vhdl architectures in file " << kv.first << std::endl;
      for(auto &pArch: kv.second->getArchitectures())
      {
        pArch->resolveEntityReferences(m_entityMap);
      }
    }
  }
}

void Project::resolveLayoutReferences()
{
  for(auto &kv: m_layoutResolverMap)
  {
    std::cout << "Resolving layout references to VHDL objects in file " << kv.first << std::endl;
    kv.second->run(m_fileToVHDLFileMap.at(kv.first).get());
  }
}

void Project::resolveLayoutComponentReferences()
{
  std::map<const Glib::ustring, LayoutComponent *> componentMap;

  /* First create a map of names to components by getting the name from the associated VHDL entity */
  for(auto &kv: m_fileToLayoutFileMap)
  {
    LayoutComponent *pComponent = (LayoutComponent *)kv.second->getComponent();
    componentMap[pComponent->getAssociatedVHDLEntity()->getName()] = pComponent;
  }

  /* Now associate all layout instances with the appropriate layout components through the associated VHDL entity name */
  for(auto &kvfile: m_fileToLayoutFileMap)
  {
    for(auto pArch: kvfile.second->getArchitectures())
    {
      for(auto &pInstance: pArch->getInstances())
      {
        auto instanceName = pInstance->getAssociatedVHDLInstance()->getName();
        pInstance->associateLayoutComponent(componentMap.at(instanceName));
      }
    }
  }
}

LayoutFile *Project::getLayoutFile(const Glib::ustring &fileName)
{
  return m_fileToLayoutFileMap.at(getBaseName(fileName)).get();
}

void Project::save()
{
  for(auto &kv: m_fileToVHDLFileMap)
  {
    std::cout << "Saving file " << kv.second->getName() << " to file " << kv.first << ".*" << std::endl;

    std::ofstream outStream;
    
    outStream.open(kv.first + ".vhd");
    kv.second->write(outStream, 0);
    outStream.close();

    outStream.open(kv.first + ".layout");
    m_fileToLayoutFileMap.at(kv.first)->write(outStream, 0);
    outStream.close();
  }
}

