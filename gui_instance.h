/* vim: ts=2:sw=2:expandtab
 *
 * Copyright 2009 Maurice van der Pot <griffon26@kfk4ever.com>
 *
 * This file is part of Foobar.
 *
 * Foobar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "gui_block.h"
#include "layout_instance.h"

class VHDLPort;

/**
 * A class that manages the GUI of a VHDL instance.
 *
 * GuiInstance extends GuiBlock with operations that are specific to component
 * instances, such as changing the position on the canvas.
 */
class GuiInstance: public GuiBlock
{
private:
  typedef union {
    struct {
      int         actionId;

      bool        layoutEventReceived;
      bool        vhdlEventReceived;

      Edge        edge;
      int         position;
      LayoutPort *pLayoutPort;
      VHDLPort   *pVHDLPort;
    };
  } EventData;

  bool                    m_dragIsMove;

  EventData               m_eventData;

  /* Model signals */
  sigc::connection        m_onLayoutPortAddedConnection;
  sigc::connection        m_onLayoutPortRemovedConnection;
  sigc::connection        m_onVHDLPortAddedConnection;
  sigc::connection        m_onVHDLPortRemovedConnection;
public:
  GuiInstance(Glib::RefPtr<Clutter::Stage> pStage, LayoutInstance *pLayoutInstance);
  virtual ~GuiInstance();

private:
  virtual bool onBodyButtonPress(Clutter::ButtonEvent *pEvent);
  virtual bool onBodyDragged(Clutter::Event *pEvent);

  void handlePortAdded();
  void handlePortRemoved();

  void onLayoutPortAdded(int actionId, Edge edge, int position, LayoutPort *pLayoutPort);
  void onLayoutPortRemoved(int actionId, Edge edge, int position);
  void onVHDLPortAdded(int actionId, VHDLPort *pVHDLPort);
  void onVHDLPortRemoved(int actionId, VHDLPort *pVHDLPort);
};
