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

#ifndef _GUI_COMPONENT_H
#define _GUI_COMPONENT_H

#include "gui_block.h"

class LayoutComponent;

/**
 * A class that manages the GUI of a VHDL component interface.
 *
 * GuiComponent is meant to be used in a diagram of a component interface as
 * opposed to a diagram with component instances. It is therefore not possible
 * to change its position on the canvas.  Use GuiInstance instead if you need
 * component instance specific operations like that.
 */
class GuiComponent: public GuiBlock
{
public:
  GuiComponent(Glib::RefPtr<Clutter::Stage> pStage, LayoutComponent *pComponent);

  void createPort(int actionId, Edge edge, int position, Direction dir, const Glib::ustring &name);
  void destroyPort(int actionId, Edge edge, int position);

private:
  LayoutComponent *thisLayoutComponent();
};

#endif /* _GUI_COMPONENT_H */
