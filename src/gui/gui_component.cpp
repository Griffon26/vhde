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

#include "gui_component.h"
#include "layout_component.h"
#include "vhdl_entity.h"

/*
 * Public methods
 */

GuiComponent::GuiComponent(Glib::RefPtr<Clutter::Stage> pStage, LayoutComponent *pComponent):
  GuiBlock(pStage, pComponent)
{
  printf("GuiComponent(%p): constructor body\n", this);

}

void GuiComponent::createPort(Edge edge, int position, Direction dir, const Glib::ustring &name)
{
  VHDLPort *pVHDLPort;
  LayoutPort *pLayoutPort;

  pVHDLPort = static_cast<VHDLEntity *>(thisLayoutComponent()->getAssociatedVHDLEntity())->createPort(dir, name);
  g_assert(pVHDLPort != NULL);

  pLayoutPort = thisLayoutComponent()->createPort(edge, position, pVHDLPort);
  g_assert(pLayoutPort != NULL);

  addPort(edge, position, pLayoutPort);
}

void GuiComponent::destroyPort(Edge edge, int position)
{
  LayoutPort *pLayoutPort;
  VHDLPort *pVHDLPort;
  printf("GuiComponent(%p)::destroyPort(%s %d)\n", this, EDGE_TO_NAME(edge), position);

  pLayoutPort = thisLayoutComponent()->getPort(edge, position);
  g_assert(pLayoutPort != NULL);
  pVHDLPort = static_cast<VHDLPort *>(pLayoutPort->getAssociatedVHDLPort());

  thisLayoutComponent()->destroyPort(edge, position);

  static_cast<VHDLEntity *>(thisLayoutComponent()->getAssociatedVHDLEntity())->destroyPort(pVHDLPort);

  removePort(edge, position);
}

/*
 * Private methods
 */

LayoutComponent *GuiComponent::thisLayoutComponent()
{
  return static_cast<LayoutComponent *>(m_pLayoutBlock);
}
