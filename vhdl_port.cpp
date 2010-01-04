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

#include "vhdl_port.h"

VHDLPort::VHDLPort(Glib::ustring name):
  m_name(name),
  m_direction(DIR_INVALID)
{
}

void VHDLPort::setDirection(Direction dir)
{
  g_assert(m_direction == DIR_INVALID);
  m_direction = dir;
}

void VHDLPort::setType(VHDLType type)
{
  m_type = type;
}

bool VHDLPort::write(FILE *pFile, int indent)
{
  fprintf(pFile, "%*s%s: %s %s;\n", indent, "", m_name.c_str(),
                                    (m_direction == DIR_IN) ? "in" :
                                    (m_direction == DIR_OUT) ? "out" :
                                                               "inout", "sometype");
  return true;
}
