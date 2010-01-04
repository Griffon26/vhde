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

#ifndef _LAYOUT_SIGNAL_H
#define _LAYOUT_SIGNAL_H

#include <glibmm.h>

#include "layout_instance.h"
#include "layout_types.h"
#include "vhdl_instance.h"
#include "vhdl_signal.h"

class LayoutSignal
{
public:
  typedef enum
  {
    BEGINNING,
    END
  } EndPointId;
private:
  typedef struct
  {
    bool isPort;
    bool connected;
    union {
      struct {
        LayoutInstance *pLayoutInstance;
        Edge            edge;
        int             position;
        int             offset_x;
        int             offset_y;
      };
      VHDLSignal *pSignal;
    };
  } EndPoint;

  VHDLSignal               *m_pSignal;

  std::list<LayoutPosition> m_corners;

  EndPoint beginning;
  EndPoint end;

public:
  LayoutSignal();
  void associateSignal(VHDLSignal *pSignal);

  void write(FILE *pFile);

  void connect(EndPointId endPointId, LayoutInstance *pInstance, Edge edge, int position);

private:
  void onInstanceMoved(LayoutInstance *pLayoutInstance, LayoutPosition pos, EndPoint *pEndPoint);
  void onPortMoved(Edge newEdge, int newPosition, EndPoint *pEndPoint);
  void onPortDisconnected(EndPoint *pEndPoint);
};

#endif /* _LAYOUT_SIGNAL_H */
