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

#ifndef _LAYOUT_INSTANCE_H
#define _LAYOUT_INSTANCE_H

#include "layout_block.h"
#include "layout_component.h"

class LayoutInstance: public LayoutBlock
{
private:
  LayoutComponent  *m_pComponent;
  INamedItem       *m_pVHDLInstance;

  sigc::connection m_onSignalDisassociatedConnection;
  sigc::connection m_onPortAddedConnection;
  sigc::connection m_onPortRemovedConnection;

public:
  /* Signals */
  sigc::signal<void, const LayoutPosition &>  moved;

  LayoutInstance(LayoutComponent *pComponent);
  ~LayoutInstance();

  void setPosition(const LayoutPosition &pos);

  void associateInstance(INamedItem *pVHDLInstance);
  INamedItem *getAssociatedVHDLInstance();

  void write(FILE *pFile);

private:
#if 0
  bool findPort(const VHDLPort *pPort, Edge *pEdge, int *pPosition);

  /* Signal handlers */
  void onSignalDisassociated(VHDLSignal *pSignal, VHDLPort *pPort);
#endif
  void onPortAdded(int actionId, Edge edge, int position, LayoutPort *pLayoutPort);
  void onPortRemoved(int actionId, Edge edge, int position);
};

#endif /* _LAYOUT_INSTANCE_H */
