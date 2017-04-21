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

#ifndef _LAYOUT_SIGNAL_H
#define _LAYOUT_SIGNAL_H

#include <glibmm.h>

#include "layout_instance.h"
#include "layout_types.h"

class INamedItem;

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
      struct {
        INamedItem     *pSignal;
        int             x;
        int             y;
      };
    };

    sigc::connection onInstanceMovedConnection;
    sigc::connection onInstanceResizedConnection;
    sigc::connection onPortMovedConnection;
    sigc::connection onPortRemovedConnection;
  } EndPoint;

  INamedItem               *m_pSignal;

  std::list<LayoutPosition> m_corners;

  EndPoint m_endPoints[2];


public:
  sigc::signal<void, EndPointId, const LayoutPosition &>  endpoint_moved;
  sigc::signal<void, EndPointId>                          endpoint_disconnected;

  LayoutSignal();

  /* This function is only used when constructing an arbitrary layout for a given signal.
   * Because I do not want to expose which endpoint is already connected, this function
   * will connect to the first free endpoint.
   */
  void init_connect(LayoutInstance *pInstance, Edge edge, int position);

  void associateSignal(INamedItem *pSignal);

  void write(std::ostream &stream);

  void connect(EndPointId endPointId, LayoutInstance *pInstance, Edge edge, int position);

  const std::list<LayoutPosition> *getCorners();

private:
  void writeEndPoint(std::ostream &stream, const EndPoint &endPoint);
  void recalcEndPoint(EndPointId endPointId);

  void onInstanceMoved(LayoutPosition pos, EndPointId endPointId);
  void onInstanceResized(LayoutSize size, EndPointId endPointId);
  void onPortMoved(Edge newEdge, int newPosition, LayoutPort *pPort, EndPointId endPointId);
  void onPortRemoved(Edge edge, int position, LayoutPort *pPort, EndPointId endPointId);
};

#endif /* _LAYOUT_SIGNAL_H */
