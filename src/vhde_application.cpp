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

#include "architecture_stage_updater.h"
#include "entity_stage_updater.h"
#include "project_treeview_updater.h"
#include "vhde_application.h"
#include "vhde_window.h"
#include "vhdl_file.h"

Glib::RefPtr<VHDEApplication> VHDEApplication::create()
{
  return Glib::RefPtr<VHDEApplication>(new VHDEApplication());
}

VHDEApplication::VHDEApplication():
  Gtk::Application("com.kfk4ever.vhde"),
  m_pWindow(nullptr)
{
}

void VHDEApplication::on_activate()
{
  auto pTreeViewUpdater = std::make_unique<ProjectTreeViewUpdater>();
  pTreeViewUpdater->setProject(&m_project);
  pTreeViewUpdater->item_activated.connect(sigc::mem_fun(*this, &VHDEApplication::on_item_activated));

  add_action("new", sigc::mem_fun(*this, &VHDEApplication::onActionFileNew));
  add_action("open", sigc::mem_fun(*this, &VHDEApplication::onActionFileOpen));
  add_action("save", sigc::mem_fun(*this, &VHDEApplication::onActionFileSave));
  add_action("quit", sigc::mem_fun(*this, &VHDEApplication::onActionFileQuit));

#ifdef CLUTTER_GTKMM_BUG
  m_pWindow = new VHDEWindow(std::move(pTreeViewUpdater),
                                m_longLivedEmbed);
#else
  m_pWindow = new VHDEWindow(std::move(pTreeViewUpdater));
#endif

  add_window(*m_pWindow);
  m_pWindow->signal_hide().connect(sigc::mem_fun(*this, &VHDEApplication::on_hide_window));

  m_pWindow->show_all();
}

void VHDEApplication::on_item_activated(const Glib::ustring fileName, int itemIndex)
{
  auto pLayoutFile = m_project.getLayoutFile(fileName);

  if(itemIndex == 0)
  {
    auto pStageUpdater = std::make_unique<EntityStageUpdater>();
    pStageUpdater->setEntity(pLayoutFile->getComponent());
    m_pWindow->setStageUpdater(std::move(pStageUpdater));
  }
  else
  {
    auto pStageUpdater = std::make_unique<ArchitectureStageUpdater>();
    pStageUpdater->setArchitecture(pLayoutFile->getArchitectures()[itemIndex - 1]);
    m_pWindow->setStageUpdater(std::move(pStageUpdater));
  }
}

void VHDEApplication::on_hide_window()
{
  delete m_pWindow;
  m_pWindow = nullptr;
}

void VHDEApplication::onActionFileNew()
{
  m_pWindow->setStageUpdater(nullptr);
  m_project.clear();
}

void VHDEApplication::onActionFileOpen()
{
  Gtk::FileChooserDialog dialog(*m_pWindow, "Open VHDE Project", Gtk::FILE_CHOOSER_ACTION_OPEN);

  auto pFilter = Gtk::FileFilter::create();
  pFilter->add_pattern("*.vhde");
  pFilter->set_name("VHDE Project Files");
  dialog.add_filter(pFilter);

  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Open", Gtk::RESPONSE_OK);

  int result = dialog.run();

  if(result == Gtk::RESPONSE_OK)
  {
    onActionFileNew();
    m_project.load(dialog.get_filename());
  }
}

void VHDEApplication::onActionFileSave()
{
  m_project.save();
}

void VHDEApplication::onActionFileQuit()
{
  m_pWindow->hide();
}

