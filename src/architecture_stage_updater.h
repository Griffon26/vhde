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

#ifndef _ARCHITECTURE_STAGE_UPDATER_H
#define _ARCHITECTURE_STAGE_UPDATER_H

#include <memory>

#include "gui_instance.h"
#include "gui_signal.h"
#include "i_stage_updater.h"

class LayoutArchitecture;

class ArchitectureStageUpdater: public IStageUpdater
{
public:
  ArchitectureStageUpdater();
  virtual ~ArchitectureStageUpdater();

  void setArchitecture(LayoutArchitecture *pArch);
  void setStage(Glib::RefPtr<Clutter::Stage> pStage);
  bool onKeyPressEvent(GdkEventKey *pEvent);

private:
  LayoutArchitecture *m_pArch;
  Glib::RefPtr<Clutter::Stage> m_pStage;

  std::unique_ptr<GuiInstance> m_pGuiInstance;
  std::unique_ptr<GuiSignal> m_pGuiSignal;
};

#endif

