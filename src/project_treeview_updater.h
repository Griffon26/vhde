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

#ifndef _PROJECT_TREEVIEW_UPDATER_H
#define _PROJECT_TREEVIEW_UPDATER_H

#include <glibmm.h>
#include <gtkmm.h>

#include "project.h"
#include "i_treeview_updater.h"

class ProjectTreeViewUpdater: public ITreeViewUpdater
{
public:
  void setProject(Project *pProject);
  void setTreeView(Gtk::TreeView *pTreeView);

private:
  void onRowActivated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *pColumn);

public:
  sigc::signal<void, const Glib::ustring, int> item_activated;

private:
  class TreeStoreColumns: public Gtk::TreeModel::ColumnRecord
  {
  public:
    TreeStoreColumns()
    {
      add(name);
    }

    Gtk::TreeModelColumn<Glib::ustring> name;
  };

  TreeStoreColumns m_treeStoreColumns;

  Project *m_pProject;
  Glib::RefPtr<Gtk::TreeStore> m_pTreeStore;
};

#endif

