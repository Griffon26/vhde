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

#ifndef _LAYOUT_FILE_H
#define _LAYOUT_FILE_H

#include <glibmm.h>

#include "layout_component.h"

class LayoutArchitecture;

/* The only purpose of this class is to group other layout objects belonging to
 * one file so it is easy to write them all out in one go.
 */
class LayoutFile
{
private:
  std::unique_ptr<LayoutComponent> m_pComponent;
  std::vector<std::unique_ptr<LayoutArchitecture>> m_architectures;

public:
  LayoutFile();
  virtual ~LayoutFile() {}

  /* These methods assume ownership of the component and architectures */
  void setComponent(std::unique_ptr<LayoutComponent> pComponent);
  LayoutComponent *getComponent() { return m_pComponent.get(); }
  void addArchitecture(std::unique_ptr<LayoutArchitecture> pLayoutArch);
  const std::vector<LayoutArchitecture *> getArchitectures();

  void write(std::ostream &stream, int indent);
};

#endif /* _LAYOUT_FILE_H */

