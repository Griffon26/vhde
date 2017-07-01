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

#ifndef _VHDE_APPLICATION_H
#define _VHDE_APPLICATION_H

#ifdef CLUTTER_GTKMM_BUG
#include <clutter-gtkmm.h>
#endif
#include <gtkmm.h>

#include "project.h"

class VHDEWindow;

/* The responsibilities of this class are:
 * - to create and manage windows
 * - to create updater objects that are the link between the data model and the
 *   widgets that display it, and pass them to the windows it creates
 */
class VHDEApplication: public Gtk::Application
{
protected:
  VHDEApplication();

public:
  static Glib::RefPtr<VHDEApplication> create();

protected:
  void on_activate() override;

private:
  void on_item_activated(const Glib::ustring fileName, int itemIndex);
  void on_hide_window();
  void onActionFileNew();
  void onActionFileOpen();
  void onActionFileSave();
  void onActionFileQuit();

  VHDEWindow *m_pWindow;
  Project m_project;
#ifdef CLUTTER_GTKMM_BUG
  Clutter::Gtk::Embed m_longLivedEmbed;
#endif
};

#endif

