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

#ifndef _VHDL_SIGNAL_H
#define _VHDL_SIGNAL_H

#include <glibmm.h>
#include <stdio.h>

#include "i_named_item.h"
#include "vhdl_type.h"

class VHDLSignal: public INamedItem
{
private:
  Glib::ustring           m_name;
  VHDLType                m_type;
  Glib::ustring           m_defaultValue;

public:
  sigc::signal<void, VHDLSignal *> removed;

  VHDLSignal(const Glib::ustring &name);
  ~VHDLSignal();

  VHDLSignal(const VHDLSignal&) = delete;
  VHDLSignal& operator=(const VHDLSignal&) = delete;

  void setType(const VHDLType &type);
  void setDefaultValue(const Glib::ustring &defaultValue);

  bool write(std::ostream &outStream, int indent);

  const Glib::ustring &getName() { return m_name; }
};

#endif /* _VHDL_SIGNAL_H */
