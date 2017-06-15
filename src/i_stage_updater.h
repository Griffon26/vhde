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

#ifndef _I_STAGE_UPDATER_H
#define _I_STAGE_UPDATER_H

#include <cluttermm.h>
#include <glibmm.h>

/*
 * This interface is to be implemented by any object that does all the drawing
 * to a Window's clutter stage. The window will pass its stage to this object
 * and destroy the object itself when it is no longer needed.
 */
class IStageUpdater
{
public:
  virtual ~IStageUpdater() {};
  virtual void setStage(Glib::RefPtr<Clutter::Stage> pStage) = 0;
};

#endif

