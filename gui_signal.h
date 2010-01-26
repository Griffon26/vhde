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

#ifndef _GUI_SIGNAL_H
#define _GUI_SIGNAL_H

#include "clutter_wire.h"
#include "layout_signal.h"

/**
 * A class that manages the GUI of a signal wire in a diagram
 */
class GuiSignal
{
private:
  LayoutSignal    *m_pLayoutSignal;

  Glib::RefPtr<Clutter::Stage>      m_pStage;
  Glib::RefPtr<Clutter::Text>       m_pText;
  Glib::RefPtr<ClutterWire>         m_pWire;

public:
  GuiSignal(Glib::RefPtr<Clutter::Stage> pStage, LayoutSignal *pLayoutSignal);
  virtual ~GuiSignal();

private:
  void onEndPointMoved(LayoutSignal::EndPointId endPointId, const LayoutPosition &newPos);

  void updateCorners(const std::list<LayoutPosition> *pCornerList);
};

#endif /* _GUI_SIGNAL_H */
