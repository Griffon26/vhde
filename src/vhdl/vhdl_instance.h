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

#ifndef _VHDL_INSTANCE_H
#define _VHDL_INSTANCE_H

#include <glibmm.h>
#include <map>

#include "i_named_item.h"
#include "vhdl_signal.h"

class VHDLComponent;

class VHDLInstance: public INamedItem
{
private:
  typedef struct
  {
    sigc::connection onPortRemovedConnection;
    sigc::connection onSignalRemovedConnection;
    VHDLSignal *pSignal;
  } MapEntry;

  Glib::ustring                             m_name;
  VHDLComponent                            *m_pComponent;
  std::list<Glib::ustring>                  m_genericMap;
  std::map<VHDLPort *, MapEntry>            m_portMap;

public:
  /* signals */
  //sigc::signal<void, VHDLSignal *, VHDLPort *> signal_disassociated;

  VHDLInstance(Glib::ustring name, VHDLComponent *pComponent);
  virtual ~VHDLInstance();

  void connectSignalToPort(VHDLSignal *pSignal, VHDLPort *pPort);
  void disconnectSignalFromPort(VHDLSignal *pSignal, VHDLPort *pPort);
  //void disconnectSignal(VHDLSignal *pSignal);

  bool write(std::ostream &outStream, int indent);

  const Glib::ustring &getName() { return m_name; }
  VHDLComponent *getComponent() { return m_pComponent; }

  std::vector<std::pair<VHDLPort *, VHDLSignal *>> *getPortsAndSignals();

private:
  void onPortRemoved(VHDLPort *pPort);
  void onSignalRemoved(VHDLSignal *pSignal, VHDLPort *pPort);
};

#endif /* _VHDL_INSTANCE_H */
