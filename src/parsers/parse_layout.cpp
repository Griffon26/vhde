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

#include "layout_component.h"
#include "layout_instance.h"
#include "layout_port.h"
#include "layout_signal.h"
#include "parse_layout.h"
#include "vhdl_architecture.h"
#include "vhdl_component.h"
#include "vhdl_entity.h"
#include "vhdl_instance.h"
#include "vhdl_port.h"
#include "vhdl_signal.h"

std::string stripQuotes(std::string s)
{
  return std::string(s.begin() + 1, s.end() - 1);
}

std::pair<int, int> parsePosition(std::istream &stream)
{
  std::string position;
  int x, y;
  stream >> position >> x >> y;
  g_assert(position == "position");
  return std::make_pair(x, y);
}

std::pair<int, int> parseSize(std::istream &stream)
{
  std::string size;
  int width, height;
  stream >> size >> width >> height;
  g_assert(size == "size");
  return std::make_pair(width, height);
}

void parsePortsHeader(std::istream &stream)
{
  std::string ports, brace;
  stream >> ports >> brace;
  g_assert(ports == "ports");
}

auto parsePort(std::istream &stream, LayoutBlock *pLayoutBlock)
{
  std::string first, name;
  int index;
  stream >> first;

  if(first != "}")
  {
    Edge edge = NAME_TO_EDGE(first);
    g_assert(edge != NR_OF_EDGES);
    stream >> index >> name;
    name = stripQuotes(name);

    auto pLayoutPort = new LayoutPort();
    pLayoutBlock->init_addPort(edge, index, pLayoutPort);
    return std::make_pair(pLayoutPort, name);
  }
  else
  {
    return std::make_pair(static_cast<LayoutPort *>(nullptr), std::string(""));
  }
}

LayoutComponent *parseComponent(std::istream &stream, LayoutResolverActions &resolver)
{
  std::string brace;

  stream >> brace;

  auto pLayoutComponent = new LayoutComponent();
  auto wh = parseSize(stream);
  pLayoutComponent->setSize(LayoutSize(wh.first, wh.second));
  resolver.add([pLayoutComponent](VHDLArchitecture *pArch) { pLayoutComponent->associateEntity(pArch->getEntity()); });

  parsePortsHeader(stream);

  std::pair<LayoutPort *, std::string> portAndName;
  do
  {
    portAndName = parsePort(stream, pLayoutComponent);
    if(portAndName.first != nullptr)
    {
      resolver.add([portAndName](VHDLArchitecture *pArch) { portAndName.first->associateVHDLPort(pArch->getEntity()->findPortByName(portAndName.second)); });
    }
  } while(portAndName.first);

  pLayoutComponent->init_done();

  return pLayoutComponent;
}

LayoutInstance *parseInstance(std::istream &stream, std::map<std::string, LayoutInstance *> &layoutInstanceMap, LayoutResolverActions &resolver)
{
  std::string instanceName, brace;

  stream >> instanceName >> brace;

  instanceName = stripQuotes(instanceName);
  auto pLayoutInstance = new LayoutInstance();
  auto xy = parsePosition(stream);
  pLayoutInstance->setPosition(LayoutPosition(xy.first, xy.second));
  auto wh = parseSize(stream);
  pLayoutInstance->setSize(LayoutSize(wh.first, wh.second));

  parsePortsHeader(stream);

  std::pair<LayoutPort *, std::string> portAndName;
  do
  {
    portAndName = parsePort(stream, pLayoutInstance);
    if(portAndName.first != nullptr)
    {
      resolver.add([portAndName, instanceName](VHDLArchitecture *pArch) { portAndName.first->associateVHDLPort(pArch->findInstanceByName(instanceName)->getComponent()->findPortByName(portAndName.second)); });
    }
  } while(portAndName.first);

  pLayoutInstance->init_done();

  resolver.add([pLayoutInstance, instanceName](VHDLArchitecture *pArch) { pLayoutInstance->associateVHDLInstance(pArch->findInstanceByName(instanceName)); });

  layoutInstanceMap[instanceName] = pLayoutInstance;

  return pLayoutInstance;
}

LayoutSignal *parseSignal(std::istream &stream, std::map<std::string, LayoutInstance *> &layoutInstanceMap, LayoutResolverActions &resolver)
{
  std::string name, brace;
  stream >> name >> brace;

  name = stripQuotes(name);
  std::string from, port1, to, port2, instance, side;
  int index;
  stream >> from >> port1 >> to >> port2 >> instance >> side >> index;
  g_assert(from == "from");
  g_assert(to == "to");
  instance = stripQuotes(instance);

  auto pLayoutSignal = new LayoutSignal();
  pLayoutSignal->connect(LayoutSignal::END, layoutInstanceMap.at(instance), NAME_TO_EDGE(side), index);

  resolver.add([pLayoutSignal, name](VHDLArchitecture *pArch) { pLayoutSignal->associateSignal(pArch->findSignalByName(name)); });

  std::string corners;
  stream >> corners >> brace;
  g_assert(corners == "corners");

  std::string x, y;
  do
  {
    stream >> x;
    if(x != "}")
    {
      stream >> y;
    }
  } while(x != "}");

  return pLayoutSignal;
}

LayoutList *parseLayout(std::istream &stream, LayoutResolverActions &layoutResolverActions)
{
  LayoutList *pLayoutList = new LayoutList();
  std::map<std::string, LayoutInstance *> layoutInstanceMap;

  std::string nextToken;
  while(stream >> nextToken)
  {
    if(nextToken == "component")
    {
      pLayoutList->push_back(parseComponent(stream, layoutResolverActions));
    }
    else if(nextToken == "instance")
    {
      pLayoutList->push_back(parseInstance(stream, layoutInstanceMap, layoutResolverActions));
    }
    else if(nextToken == "signal")
    {
      pLayoutList->push_back(parseSignal(stream, layoutInstanceMap, layoutResolverActions));
    }
    else if(nextToken == "}")
    {
      // this should be the last token
    }
    else
    {
      g_assert_not_reached();
    }
  }

  return pLayoutList;
}

