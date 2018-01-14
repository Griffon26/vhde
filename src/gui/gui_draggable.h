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

#ifndef _GUI_DRAGGABLE_H
#define _GUI_DRAGGABLE_H

#include <cluttermm.h>
#include "gui_selectable.h"

class GuiDraggable: public GuiSelectable
{
public:
  GuiDraggable(Type type):
    GuiSelectable(type)
  {
  }

  virtual void startMove() {}
  virtual void updateMove(int offsetx, int offsety) {}
  virtual void finishMove() {}

  virtual void startResize() = 0;
  virtual void updateResize(int offsetx, int offsety) = 0;
  virtual void finishResize() = 0;

  virtual void click(int modifiers) = 0;

public:
  sigc::signal<void, Clutter::ButtonEvent *, GuiDraggable *> dragStarted;
};

#endif /* _GUI_DRAGGABLE_H */

