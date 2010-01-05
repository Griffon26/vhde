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
#include "vhdl_instance.h"

class LayoutInstance: public LayoutBlock
{
private:
  VHDLInstance   *m_pInstance;

  sigc::connection m_onSignalDisassociatedConnection;
  sigc::connection m_onPortRemovedConnection;

public:
  /* Signals */
  sigc::signal<void, LayoutInstance *, const LayoutPosition &>  moved;

  LayoutInstance();
  ~LayoutInstance();

  void setPosition(const LayoutPosition &pos);

  void associateInstance(VHDLInstance *pInstance);
  VHDLInstance *getAssociatedInstance();

  void write(FILE *pFile);

private:
  bool findPort(const VHDLPort *pPort, Edge *pEdge, int *pPosition);

  /* Signal handlers */
  void onSignalDisassociated(VHDLSignal *pSignal, VHDLPort *pPort);
  void onPortRemoved(VHDLPort *pPort);
};

#endif /* _LAYOUT_INSTANCE_H */
