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

#include "gui_signal.h"

/*
 * Public methods
 */

GuiSignal::GuiSignal(Glib::RefPtr<Clutter::Stage> pStage, LayoutSignal *pLayoutSignal):
  m_pLayoutSignal(pLayoutSignal),
  m_pStage(pStage)
{
  m_pWire = ClutterWire::create(Clutter::Color(0x00, 0x00, 0x00, 0xFF));

  updateCorners(m_pLayoutSignal->getCorners());

  m_pStage->add_actor(m_pWire);

  m_pLayoutSignal->endpoint_moved.connect(sigc::mem_fun(this, &GuiSignal::onEndPointMoved));
}

GuiSignal::~GuiSignal()
{
}

void GuiSignal::onEndPointMoved(LayoutSignal::EndPointId endPointId, const LayoutPosition &newPos)
{
  updateCorners(m_pLayoutSignal->getCorners());
}

/*
 * Private methods
 */

void GuiSignal::updateCorners(const std::list<LayoutPosition> *pCornerList)
{
  float *pCornerCoords, *pCurrentCoord;
  std::list<LayoutPosition>::const_iterator it;

  pCornerCoords = (float *)g_malloc(pCornerList->size() * 2 * sizeof(float));

  pCurrentCoord = pCornerCoords;
  for(it = pCornerList->begin(); it != pCornerList->end(); it++)
  {
    *pCurrentCoord++ = it->x;
    *pCurrentCoord++ = it->y;
  }
  m_pWire->setCorners(pCornerList->size(), pCornerCoords);
}
