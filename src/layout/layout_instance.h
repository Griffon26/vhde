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

#ifndef _LAYOUT_INSTANCE_H
#define _LAYOUT_INSTANCE_H

#include "layout_block.h"

class INamedItem;
class LayoutComponent;

class LayoutInstance: public LayoutBlock
{
private:
  LayoutComponent  *m_pComponent;
  INamedItem       *m_pVHDLInstance;

  sigc::connection m_onSignalDisassociatedConnection;
  sigc::connection m_onPortAddedConnection;
  std::map<LayoutPort *, sigc::connection> m_onPortRemovedConnections;

public:
  /* Signals */
  sigc::signal<void, const LayoutPosition &>  moved;
  sigc::signal<void, LayoutInstance *> deleteRequested;

  LayoutInstance();
  ~LayoutInstance();

  LayoutInstance(const LayoutInstance&) = delete;
  LayoutInstance& operator=(const LayoutInstance&) = delete;

  void setPosition(const LayoutPosition &pos);

  void associateLayoutComponent(LayoutComponent *pLayoutComponent);
  void associateVHDLInstance(INamedItem *pVHDLInstance);
  INamedItem *getAssociatedVHDLInstance();

  void write(std::ostream &stream, int indent);

  void discard();

private:
#if 0
  bool findPort(const VHDLPort *pPort, Edge *pEdge, int *pPosition);

  /* Signal handlers */
  void onSignalDisassociated(VHDLSignal *pSignal, VHDLPort *pPort);
#endif
  void onPortAdded(Edge edge, int position, LayoutPort *pLayoutPort);
  void onPortRemoved(Edge edge, int position, LayoutPort *pComponentLayoutPort, LayoutPort *pOurLayoutPort);
};

#endif /* _LAYOUT_INSTANCE_H */
