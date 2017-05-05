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

#include <glib.h>
#include <sstream>

#include "layout_architecture.h"
#include "layout_component.h"
#include "layout_file.h"
#include "layout_instance.h"
#include "layout_port.h"
#include "layout_signal.h"
#include "parse_layout.h"
#include "vhdl_architecture.h"
#include "vhdl_component.h"
#include "vhdl_entity.h"
#include "vhdl_file.h"
#include "vhdl_instance.h"
#include "vhdl_port.h"
#include "vhdl_signal.h"

Glib::ustring stripQuotes(const Glib::ustring &s)
{
  g_assert(s[0] == '"');
  g_assert(s[s.length() - 1] == '"');

  return s.substr(1, s.length() - 2);
}

std::pair<int, int> parsePosition(std::istream &stream)
{
  Glib::ustring position;
  int x, y;
  stream >> position >> x >> y;
  g_assert(position == "position");
  return std::make_pair(x, y);
}

std::pair<int, int> parseSize(std::istream &stream)
{
  Glib::ustring size;
  int width, height;
  stream >> size >> width >> height;
  g_assert(size == "size");
  return std::make_pair(width, height);
}

void parsePortsHeader(std::istream &stream)
{
  Glib::ustring ports, brace;
  stream >> ports >> brace;
  g_assert(ports == "ports");
  g_assert(brace == "{");
}

auto parsePort(std::istream &stream, LayoutBlock *pLayoutBlock)
{
  Glib::ustring first, edgeName;
  int index;
  stream >> first;

  if(first != "}")
  {
    first = stripQuotes(first);

    stream >> edgeName >> index;
    Edge edge = NAME_TO_EDGE(edgeName);
    g_assert(edge != NR_OF_EDGES);

    auto pLayoutPort = std::make_unique<LayoutPort>();
    auto portAndName = std::make_pair(pLayoutPort.get(), first);
    pLayoutBlock->init_addPort(edge, index, std::move(pLayoutPort));
    return portAndName;
  }
  else
  {
    return std::make_pair(static_cast<LayoutPort *>(nullptr), Glib::ustring(""));
  }
}

std::unique_ptr<LayoutComponent> parseComponent(std::istream &stream, LayoutResolverActions &resolver)
{
  Glib::ustring brace;

  stream >> brace;
  g_assert(brace == "{");

  auto pLayoutComponent = std::make_unique<LayoutComponent>();
  auto wh = parseSize(stream);
  pLayoutComponent->setSize(LayoutSize(wh.first, wh.second));

  /* Use a raw pointer here so it can be captured by value */
  auto pRawLayoutComponent = pLayoutComponent.get();
  resolver.add([=](VHDLFile *pVHDLFile) { pRawLayoutComponent->associateEntity(pVHDLFile->getEntity()); });

  parsePortsHeader(stream);

  std::pair<LayoutPort *, Glib::ustring> portAndName;
  do
  {
    portAndName = parsePort(stream, pLayoutComponent.get());
    if(portAndName.first != nullptr)
    {
      resolver.add([portAndName](VHDLFile *pVHDLFile) { portAndName.first->associateVHDLPort(pVHDLFile->getEntity()->findPortByName(portAndName.second)); });
    }
  } while(portAndName.first);

  pLayoutComponent->init_done();

  stream >> brace;
  g_assert(brace == "}");

  return pLayoutComponent;
}

LayoutInstance *parseInstance(std::istream &stream, const Glib::ustring &archName, std::map<const Glib::ustring, LayoutInstance *> &layoutInstanceMap, LayoutResolverActions &resolver)
{
  Glib::ustring instanceName, brace;

  stream >> instanceName >> brace;
  g_assert(brace == "{");

  instanceName = stripQuotes(instanceName);
  auto pLayoutInstance = new LayoutInstance();
  auto xy = parsePosition(stream);
  pLayoutInstance->setPosition(LayoutPosition(xy.first, xy.second));
  auto wh = parseSize(stream);
  pLayoutInstance->setSize(LayoutSize(wh.first, wh.second));

  parsePortsHeader(stream);

  std::pair<LayoutPort *, Glib::ustring> portAndName;
  do
  {
    portAndName = parsePort(stream, pLayoutInstance);
    if(portAndName.first != nullptr)
    {
      resolver.add([=](VHDLFile *pVHDLFile) {
        portAndName.first->associateVHDLPort(pVHDLFile->findArchitectureByName(archName)->findInstanceByName(instanceName)->getComponent()->findPortByName(portAndName.second));
      });
    }
  } while(portAndName.first);

  stream >> brace;
  g_assert(brace == "}");

  pLayoutInstance->init_done();

  resolver.add([=](VHDLFile *pVHDLFile) {
    pLayoutInstance->associateVHDLInstance(pVHDLFile->findArchitectureByName(archName)->findInstanceByName(instanceName));
  });

  layoutInstanceMap[instanceName] = pLayoutInstance;

  return pLayoutInstance;
}

