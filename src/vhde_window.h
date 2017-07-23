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

class IStageUpdater;
class ITreeViewUpdater;

/* The responsibility of this class is to maintain a set of widgets and connect
 * the provided updater objects to the appropriate widget.
 */
class VHDEWindow : public Gtk::ApplicationWindow
{
public:
/* Until https://bugzilla.gnome.org/show_bug.cgi?id=783362 has been solved,
 * pass an Embed with a longer life-time than the window as a parameter.
 */
#ifdef CLUTTER_GTKMM_BUG
  VHDEWindow(std::unique_ptr<ITreeViewUpdater> pTreeViewUpdater,
             Clutter::Gtk::Embed &rClutterEmbed);
#else
  VHDEWindow(std::unique_ptr<ITreeViewUpdater> pTreeViewUpdater);
#endif
  virtual ~VHDEWindow();

  void setStageUpdater(std::unique_ptr<IStageUpdater> pStageUpdater);
  void setTreeViewUpdater();

private:
  void add_accelerator(Glib::RefPtr<Gtk::Builder> pBuilder, Glib::ustring menuItemName, guint accelKey, Gdk::ModifierType accelMods);

  bool on_treeview_focus_out_event(GdkEventFocus *pEvent, Gtk::TreeView *pTreeView);
  bool on_treeview_focus_in_event(GdkEventFocus *pEvent, Gtk::TreeView *pTreeView);
  bool on_stage_captured_event(Clutter::Event *pEvent);
  bool onKeyPressEvent(GdkEventKey *pEvent);
  void onActionEditDelete();

  std::vector<sigc::connection> m_subscriptions;
  sigc::connection m_updater_key_press_connection;

  Gtk::Box *m_pClutterEmbedBox;
#ifdef CLUTTER_GTKMM_BUG
  Clutter::Gtk::Embed &m_clutterEmbed;
#else
  Clutter::Gtk::Embed m_clutterEmbed;
#endif
  Glib::RefPtr<Clutter::Stage> m_stage;
  Gtk::TreeView *m_pTreeView;

  std::unique_ptr<IStageUpdater> m_pStageUpdater;
  std::unique_ptr<ITreeViewUpdater> m_pTreeViewUpdater;

  Gtk::TreeModel::Path m_previousSelection;
};

#endif

