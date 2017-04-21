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

#ifndef _LAYOUT_COMPONENT_H
#define _LAYOUT_COMPONENT_H

#include "layout_block.h"
#include "layout_types.h"

class INamedItem;

class LayoutComponent: public LayoutBlock
{
private:
  INamedItem       *m_pVHDLEntity;

public:
  LayoutComponent();

  void associateEntity(INamedItem *pVHDLEntity);
  INamedItem *getAssociatedVHDLEntity();

  /* This method retains ownership of the returned LayoutPort */
  LayoutPort *createPort(Edge edge, int position, INamedItem *pVHDLPort);
  void destroyPort(Edge edge, int position);

  void write(std::ostream &stream);
};

#endif /* _LAYOUT_COMPONENT_H */
