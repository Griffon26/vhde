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

#ifndef _PARSE_LAYOUT_H
#define _PARSE_LAYOUT_H

#include <functional>
#include <glib.h>
#include <iostream>
#include <vector>

class LayoutComponent;
class LayoutFile;
class LayoutInstance;
class LayoutSignal;
class VHDLFile;

enum class LayoutType
{
  COMPONENT,
  INSTANCE,
  SIGNAL
};

/* Use this class for now until std::variant is more widely supported */
class LayoutPtr
{
private:
  LayoutType m_type;
  union {
    LayoutComponent *m_pComponent;
    LayoutInstance *m_pInstance;
    LayoutSignal *m_pSignal;
  };
public:
  LayoutPtr(LayoutComponent *pComponent):
    m_type(LayoutType::COMPONENT),
    m_pComponent(pComponent)
  {
  }

  LayoutPtr(LayoutInstance *pInstance):
    m_type(LayoutType::INSTANCE),
    m_pInstance(pInstance)
  {
  }

  LayoutPtr(LayoutSignal *pSignal):
    m_type(LayoutType::SIGNAL),
    m_pSignal(pSignal)
  {
  }

  LayoutType type()
  {
    return m_type;
  }

  LayoutComponent *getComponent()
  {
    g_assert(m_type == LayoutType::COMPONENT);
    return m_pComponent;
  }

  LayoutInstance *getInstance()
  {
    g_assert(m_type == LayoutType::INSTANCE);
    return m_pInstance;
  }

  LayoutSignal *getSignal()
  {
    g_assert(m_type == LayoutType::SIGNAL);
    return m_pSignal;
  }
};

class LayoutResolverActions
{
private:
  std::vector<std::function<void(VHDLFile *)>> m_actions;

public:
  void add(std::function<void(VHDLFile *)> action) { m_actions.push_back(action); }
  void run(VHDLFile *pVHDLFile)
  {
    for(auto &action: m_actions)
    {
      action(pVHDLFile);
    }
  }
};

typedef std::vector<LayoutPtr> LayoutList;

std::unique_ptr<LayoutFile> parseLayout(std::istream &stream, LayoutResolverActions &layoutResolverActions);

#endif

