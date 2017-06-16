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

#include "project_treeview_updater.h"

void ProjectTreeViewUpdater::setTreeView(Gtk::TreeView *pTreeView)
{
  m_pTreeStore = Gtk::TreeStore::create(m_treeStoreColumns);
  pTreeView->set_model(m_pTreeStore);
  pTreeView->append_column("Name", m_treeStoreColumns.name);

  auto parentRow = *m_pTreeStore->append();
  parentRow[m_treeStoreColumns.name] = "parent2";
  auto childRow = *m_pTreeStore->append(parentRow.children());
  childRow[m_treeStoreColumns.name] = "child2";
}

