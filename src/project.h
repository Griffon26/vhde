/* vim: ts=2:sw=2:expandtab
 *
 * VHDE - The VHDL Diagram Editor
 * Copyright (C) 2009-2017  Maurice van der Pot <griffon26@kfk4ever.com>
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

#ifndef _PROJECT_H
#define _PROJECT_H

#include <map>
#include <string>

#include "parse_layout.h"
#include "vhdl_file.h"

class LayoutArchitecture;
class LayoutFile;
class VHDLArchitecture;
class VHDLEntity;
class VHDLInterface;

class Project
{
private:
  std::map<const Glib::ustring, VHDLEntity *> m_entityMap;

  /* These maps use the base name of the VHDL file as key */
  std::map<const Glib::ustring, std::unique_ptr<VHDLFile>> m_fileToVHDLFileMap;
  std::map<const Glib::ustring, std::unique_ptr<LayoutFile>> m_fileToLayoutFileMap;
  std::map<const Glib::ustring, std::unique_ptr<LayoutResolverActions>> m_layoutResolverMap;

  static std::unique_ptr<VHDLFile> readVHDLFromFile(const Glib::ustring &fileName, VHDLFile::Mode mode, std::map<const Glib::ustring, VHDLEntity *> &entityMap);

  std::unique_ptr<LayoutFile> readLayoutFromFile(const Glib::ustring &fileName, LayoutResolverActions *pLayoutResolverActions);
  void createDefaultPorts(VHDLInterface *pVHDLInterface, LayoutBlock *pLayoutBlock);
  std::unique_ptr<LayoutArchitecture> createDefaultArchitectureLayout(VHDLArchitecture *pArch);
  std::unique_ptr<LayoutFile> createDefaultFileLayout(VHDLFile *pVHDLFile);

public:
  void addFile(const Glib::ustring &fileName, VHDLFile::Mode mode);
  void resolveEntityReferences();
  void resolveLayoutReferences();
  void resolveLayoutComponentReferences();

  LayoutFile *getLayoutFile(const Glib::ustring &fileName);

  /* temporary method for testing if parsing and saving is done correctly */
  void save();
};

#endif

