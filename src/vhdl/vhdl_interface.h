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

#ifndef _VHDL_INTERFACE_H
#define _VHDL_INTERFACE_H

#include <glibmm.h>

#include "vhdl_port.h"

class VHDLFragment;

class VHDLInterface: public INamedItem
{
protected:
  bool                      m_init;
  std::unique_ptr<VHDLFragment> m_pGenerics;
  std::vector<std::unique_ptr<VHDLPort>> m_ports;

public:
  /* Signals */
  sigc::signal<void, VHDLPort *> port_added;

  VHDLInterface();
  VHDLInterface(const VHDLInterface&) = delete;
  VHDLInterface& operator=(const VHDLInterface&) = delete;

  /* These methods assume ownership of the objects passed */
  void init_addGenerics(std::unique_ptr<VHDLFragment> pFragment);
  void init_addPort(std::unique_ptr<VHDLPort> pPort);
  void init_done() { m_init = false; }

  VHDLPort *findPortByName(const Glib::ustring &name);

  virtual bool write(std::ostream &outStream, int indent) = 0;

  const std::vector<VHDLPort *> getPortList();

protected:
  void addPort(std::unique_ptr<VHDLPort> pPort);
  void removePort(VHDLPort *pPort);
};

#endif /* _VHDL_INTERFACE_H */
