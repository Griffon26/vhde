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

#ifndef _LAYOUT_ARCHITECTURE_H
#define _LAYOUT_ARCHITECTURE_H

#include <glibmm.h>
#include <stdio.h>

class INamedItem;
class LayoutComponent;
class LayoutInstance;
class LayoutSignal;

class LayoutArchitecture
{
private:
  bool                          m_init;
  std::vector<std::unique_ptr<LayoutInstance>> m_instances;
  std::vector<std::unique_ptr<LayoutSignal>> m_signals;
  INamedItem                   *m_pVHDLArchitecture;

public:
  LayoutArchitecture();
  virtual ~LayoutArchitecture();

  LayoutArchitecture(const LayoutArchitecture&) = delete;
  LayoutArchitecture& operator=(const LayoutArchitecture&) = delete;

  /* These methods assume ownership of the component, instance and signal */
  void init_addInstance(std::unique_ptr<LayoutInstance> pInstance);
  void init_addSignal(std::unique_ptr<LayoutSignal> pSignal);

  void init_done() { m_init = false; }

  void associateVHDLArchitecture(INamedItem *pArch);

  /* Accessors */
  const Glib::ustring &getName();

  const std::vector<LayoutInstance *> getInstances();
  const std::vector<LayoutSignal *> getSignals();

  void write(std::ostream &stream, int indent);
};

#endif /* _LAYOUT_ARCHITECTURE_H */

