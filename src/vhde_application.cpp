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
#include "vhde_application.h"
#include "vhde_window.h"
#include "vhdl_file.h"

Glib::RefPtr<VHDEApplication> VHDEApplication::create()
{
  return Glib::RefPtr<VHDEApplication>(new VHDEApplication());
}

VHDEApplication::VHDEApplication():
  Gtk::Application("com.kfk4ever.vhde")
{
}

void VHDEApplication::on_activate()
{
  /*
   * Read in some example files to create a model
   */
  m_project.addFile("test/top_entity.vhd", VHDLFile::GRAPHICAL);
  m_project.addFile("test/used_entity.vhd", VHDLFile::TEXT);
  m_project.resolveEntityReferences();
  m_project.resolveLayoutReferences();

  /* TODO: resolve the conflicts between the VHDL and the layout files, such as
   * absence of a .layout file
   */

  auto pArchStageUpdater = std::make_unique<ArchitectureStageUpdater>();
  auto pLayoutTopArch = m_project.getLayoutFile("test/top_entity.vhd")->getArchitectures()[0];
  pArchStageUpdater->setArchitecture(pLayoutTopArch);

#ifdef CLUTTER_GTKMM_BUG
  auto pWindow = new VHDEWindow(std::move(pArchStageUpdater), m_longLivedEmbed);
#else
  auto pWindow = new VHDEWindow(std::move(pArchStageUpdater));
#endif
  add_window(*pWindow);
  pWindow->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this, &VHDEApplication::on_hide_window), pWindow));

  pWindow->show_all();
}

void VHDEApplication::on_hide_window(Gtk::Window *pWindow)
{
  delete pWindow;

  /*
   * Write the model back out to files, suffixing each filename with a '2'
   * (to avoid making changes in versioned example files)
   */
  m_project.save();
}

