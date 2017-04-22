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

#ifndef _VHDL_FILE_H
#define _VHDL_FILE_H

#include <glibmm.h>

class VHDLArchitecture;
class VHDLEntity;
class VHDLFragment;

class VHDLFile
{
private:
  Glib::ustring m_name;

  VHDLFragment *m_pContext;
  VHDLEntity *m_pEntity;
  std::vector<VHDLArchitecture *> m_architectures;

public:
  VHDLFile();
  
  void setName(const Glib::ustring &name);
  Glib::ustring getName() { return m_name; }

  void setContext(VHDLFragment *pFragment);

  void setEntity(VHDLEntity *pEntity);
  VHDLEntity *getEntity() { return m_pEntity; }

  void addArchitecture(VHDLArchitecture *pArch);
  const std::vector<VHDLArchitecture *> getArchitectures();

  VHDLArchitecture *findArchitectureByName(Glib::ustring name);

  bool write(std::ostream &outStream, int indent);
};

#endif /* _VHDL_FILE_H */

