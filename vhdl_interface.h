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

#ifndef _VHDL_INTERFACE_H
#define _VHDL_INTERFACE_H

#include <glibmm.h>

#include "vhdl_generic.h"
#include "vhdl_port.h"

class VHDLInterface
{
protected:
  bool                      m_init;
  std::list<VHDLGeneric *>  m_generics;
  std::list<VHDLPort *>     m_ports;

public:
  /* Signals */
  sigc::signal<void, int, VHDLPort *> port_added;
  sigc::signal<void, int, VHDLPort *> port_removed;

  VHDLInterface();

  void init_addPort(VHDLPort *pPort);
  void init_done() { m_init = false; }

  VHDLPort *findPortByName(Glib::ustring name);

  virtual bool write(FILE *pFile, int indent) = 0;

  const std::list<VHDLPort *>        &getPorts()  { return m_ports; }

protected:
  /* This method assumes ownership of the port */
  void addPort(int actionId, VHDLPort *pPort);
  void removePort(int actionId, VHDLPort *pPort);
};

#endif /* _VHDL_INTERFACE_H */
