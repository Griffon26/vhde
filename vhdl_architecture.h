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

#ifndef _VHDL_ARCHITECTURE_H
#define _VHDL_ARCHITECTURE_H

#include <glibmm.h>
#include <stdio.h>

#include "vhdl_instance.h"
#include "vhdl_interface.h"
#include "vhdl_wire.h"

class VHDLArchitecture
{
private:
  Glib::ustring           m_name;
  VHDLInterface          *m_pEntity;
  std::list<VHDLWire>     m_signals;
  std::list<VHDLInstance> m_instances;

public:
  bool write(FILE *pFile, int indent);
};

#endif /* _VHDL_ARCHITECTURE_H */
