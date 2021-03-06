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

#ifndef _VHDL_COMPONENT_H
#define _VHDL_COMPONENT_H

#include <glibmm.h>

#include "i_named_item.h"
#include "vhdl_interface.h"

class VHDLEntity;

class VHDLComponent: public VHDLInterface
{
private:
  int m_refCount;
  VHDLEntity *m_pEntity;
  sigc::connection m_onPortAddedConnection;
  std::map<VHDLPort *, sigc::connection> m_onPortRemovedConnections;
  Glib::ustring m_unresolvedName;

public:
  VHDLComponent(const Glib::ustring &entityName);
  ~VHDLComponent();

  /* VHDLInstances should call the add/removeReference functions when they are
   * created/destroyed. The VHDLArchitecture uses this refcount to control the
   * lifetime of the VHDLComponent
   */
  void addReference();
  void removeReference();
  int getReferenceCount();

  VHDLComponent(const VHDLComponent &) = delete;
  VHDLComponent& operator=(const VHDLComponent&) = delete;

  /* Inherited methods */
  virtual bool write(std::ostream &outStream, int indent);

  void associateEntity(VHDLEntity *pEntity);
  VHDLEntity *getAssociatedEntity();

  const Glib::ustring &getName();

private:
  void onPortAdded(VHDLPort *pPort);
  void onPortRemoved(VHDLPort *pEntityPort, VHDLPort *pOurPort);
};

#endif /* _VHDL_COMPONENT_H */
