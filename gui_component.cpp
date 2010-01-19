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

void GuiComponent::createPort(int actionId, Edge edge, int position, Direction dir, const Glib::ustring &name)
{
  VHDLPort *pVHDLPort;
  LayoutPort *pLayoutPort;

  pVHDLPort = static_cast<VHDLEntity *>(thisLayoutComponent()->getAssociatedVHDLEntity())->createPort(actionId, dir, name);
  g_assert(pVHDLPort != NULL);

  pLayoutPort = thisLayoutComponent()->createPort(actionId, edge, position, pVHDLPort);
  g_assert(pLayoutPort != NULL);

  addPort(edge, position, pLayoutPort);
}

/*
 * Private methods
 */

LayoutComponent *GuiComponent::thisLayoutComponent()
{
  return static_cast<LayoutComponent *>(m_pLayoutBlock);
}
