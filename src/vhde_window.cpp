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

enum State
{
  NORMAL,
  WAITING_FOR_EDGE,
  WAITING_FOR_INDEX
};

bool VHDEWindow::on_idle_hide_window()
{
  hide();
  return false;
}

bool VHDEWindow::on_key_pressed(Clutter::KeyEvent *pEvent, GuiComponent *pGuiComponent)
{
  int position;
  static Edge edge;
  static State state = NORMAL;

  if(m_stage)
  {
    std::cout << "Stage points to something valid" << std::endl;
  }
  else
  {
    std::cout << "Stage !!DOES NOT!! point to something valid" << std::endl;
  }

  /* Handle escape */
  if(pEvent->keyval == CLUTTER_KEY_Escape)
  {
    printf("Escaped\n");
    state = NORMAL;
    return HANDLED;
  }

  switch(state)
  {
  case NORMAL:
    if(pEvent->keyval == 'a')
    {
      if(pGuiComponent->findFreeSlot(EDGE_BOTTOM, 2, &edge, &position))
      {
        printf("Adding port at %s %d\n", EDGE_TO_NAME(edge), position);
        pGuiComponent->createPort(edge, position, DIR_OUT, "port3");
      }
      else
      {
        printf("Can't add any more ports\n");
      }
    }
    if(pEvent->keyval == 'd')
    {
      state = WAITING_FOR_EDGE;
      printf("Enter edge of port to delete (L, R, T, B)\n");
    }
    if(pEvent->keyval == 'q')
    {
      printf("Exiting...\n");

      /* Hide the window on idle instead of calling hide() here to avoid a
       * deadlock with a second clutter signal (when the window is destroyed)
       * being emitted from within this handler of the first.
       */
      Glib::signal_idle().connect(sigc::mem_fun(*this, &VHDEWindow::on_idle_hide_window));
    }
    break;
  case WAITING_FOR_EDGE:
    state = WAITING_FOR_INDEX;
    switch(pEvent->keyval)
    {
    case 'l':
      edge = EDGE_LEFT;
      break;
    case 'r':
      edge = EDGE_RIGHT;
      break;
    case 't':
      edge = EDGE_TOP;
      break;
    case 'b':
      edge = EDGE_BOTTOM;
      break;
    default:
      state = WAITING_FOR_EDGE;
      break;
    }
    if(state == WAITING_FOR_INDEX)
    {
      printf("edge = %s\n", EDGE_TO_NAME(edge));
      printf("Enter position of port to be deleted (0-9)\n");
    }
    break;
  case WAITING_FOR_INDEX:
    if(isdigit(pEvent->keyval))
    {
      int position = pEvent->keyval - '0';
      printf("Destroying port at edge %s pos %d\n", EDGE_TO_NAME(edge), position);
      pGuiComponent->destroyPort(edge, position);
      state = NORMAL;
    }
    break;
  default:
    g_assert_not_reached();
  }

  return HANDLED;
}

#ifdef CLUTTER_GTKMM_BUG
VHDEWindow::VHDEWindow(Project *pProject, Clutter::Gtk::Embed &m_clutterEmbed):
#else
VHDEWindow::VHDEWindow(Project *pProject):
#endif
  Gtk::ApplicationWindow()
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

  /*
   * Create GUI objects from some of the layout classes to show them on the screen
   */
  auto pLayoutTopArch = pProject->getLayoutFile("test/top_entity.vhd")->getArchitectures()[0];
  auto pLayoutUsedFile = pProject->getLayoutFile("test/used_entity.vhd");

  m_pGuiSignal = std::make_unique<GuiSignal>(m_stage, pLayoutTopArch->getSignals()[0]);
  m_pGuiInstance = std::make_unique<GuiInstance>(m_stage, pLayoutTopArch->getInstances()[0]);
  m_pGuiComponent = std::make_unique<GuiComponent>(m_stage, pLayoutUsedFile->getComponent());

  /*
   * Allow the user to interact with the diagram
   */
  m_capture_connection = m_stage->signal_captured_event().connect(sigc::bind(&on_my_captured_event, m_stage));
  m_key_press_connection = m_stage->signal_key_press_event().connect(sigc::bind(sigc::mem_fun(*this, &VHDEWindow::on_key_pressed), m_pGuiComponent.get()));
}

VHDEWindow::~VHDEWindow()
{
  std::cout << "Disconnecting signals" << std::endl;
  m_capture_connection.disconnect();
  m_key_press_connection.disconnect();

  m_pGuiSignal.reset(nullptr);
  m_pGuiInstance.reset(nullptr);
  m_pGuiComponent.reset(nullptr);
}


