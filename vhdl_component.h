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

#ifndef _VHDL_COMPONENT_H
#define _VHDL_COMPONENT_H

#include <glibmm.h>

#include "vhdl_entity.h"
#include "vhdl_interface.h"

class VHDLComponent: public VHDLInterface
{
private:
  VHDLEntity *m_pEntity;
  sigc::connection m_onNameChangedConnection;
  sigc::connection m_onPortRemovedConnection;
  sigc::connection m_onPortAddedConnection;

public:
  VHDLComponent(Glib::ustring name);
  ~VHDLComponent();

  /* Inherited methods */
  virtual bool write(FILE *pFile, int indent);

  void associateEntity(VHDLEntity *pEntity);

private:
  void onNameChanged(Glib::ustring newName);
  void onPortAdded(VHDLPort *pPort);
  void onPortRemoved(VHDLPort *pPort);
};

#endif /* _VHDL_COMPONENT_H */
