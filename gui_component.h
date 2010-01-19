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

#ifndef _GUI_COMPONENT_H
#define _GUI_COMPONENT_H

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
private:

protected:

public:
  void createPort(int actionId, Edge edge, int position, Direction dir, const Glib::ustring &name);
  void removePort(int actionId, Edge edge, int position);
};

#endif /* _GUI_COMPONENT_H */
