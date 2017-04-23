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

VHDLFile *Project::readVHDLFromFile(const Glib::ustring &fileName, VHDLFile::Mode mode, std::map<const Glib::ustring, VHDLEntity *> &entityMap)
{
  std::cout << "Opening file " << fileName << "..." << std::endl;
  std::ifstream inFile(fileName);
  VHDLFile *pVHDLFile = ::parseVHDL(inFile, mode);

  VHDLEntity *pEntity = pVHDLFile->getEntity();
  entityMap[pEntity->getName()] = pEntity;

  pVHDLFile->setName(fileName);

  return pVHDLFile;
}

LayoutFile *Project::readLayoutFromFile(const Glib::ustring &fileName, LayoutResolverActions *pLayoutResolverActions)
{
  std::cout << "Opening file " << fileName << "..." << std::endl;
  LayoutFile *pLayoutFile = nullptr;
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

  pLayoutArch->associateVHDLArchitecture(pArch);

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

LayoutFile *Project::createDefaultFileLayout(VHDLFile *pVHDLFile)
{
  std::cout << "Creating default layout for new file" << std::endl;

  LayoutFile *pLayoutFile = new LayoutFile();

  LayoutComponent *pLayoutComponent = new LayoutComponent();
  createDefaultPorts(pVHDLFile->getEntity(), pLayoutComponent);

  pLayoutFile->setComponent(pLayoutComponent);

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
  m_fileToVHDLFileMap[baseName] = pVHDLFile;

  auto layoutFileName = baseName + ".layout";

  LayoutResolverActions *pResolver = new LayoutResolverActions();
  auto pLayoutFile = readLayoutFromFile(layoutFileName, pResolver);

  /* If we failed to read an architecture layout from file, we'll construct a suitable one here */
  if(!pLayoutFile)
  {
    pLayoutFile = createDefaultFileLayout(pVHDLFile);
  }
  m_fileToLayoutFileMap[baseName] = pLayoutFile;

  m_layoutResolverMap[baseName] = pResolver;
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
    kv.second->run(m_fileToVHDLFileMap.at(kv.first));
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
  return m_fileToLayoutFileMap.at(getBaseName(fileName));
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

