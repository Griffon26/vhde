/* vim: ts=2:sw=2:expandtab
 *
 * VHDE - The VHDL Diagram Editor
 * Copyright (C) 2009-2010  Maurice van der Pot <griffon26@kfk4ever.com>
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

#ifndef _VHDL_ARCHITECTURE_H
#define _VHDL_ARCHITECTURE_H

#include <glibmm.h>
#include <stdio.h>

#include "i_named_item.h"

class VHDLComponent;
class VHDLEntity;
class VHDLInstance;
class VHDLSignal;

class VHDLArchitecture: public INamedItem
{
private:
  bool                          m_init;
  Glib::ustring                 m_name;
  VHDLEntity                   *m_pEntity;
  std::vector<std::unique_ptr<VHDLComponent>> m_components;
  std::vector<std::unique_ptr<VHDLSignal>> m_signals;
  std::vector<std::unique_ptr<VHDLInstance>> m_instances;

public:
  VHDLArchitecture(const Glib::ustring &name);
  virtual ~VHDLArchitecture();

  VHDLArchitecture(const VHDLArchitecture&) = delete;
  VHDLArchitecture& operator=(const VHDLArchitecture&) = delete;

  void init_addComponent(std::unique_ptr<VHDLComponent> pComponent);
  void init_addSignal(std::unique_ptr<VHDLSignal> pSignal);
  void init_addInstance(std::unique_ptr<VHDLInstance> pInstance);
  void init_done() { m_init = false; }

  /* Accessors */
  const std::vector<VHDLInstance *> getInstances();
  const std::vector<VHDLSignal *> getSignals();

  void setEntity(VHDLEntity *pEntity);
  VHDLEntity *getEntity() { return m_pEntity; }

  VHDLComponent *findComponentByName(const Glib::ustring &name);
  VHDLSignal *findSignalByName(const Glib::ustring &name);
  VHDLInstance *findInstanceByName(const Glib::ustring &name);

  bool write(std::ostream &outStream, int indent);

  const Glib::ustring &getName() { return m_name; }

  void resolveEntityReferences(const std::map<const Glib::ustring, VHDLEntity *> &entityMap);

};

#endif /* _VHDL_ARCHITECTURE_H */

