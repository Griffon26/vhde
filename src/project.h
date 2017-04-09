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

class LayoutArchitecture;
class LayoutResolverActions;
class VHDLArchitecture;
class VHDLEntity;

class Project
{
private:
  std::map<std::string, VHDLEntity *> m_entityMap;

  /* These maps use the base name of the VHDL file as key */
  std::map<std::string, VHDLArchitecture *> m_fileToVHDLArchMap;
  std::map<std::string, LayoutArchitecture *> m_fileToLayoutArchMap;
  std::map<std::string, LayoutResolverActions *> m_layoutResolverMap;

  VHDLArchitecture *readVHDLFromFile(std::string fileName);
  LayoutArchitecture *readLayoutFromFile(std::string fileName, LayoutResolverActions *pLayoutResolverActions);

public:
  void addFile(std::string fileName);
  void resolveEntityReferences();
  void resolveLayoutReferences();

  LayoutArchitecture *getLayoutArchitecture(std::string fileName);

  /* temporary method for testing if parsing and saving is done correctly */
  void save();
};

#endif

