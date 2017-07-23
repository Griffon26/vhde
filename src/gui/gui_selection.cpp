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

#include "gui_selection.h"
#include "gui_selectable.h"

void GuiSelection::add(GuiSelectable *pGuiSelectable)
{
  pGuiSelectable->select(true);
  m_selectedObjects.insert(pGuiSelectable);
}

void GuiSelection::remove(GuiSelectable *pGuiSelectable)
{
  pGuiSelectable->select(false);
  m_selectedObjects.erase(pGuiSelectable);
}

void GuiSelection::clear()
{
  for(auto pGuiSelectable: m_selectedObjects)
  {
    pGuiSelectable->select(false);
  }
  m_selectedObjects.clear();
}

void GuiSelection::toggle(GuiSelectable *pGuiSelectable)
{
  if(m_selectedObjects.count(pGuiSelectable))
  {
    remove(pGuiSelectable);
  }
  else
  {
    add(pGuiSelectable);
  }
}

void GuiSelection::set(GuiSelectable *pGuiSelectable)
{
  clear();
  add(pGuiSelectable);
}

