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

#ifndef _I_TREEVIEW_UPDATER_H
#define _I_TREEVIEW_UPDATER_H

#include <glibmm.h>
#include <gtkmm.h>

/*
 * This interface is to be implemented by any object that performs all updates
 * to a Treeview widget. The window will pass its treeview to this object and
 * destroy the object itself when it is no longer needed.
 */
class ITreeViewUpdater
{
public:
  virtual ~ITreeViewUpdater() {};
  virtual void setTreeView(Gtk::TreeView *pTreeView) = 0;
};

#endif

