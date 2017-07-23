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

#ifndef _GUI_SELECTION_H
#define _GUI_SELECTION_H

#include <unordered_set>

class GuiSelectable;

/**
 * The selection is responsible for making sure that all selected objects are
 * notified of events that affect them.
 *
 * Dragging:
 * - the effect of dragging an object can be different depending on the type of
 *   object; dragging a block will move it, dragging a signal will create
 *   and/or move a "waypoint" for this signal but keep the endpoints at the
 *   same position.
 * - the effect of dragging on an object is also influenced by whether any
 *   other objects are selected; dragging a signal on its own will manipulate a
 *   waypoint, but dragging it as part of a bigger selection will move the
 *   endpoints (either by moving the object it is connected to or by moving it
 *   itself if it's unconnected)
 *
 * Keyboard input, delete:
 * - pressing delete will remove all selected objects from the diagram
 *
 * Mouse clicks:
 * - when clicking outside any objects, the selection should become empty
 * - when clicking on a component, only that component should become selected
 * - when shift-clicking on a component, that component should be toggled into/out of the selection
 * - when dragging a frame, only the components within the frame should become selected
 * - when shift-dragging a frame, the components within the frame should be added to the selection
 *
 */
class GuiSelection
{
public:
  void add(GuiSelectable *);
  void remove(GuiSelectable *);
  void clear();

  /* Convenience functions */
  void toggle(GuiSelectable *);
  void set(GuiSelectable *); // sets selection to this one object

private:
  std::unordered_set<GuiSelectable *> m_selectedObjects;
};

#endif /* _GUI_SELECTION_H */
