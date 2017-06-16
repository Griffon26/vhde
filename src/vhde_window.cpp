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


#define FACTOR  1.2

static bool on_my_captured_event(Clutter::Event* pEvent, Glib::RefPtr<Clutter::Stage> pStage)
{
  if(pEvent->type == CLUTTER_SCROLL)
  {
    if(pEvent->scroll.modifier_state & CLUTTER_CONTROL_MASK)
    {
      int scaleDirection;

      switch(pEvent->scroll.direction)
      {
      case CLUTTER_SCROLL_UP:
        scaleDirection = 1;
        break;
      case CLUTTER_SCROLL_DOWN:
        scaleDirection = -1;
        break;
      case CLUTTER_SCROLL_SMOOTH:
        printf("Support for CLUTTER_SCROLL_SMOOTH events has not yet been implemented.\n");
        scaleDirection = 0;
        break;
      default:
        scaleDirection = 0;
        break;
      }

      if(scaleDirection != 0)
      {
        double scale_x, scale_y;
        printf("scaling\n");

        if(scaleDirection == 1)
        {
          pStage->get_scale(scale_x, scale_y);
          pStage->set_scale(scale_x * FACTOR, scale_y * FACTOR);
          return HANDLED;
        }
        else
        {
          pStage->get_scale(scale_x, scale_y);
          pStage->set_scale(scale_x / FACTOR, scale_y / FACTOR);
          return HANDLED;
        }
      }
      else
      {
        return UNHANDLED;
      }
    }
    else
    {
      printf("scrolling\n");
      return UNHANDLED;
    }
  }
  else
  {
    return UNHANDLED;
  }
}

#ifdef CLUTTER_GTKMM_BUG
VHDEWindow::VHDEWindow(std::unique_ptr<IStageUpdater> pStageUpdater, Clutter::Gtk::Embed &m_clutterEmbed):
#else
VHDEWindow::VHDEWindow(std::unique_ptr<IStageUpdater> pStageUpdater):
#endif
  Gtk::ApplicationWindow(),
  m_pStageUpdater(std::move(pStageUpdater))
{
  /*
   * Construct the widgets in this window
   */

  Glib::RefPtr<Gtk::Builder> pBuilder = Gtk::Builder::create_from_file("glade/window.glade");

  Gtk::Box *pBox;
  pBuilder->get_widget("toplevelbox", pBox);
  add(*pBox);

  Gtk::Box *pClutterEmbedBox;
  pBuilder->get_widget("clutterEmbedBox", pClutterEmbedBox);
  pClutterEmbedBox->add(m_clutterEmbed);

  Gtk::TreeView *pTreeView;
  pBuilder->get_widget("treeview", pTreeView);
  m_pTreeStore = Gtk::TreeStore::create(m_treeStoreColumns);
  pTreeView->set_model(m_pTreeStore);
  pTreeView->append_column("Name", m_treeStoreColumns.name);

  auto parentRow = *m_pTreeStore->append();
  parentRow[m_treeStoreColumns.name] = "parent";
  auto childRow = *m_pTreeStore->append(parentRow.children());
  childRow[m_treeStoreColumns.name] = "child";

  set_default_size(1300, 700);

  m_stage = m_clutterEmbed.get_stage();
  m_stage->set_color(STAGE_COLOR);
  m_stage->set_motion_events_enabled(false);

  m_pStageUpdater->setStage(m_stage);

  /*
   * Allow the user to interact with the diagram
   */
  m_capture_connection = m_stage->signal_captured_event().connect(sigc::bind(&on_my_captured_event, m_stage));
}

VHDEWindow::~VHDEWindow()
{
  std::cout << "Disconnecting signals" << std::endl;
  m_capture_connection.disconnect();

  /* Force removal of the updater and thereby registration to events, when the window is closed */
  m_pStageUpdater = nullptr;
}


