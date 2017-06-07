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

#ifndef _VHDE_WINDOW_H
#define _VHDE_WINDOW_H

#include <clutter-gtkmm.h>
#include <cluttermm.h>
#include <gtkmm.h>

#include "gui_component.h"
#include "gui_instance.h"
#include "gui_signal.h"

class Project;

class VHDEWindow : public Gtk::ApplicationWindow
{
public:
/* Until https://bugzilla.gnome.org/show_bug.cgi?id=783362 has been solved,
 * pass an Embed with a longer life-time than the window as a parameter.
 */
#ifdef CLUTTER_GTKMM_BUG
  VHDEWindow(Project *pProject, Clutter::Gtk::Embed &m_clutterEmbed);
#else
  VHDEWindow(Project *pProject);
#endif
  virtual ~VHDEWindow();

private:
  bool on_idle_hide_window();
  bool on_key_pressed(Clutter::KeyEvent *pEvent, GuiComponent *pGuiComponent);

#ifndef CLUTTER_GTKMM_BUG
  Clutter::Gtk::Embed m_clutterEmbed;
#endif
  Glib::RefPtr<Clutter::Stage> m_stage;
  sigc::connection m_capture_connection;
  sigc::connection m_key_press_connection;

  /* Temporary member variables. Eventually these objects are to be created
   * dynamically based on the model. */
  std::unique_ptr<GuiSignal> m_pGuiSignal;
  std::unique_ptr<GuiInstance> m_pGuiInstance;
  std::unique_ptr<GuiComponent> m_pGuiComponent;
};

#endif