void parseEndPoint(std::istream &stream, LayoutSignal::EndPointId expectedEndPointId, std::map<const Glib::ustring, LayoutInstance *> &layoutInstanceMap, LayoutSignal *pLayoutSignal)
{
  Glib::ustring from_or_to, port_or_signal, instance, side;
  int index;

  stream >> from_or_to;

  LayoutSignal::EndPointId endPointId;
  if(from_or_to == "from")
  {
    endPointId = LayoutSignal::BEGINNING;
  }
  else if(from_or_to == "to")
  {
    endPointId = LayoutSignal::END;
  }
  else
  {
    g_assert_not_reached();
  }
  g_assert(endPointId == expectedEndPointId);

  stream >> port_or_signal;

  if(port_or_signal == "port")
  {
    stream >> instance >> side >> index;

    Edge edge = NAME_TO_EDGE(side);
    g_assert(edge != NR_OF_EDGES);
    instance = stripQuotes(instance);

    pLayoutSignal->connect(endPointId, layoutInstanceMap.at(instance), edge, index);
  }
  else if(port_or_signal == "signal")
  {
    /* TODO: implement */
    g_assert_not_reached();
  }
  else if(port_or_signal == "unconnected")
  {
    /* Do nothing */
    return;
  }
  else
  {
    g_assert_not_reached();
  }
}

LayoutSignal *parseSignal(std::istream &stream, const Glib::ustring &archName, std::map<const Glib::ustring, LayoutInstance *> &layoutInstanceMap, LayoutResolverActions &resolver)
{
  Glib::ustring name, brace;
  stream >> name >> brace;

  name = stripQuotes(name);
  
  auto pLayoutSignal = new LayoutSignal();

  parseEndPoint(stream, LayoutSignal::BEGINNING, layoutInstanceMap, pLayoutSignal);
  parseEndPoint(stream, LayoutSignal::END, layoutInstanceMap, pLayoutSignal);

  resolver.add([=](VHDLFile *pVHDLFile) { pLayoutSignal->associateSignal(pVHDLFile->findArchitectureByName(archName)->findSignalByName(name)); });

  Glib::ustring corners;
  stream >> corners >> brace;
  g_assert(corners == "corners");

  Glib::ustring x, y;
  do
  {
    stream >> x;
    if(x != "}")
    {
      stream >> y;
    }
  } while(x != "}");

  stream >> brace;
  g_assert(brace == "}");

  return pLayoutSignal;
}

LayoutArchitecture *parseArchitecture(std::istream &stream, LayoutResolverActions &layoutResolverActions)
{
  Glib::ustring name, brace;
  stream >> name >> brace;

  name = stripQuotes(name);
  auto pLayoutArchitecture = new LayoutArchitecture();
  layoutResolverActions.add([=](VHDLFile *pVHDLFile) { pLayoutArchitecture->associateVHDLArchitecture(pVHDLFile->findArchitectureByName(name)); });

  std::map<const Glib::ustring, LayoutInstance *> layoutInstanceMap;
  Glib::ustring nextToken;
  while(stream >> nextToken)
  {
    if(nextToken == "instance")
    {
      pLayoutArchitecture->init_addInstance(parseInstance(stream, name, layoutInstanceMap, layoutResolverActions));
    }
    else if(nextToken == "signal")
    {
      pLayoutArchitecture->init_addSignal(parseSignal(stream, name, layoutInstanceMap, layoutResolverActions));
    }
    else if(nextToken == "}")
    {
      pLayoutArchitecture->init_done();
      break;
    }
  }
  return pLayoutArchitecture;
}

LayoutFile *parseLayout(std::istream &stream, LayoutResolverActions &layoutResolverActions)
{
  LayoutFile *pLayoutFile = new LayoutFile();

  Glib::ustring nextToken;
  while(stream >> nextToken)
  {
    if(nextToken == "component")
    {
      pLayoutFile->setComponent(parseComponent(stream, layoutResolverActions));
    }
    else if(nextToken == "architecture")
    {
      pLayoutFile->addArchitecture(parseArchitecture(stream, layoutResolverActions));
    }
    else
    {
      std::cout << "Found unexpected token: " << nextToken << "\n";
      g_assert_not_reached();
    }
  }

  return pLayoutFile;
}

