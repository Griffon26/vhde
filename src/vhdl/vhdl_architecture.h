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

#include "vhdl_component.h"
#include "vhdl_entity.h"
#include "vhdl_instance.h"
#include "vhdl_signal.h"

class VHDLArchitecture
{
private:
  bool                        m_init;
  Glib::ustring               m_name;
  VHDLEntity                 *m_pEntity;
  std::list<VHDLComponent *>  m_components;
  std::list<VHDLSignal *>     m_signals;
  std::list<VHDLInstance *>   m_instances;

public:
  VHDLArchitecture(Glib::ustring name);
  virtual ~VHDLArchitecture();

  /* This method assumes ownership of the port */
  void init_addSignal(VHDLSignal *pSignal);
  /* This method assumes ownership of the component */
  void init_addComponent(VHDLComponent *pComponent);
  /* This method assumes ownership of the instance */
  void init_addInstance(VHDLInstance *pInstance);
  void init_done() { m_init = false; }

  void setEntity(VHDLEntity *pEntity);
  VHDLEntity *getEntity() { return m_pEntity; }

  VHDLComponent *findComponentByName(Glib::ustring name);

  VHDLSignal *findSignalByName(Glib::ustring name);

  VHDLInstance *findInstanceByName(Glib::ustring name);

  bool write(FILE *pFile, int indent);

  Glib::ustring getName() { return m_name; }
};

#endif /* _VHDL_ARCHITECTURE_H */
