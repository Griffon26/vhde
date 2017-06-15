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

#ifndef _ENTITY_STAGE_UPDATER_H
#define _ENTITY_STAGE_UPDATER_H

#include <memory>

#include "gui_component.h"
#include "i_stage_updater.h"

class LayoutComponent;

class EntityStageUpdater: public IStageUpdater
{
public:
  EntityStageUpdater();
  virtual ~EntityStageUpdater();

  void setEntity(LayoutComponent *pEntity);
  void setStage(Glib::RefPtr<Clutter::Stage> pStage);

private:
  LayoutComponent *m_pEntity;
  Glib::RefPtr<Clutter::Stage> m_pStage;

  std::unique_ptr<GuiComponent> m_pGuiComponent;
};

#endif

