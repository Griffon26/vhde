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

#include "i_stage_updater.h"
#include "i_treeview_updater.h"
#include "gui_common.h"
#include "gui_component.h"
#include "gui_instance.h"
#include "gui_signal.h"
#include "project.h"
#include "vhde_window.h"

#define COMPONENT_WIDTH      200
#define COMPONENT_HEIGHT     300
#define COMPONENT_INITIAL_X  100
#define COMPONENT_INITIAL_Y  100

bool VHDEWindow::on_treeview_focus_out_event(GdkEventFocus *pEvent, Gtk::TreeView *pTreeView)
{
  auto selection = pTreeView->get_selection();
  if(selection->get_selected_rows().size() != 0)
  {
    m_previousSelection = selection->get_selected_rows()[0];
  }
  pTreeView->get_selection()->unselect_all();
  return false;
}

bool VHDEWindow::on_treeview_focus_in_event(GdkEventFocus *pEvent, Gtk::TreeView *pTreeView)
{
  auto selection = pTreeView->get_selection();
  if(selection->get_selected_rows().size() == 0)
  {
    selection->select(m_previousSelection);
  }
  return false;
}

bool VHDEWindow::on_stage_captured_event(Clutter::Event *pEvent)
{
  if(pEvent->type == CLUTTER_BUTTON_PRESS)
  {
    std::cout << "Stage clicked" << std::endl;
    m_pClutterEmbedBox->grab_focus();
  }
  return false;
}

#ifdef CLUTTER_GTKMM_BUG
VHDEWindow::VHDEWindow(std::unique_ptr<ITreeViewUpdater> pTreeViewUpdater,
                       Clutter::Gtk::Embed &rClutterEmbed):
  Gtk::ApplicationWindow(),
  m_pClutterEmbedBox(nullptr),
  m_clutterEmbed(rClutterEmbed),
  m_pTreeViewUpdater(std::move(pTreeViewUpdater))
#else
VHDEWindow::VHDEWindow(std::unique_ptr<ITreeViewUpdater> pTreeViewUpdater):
  Gtk::ApplicationWindow(),
  m_pClutterEmbedBox(nullptr),
  m_pTreeViewUpdater(std::move(pTreeViewUpdater))
#endif
{
  /*
   * Construct the widgets in this window
   */

  Glib::RefPtr<Gtk::Builder> pBuilder = Gtk::Builder::create_from_file("glade/window.glade");

  Gtk::Box *pBox;
  pBuilder->get_widget("toplevelbox", pBox);
  add(*pBox);

  add_action("delete", sigc::mem_fun(*this, &VHDEWindow::onActionEditDelete));

  // For now define accelerators here in code instead of in the glade file,
  // because I could not find a way to have glade add the accelerators to an
  // accelgroup that I can connect to VHDEWindow.
  add_accelerator(pBuilder, "menuFileQuit", 'q', Gdk::CONTROL_MASK);
  add_accelerator(pBuilder, "menuFileSave", 's', Gdk::CONTROL_MASK);
  add_accelerator(pBuilder, "menuEditCut", 'x', Gdk::CONTROL_MASK);
  add_accelerator(pBuilder, "menuEditCopy", 'c', Gdk::CONTROL_MASK);
  add_accelerator(pBuilder, "menuEditPaste", 'v', Gdk::CONTROL_MASK);
  add_accelerator(pBuilder, "menuEditDelete", GDK_KEY_Delete, (Gdk::ModifierType)0);

  pBuilder->get_widget("clutterEmbedBox", m_pClutterEmbedBox);
  m_pClutterEmbedBox->add(m_clutterEmbed);

  pBuilder->get_widget("treeview", m_pTreeView);
  m_subscriptions.push_back(m_pTreeView->signal_focus_in_event().connect(
        sigc::bind(sigc::mem_fun(this, &VHDEWindow::on_treeview_focus_in_event), m_pTreeView)));
  m_subscriptions.push_back(m_pTreeView->signal_focus_out_event().connect(
        sigc::bind(sigc::mem_fun(this, &VHDEWindow::on_treeview_focus_out_event), m_pTreeView)));

  m_pTreeViewUpdater->setTreeView(m_pTreeView);

  set_default_size(1300, 700);

  m_stage = m_clutterEmbed.get_stage();
  m_stage->set_color(STAGE_COLOR);

/* This is a workaround for the old versions of deps used by Travis. The
 * function is available in clutter 1.16.4, but the cluttermm version that
 * still supports 1.16.4 does not expose it.
 */
#if CLUTTER_MINOR_VERSION > 16
  m_stage->set_motion_events_enabled(false);
#endif

  m_subscriptions.push_back(signal_key_press_event().connect(sigc::mem_fun(this, &VHDEWindow::onKeyPressEvent)));
  m_subscriptions.push_back(m_stage->signal_captured_event().connect(sigc::mem_fun(this, &VHDEWindow::on_stage_captured_event)));

}

VHDEWindow::~VHDEWindow()
{
  /* Force removal of the updater and thereby registration to events, when the window is closed */
  m_pStageUpdater = nullptr;

  for(auto &subscription: m_subscriptions)
  {
    subscription.disconnect();
  }
  m_updater_key_press_connection.disconnect();
}

void VHDEWindow::setStageUpdater(std::unique_ptr<IStageUpdater> pStageUpdater)
{
  m_updater_key_press_connection.disconnect();

  m_pStageUpdater = std::move(pStageUpdater);

  if(m_pStageUpdater)
  {
    /* This is a workaround that allows us to handle key events in the stage updater or in the treeview, depending on input focus.
     * When migrating away from Clutter there will hopefully be a way for the updater to connect to key press events on the canvas.
     */
    m_updater_key_press_connection = m_pClutterEmbedBox->signal_key_press_event().connect(sigc::mem_fun(m_pStageUpdater.get(), &IStageUpdater::onKeyPressEvent));

    m_pStageUpdater->setStage(m_stage);
  }
}

void VHDEWindow::add_accelerator(Glib::RefPtr<Gtk::Builder> pBuilder, Glib::ustring menuItemName, guint accelKey, Gdk::ModifierType accelMods)
{
  auto pAccelGroup = get_accel_group();

  Gtk::MenuItem *pMenuItem;
  pBuilder->get_widget(menuItemName, pMenuItem);
  pMenuItem->add_accelerator("activate", pAccelGroup, accelKey, accelMods, Gtk::ACCEL_VISIBLE);
}

bool VHDEWindow::onKeyPressEvent(GdkEventKey *pEvent)
{
  return UNHANDLED;
}

void VHDEWindow::onActionEditDelete()
{
  if(m_pStageUpdater)
  {
    m_pStageUpdater->deleteSelection();
  }
}

