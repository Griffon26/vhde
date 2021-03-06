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

#include <iostream>
#include "project_treeview_updater.h"

ProjectTreeViewUpdater::ProjectTreeViewUpdater():
  m_pProject(nullptr),
  m_pTreeView(nullptr)
{
  std::cout << "ProjectTreeViewUpdater::ProjectTreeViewUpdater\n";
}

ProjectTreeViewUpdater::~ProjectTreeViewUpdater()
{
  std::cout << "ProjectTreeViewUpdater::~ProjectTreeViewUpdater\n";
  m_projectChangedConnection.disconnect();
  m_treeViewRowActivatedConnection.disconnect();
}

void ProjectTreeViewUpdater::onProjectChanged()
{
  repopulateTreeView();
}

void ProjectTreeViewUpdater::setProject(Project *pProject)
{
  m_pProject = pProject;
  m_projectChangedConnection = m_pProject->changed.connect(sigc::mem_fun(*this, &ProjectTreeViewUpdater::onProjectChanged));
}

void ProjectTreeViewUpdater::onRowActivated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *pColumn)
{
  if(path.size() == 2)
  {
    auto fileNames = m_pProject->getFileNames();
    item_activated.emit(fileNames[path[0]], path[1]);
  }
}

void ProjectTreeViewUpdater::setTreeView(Gtk::TreeView *pTreeView)
{
  g_assert(m_pProject);

  m_pTreeView = pTreeView;

  m_pTreeStore = Gtk::TreeStore::create(m_treeStoreColumns);
  m_pTreeView->set_model(m_pTreeStore);
  m_pTreeView->append_column("Name", m_treeStoreColumns.name);

  m_treeViewRowActivatedConnection = m_pTreeView->signal_row_activated().connect(sigc::mem_fun(*this, &ProjectTreeViewUpdater::onRowActivated));

  repopulateTreeView();
}

void ProjectTreeViewUpdater::repopulateTreeView()
{
  m_pTreeStore->clear();

  auto fileNames = m_pProject->getFileNames();
  for(auto &pFileName: fileNames)
  {
    auto parentRow = *m_pTreeStore->append();
    parentRow[m_treeStoreColumns.name] = pFileName;

    auto childRow = *m_pTreeStore->append(parentRow.children());
    auto pLayoutFile = m_pProject->getLayoutFile(pFileName);

    auto name = pLayoutFile->getComponent()->getName();
    childRow[m_treeStoreColumns.name] = name;

    for(auto &pArch: pLayoutFile->getArchitectures())
    {
      childRow = *m_pTreeStore->append(parentRow.children());
      name = pArch->getName();
      childRow[m_treeStoreColumns.name] = name;
    }
  }

  m_pTreeView->expand_all();

  if(fileNames.size() != 0)
  {
    item_activated.emit(fileNames[0], 0);
  }
}

