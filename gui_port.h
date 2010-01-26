/* vim: ts=2:sw=2:expandtab
 *
 * VHDE - The VHDL Diagram Editor
 * Copyright (C) 2009-2010  Maurice van der Pot <griffon26@kfk4ever.com>
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

#ifndef _GUI_PORT_H
#define _GUI_PORT_H

#include "clutter_block.h"
#include "clutter_port.h"
#include "gui_common.h"
#include "layout_block.h"

class GuiPort
{
private:
  Glib::RefPtr<Clutter::Group>  m_pGroup;
  Edge                          m_edge;
  int                           m_position;
  LayoutBlock                  *m_pLayoutBlock;
  LayoutPort                   *m_pLayoutPort;
  Glib::RefPtr<ClutterPort>     m_pActor;

  /* GUI signals */
  sigc::connection          m_onPortButtonPressConnection;

  /* Model signals */
  sigc::connection          m_onPortMovedConnection;
  sigc::connection          m_onBlockResizedConnection;

public:
  /* Signals */
  sigc::signal<bool, Clutter::ButtonEvent *, GuiPort *> button_pressed;

  GuiPort(Glib::RefPtr<Clutter::Group> pGroup, Edge edge, int position, LayoutBlock *pLayoutBlock, LayoutPort *pLayoutPort);
  virtual ~GuiPort();

  Edge  getEdge()     { return m_edge; }
  int   getPosition() { return m_position; }

  void  updatePosition();

private:
  bool onPortButtonPress(Clutter::ButtonEvent* pEvent);
  void onPortMoved(Edge edge, int position);
  void onBlockResized(LayoutSize layoutSize);

  ClutterPort::Orientation edgeToOrientation(Edge edge);
};

#endif /* _GUI_PORT_H */
