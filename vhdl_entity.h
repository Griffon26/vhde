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

#ifndef _VHDL_ENTITY_H
#define _VHDL_ENTITY_H

#include <glibmm.h>

#include "i_named_item.h"
#include "vhdl_interface.h"

class VHDLEntity: public VHDLInterface, public INamedItem
{
private:
  Glib::ustring             m_name;

public:
  /* Signals */
  sigc::signal<void, Glib::ustring> name_changed;

  VHDLEntity(Glib::ustring name);

  void                setName(Glib::ustring name);
  const Glib::ustring &getName();


  VHDLPort *createPort(Direction dir, const Glib::ustring &name);
  void destroyPort(VHDLPort *pPort);

  bool write(FILE *pFile, int indent);
};

#endif /* _VHDL_ENTITY_H */
