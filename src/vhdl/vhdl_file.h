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

#include "vhdl_entity.h"
#include "vhdl_fragment.h"

class VHDLArchitecture;

class VHDLFile
{
public:
  typedef enum
  {
    GRAPHICAL,
    TEXT
  } Mode;

private:
  Glib::ustring m_name;
  Mode m_mode;

  std::unique_ptr<VHDLFragment> m_pContext;
  std::vector<std::unique_ptr<VHDLArchitecture>> m_architectures;
  std::unique_ptr<VHDLEntity> m_pEntity;
  std::vector<std::unique_ptr<VHDLFragment>> m_otherFragments;

public:
  VHDLFile(Mode mode);

  Mode getMode() { return m_mode; }
  
  void setName(const Glib::ustring &name);
  const Glib::ustring &getName() { return m_name; }

  void setContext(std::unique_ptr<VHDLFragment> pFragment);

  void setEntity(std::unique_ptr<VHDLEntity> pEntity);
  VHDLEntity *getEntity() { return m_pEntity.get(); }

  void addArchitecture(std::unique_ptr<VHDLArchitecture> pArch);
  const std::vector<VHDLArchitecture *> getArchitectures();
  VHDLArchitecture *findArchitectureByName(const Glib::ustring &name);

  void addOtherFragment(std::unique_ptr<VHDLFragment> pFragment);
  const std::vector<VHDLFragment *> getFragments();

  bool write(std::ostream &outStream, int indent);
};

#endif /* _VHDL_FILE_H */

