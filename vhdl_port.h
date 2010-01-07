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

#ifndef _VHDL_PORT_H
#define _VHDL_PORT_H

#include <glibmm.h>
#include <stdio.h>

#include "vhdl_type.h"

class VHDLPort
{
public:
  typedef enum {
    DIR_IN,
    DIR_OUT,
    DIR_INOUT,
    DIR_INVALID
  } Direction;
private:
  Glib::ustring           m_name;
  Direction               m_direction;
  VHDLType                m_type;

public:
  VHDLPort(Glib::ustring name);

  void setDirection(Direction dir);
  void setType(VHDLType type);

  bool write(FILE *pFile, int indent);

  const Glib::ustring   getName()       { return m_name; }
  const Direction       getDirection()  { return m_direction; }
};

#endif /* _VHDL_WIRE_H */
