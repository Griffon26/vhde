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

#ifndef _GUI_SELECTABLE_H
#define _GUI_SELECTABLE_H

#include <iostream>

/**
 * A class that Gui classes can inherit from to deal with the aspects of being selected.
 */
class GuiSelectable
{
public:
  GuiSelectable():
    m_selected(false)
  {
  }

  /* This function will be called when the Gui object becomes selected or unselected.
   * It automatically calls the queue_draw() function to be implemented by the Gui class.
   * There is no need to override this method.
   */
  virtual void select(bool selectionState)
  {
    m_selected = selectionState;
    queueRedraw();
  }

protected:
  /* Method to be implemented by all selectable Gui classes. This should queue
   * a redraw for all clutter actors that are drawn differently based on
   * selection state. The draw function can use the m_selected boolean to
   * determine how to draw the object.
   */
  virtual void queueRedraw() = 0;

protected:
  /* A boolean that indicates whether the current object is part of the selection */
  bool m_selected;
};

#endif /* _GUI_SELECTABLE_H */
