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
VHDEWindow::VHDEWindow(std::unique_ptr<IStageUpdater> pStageUpdater,
                       std::unique_ptr<ITreeViewUpdater> pTreeViewUpdater,
                       Clutter::Gtk::Embed &rClutterEmbed):
  Gtk::ApplicationWindow(),
  m_pClutterEmbedBox(nullptr),
  m_clutterEmbed(rClutterEmbed),
  m_pTreeViewUpdater(std::move(pTreeViewUpdater))
#else
VHDEWindow::VHDEWindow(std::unique_ptr<IStageUpdater> pStageUpdater,
                       std::unique_ptr<ITreeViewUpdater> pTreeViewUpdater):
  Gtk::ApplicationWindow(),
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

  pBuilder->get_widget("clutterEmbedBox", m_pClutterEmbedBox);
  m_pClutterEmbedBox->add(m_clutterEmbed);

  Gtk::TreeView *pTreeView;
  pBuilder->get_widget("treeview", pTreeView);
  pTreeView->signal_focus_in_event().connect(sigc::bind(sigc::mem_fun(this, &VHDEWindow::on_treeview_focus_in_event), pTreeView));
  pTreeView->signal_focus_out_event().connect(sigc::bind(sigc::mem_fun(this, &VHDEWindow::on_treeview_focus_out_event), pTreeView));

  m_pTreeViewUpdater->setTreeView(pTreeView);

  set_default_size(1300, 700);

  m_stage = m_clutterEmbed.get_stage();
  m_stage->set_color(STAGE_COLOR);
  m_stage->set_motion_events_enabled(false);

  signal_key_press_event().connect(sigc::mem_fun(this, &VHDEWindow::onKeyPressEvent));

  m_stage_captured_event_connection = m_stage->signal_captured_event().connect(sigc::mem_fun(this, &VHDEWindow::on_stage_captured_event));

  setStageUpdater(std::move(pStageUpdater));
}

VHDEWindow::~VHDEWindow()
{
  /* Force removal of the updater and thereby registration to events, when the window is closed */
  m_pStageUpdater = nullptr;
  m_stage_captured_event_connection.disconnect();
}

void VHDEWindow::setStageUpdater(std::unique_ptr<IStageUpdater> pStageUpdater)
{
  m_updater_key_press_connection.disconnect();

  m_pStageUpdater = std::move(pStageUpdater);

  /* This is a workaround that allows us to handle key events in the stage updater or in the treeview, depending on input focus.
   * When migrating away from Clutter there will hopefully be a way for the updater to connect to key press events on the canvas.
   */
  m_updater_key_press_connection = m_pClutterEmbedBox->signal_key_press_event().connect(sigc::mem_fun(m_pStageUpdater.get(), &IStageUpdater::onKeyPressEvent));


  m_pStageUpdater->setStage(m_stage);
}

bool VHDEWindow::onKeyPressEvent(GdkEventKey *pEvent)
{
  printf("VHDEWindow::onKeyPressEvent\n");
  if(pEvent->keyval == 'q')
  {
    printf("Exiting...\n");

    hide();
    return HANDLED;
  }
  printf("  key not handled\n");
  return UNHANDLED;
}


