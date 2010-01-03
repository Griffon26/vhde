/* vim: ts=2:sw=2:expandtab
 *
 * Copyright 2009 Maurice van der Pot <griffon26@kfk4ever.com>
 *
 * This file is part of Foobar.
 *
 * Foobar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _VHDL_INSTANCE_H
#define _VHDL_INSTANCE_H

#include <glibmm.h>
#include <map>

#include "vhdl_interface.h"
#include "vhdl_signal.h"

class VHDLInstance
{
private:
  Glib::ustring                             m_name;
  VHDLInterface                            *m_pComponent;
  std::list<Glib::ustring>                  m_genericMap;
  std::map<VHDLPort *, VHDLSignal *>        m_portMap;

  sigc::connection                          m_onPortRemovedConnection;

public:
  VHDLInstance(Glib::ustring name, VHDLInterface *pComponent);
  virtual ~VHDLInstance();

  void associateSignalWithPort(VHDLSignal *pSignal, VHDLPort *pPort);
  void disassociateSignalWithPort(VHDLSignal *pSignal, VHDLPort *pPort);
  void disassociateSignal(VHDLSignal *pSignal);

  bool write(FILE *pFile, int indent);

  const Glib::ustring getName() { return m_name; }
  VHDLInterface *getComponent() { return m_pComponent; }

private:
  void onPortRemoved(VHDLPort *pPort);
};

#endif /* _VHDL_INSTANCE_H */
