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

#ifndef _VHDL_FRAGMENT_H
#define _VHDL_FRAGMENT_H

#include <glibmm.h>

class VHDLFragment
{
private:
  Glib::ustring           m_text;

public:
  VHDLFragment(const Glib::ustring &text);
  virtual ~VHDLFragment();

  VHDLFragment(const VHDLFragment&) = delete;
  VHDLFragment& operator=(const VHDLFragment&) = delete;

  const Glib::ustring &getText();

  bool write(std::ostream &outStream, int indent);
};

#endif /* _VHDL_FRAGMENT_H */

