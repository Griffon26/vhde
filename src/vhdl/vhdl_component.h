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

#include "vhdl_interface.h"

class VHDLEntity;

class VHDLComponent: public VHDLInterface, public INamedItem
{
private:
  VHDLEntity *m_pEntity;
  sigc::connection m_onPortAddedConnection;
  Glib::ustring m_unresolvedName;

public:
  VHDLComponent(Glib::ustring entityName);
  ~VHDLComponent();

  /* Inherited methods */
  virtual bool write(FILE *pFile, int indent);

  void associateEntity(VHDLEntity *pEntity);
  VHDLEntity *getAssociatedEntity();

  const Glib::ustring &getName();

private:
  void onPortAdded(VHDLPort *pPort);
  void onPortRemoved(VHDLPort *pEntityPort, VHDLPort *pOurPort);
};

#endif /* _VHDL_COMPONENT_H */
