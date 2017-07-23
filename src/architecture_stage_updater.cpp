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

#include <glib.h>
#include <iostream>

#include "architecture_stage_updater.h"
#include "layout_architecture.h"

ArchitectureStageUpdater::ArchitectureStageUpdater():
  m_pArch(nullptr)
{
  std::cout << "ArchitectureStageUpdater()::ArchitectureStageUpdater()\n";
}

ArchitectureStageUpdater::~ArchitectureStageUpdater()
{
  std::cout << "ArchitectureStageUpdater()::~ArchitectureStageUpdater()\n";
}

void ArchitectureStageUpdater::setArchitecture(LayoutArchitecture *pArch)
{
  g_assert(m_pArch == nullptr);
  m_pArch = pArch;
}

void ArchitectureStageUpdater::setStage(Glib::RefPtr<Clutter::Stage> pStage)
{
  g_assert(!m_pStage);
  g_assert(m_pArch != nullptr);
  m_pStage = pStage;

  m_pStage->signal_button_release_event().connect(sigc::mem_fun(*this, &ArchitectureStageUpdater::onStageClicked));

  for(auto &layoutSignal: m_pArch->getSignals())
  {
    m_pGuiSignals.push_back(std::make_unique<GuiSignal>(m_pStage, layoutSignal));
  }
  for(auto &layoutInstance: m_pArch->getInstances())
  {
    auto pGuiInstance = std::make_unique<GuiInstance>(m_pStage, layoutInstance);
    pGuiInstance->clicked.connect(sigc::mem_fun(*this, &ArchitectureStageUpdater::onInstanceClicked));
    m_pGuiInstances.push_back(std::move(pGuiInstance));
  }
}

bool ArchitectureStageUpdater::onKeyPressEvent(GdkEventKey *pEvent)
{
  return false;
}

void ArchitectureStageUpdater::onInstanceClicked(unsigned int modifiers, GuiBlock *pInstance)
{
  switch(modifiers)
  {
    case CLUTTER_SHIFT_MASK:
    case CLUTTER_CONTROL_MASK:
      m_guiSelection.toggle(pInstance);
      break;
    default:
      m_guiSelection.set(pInstance);
      break;
  }
}

bool ArchitectureStageUpdater::onStageClicked(Clutter::ButtonEvent *pButtonEvent)
{
  if( pButtonEvent->button == 1 &&
      (pButtonEvent->modifier_state & ALL_MODIFIERS_MASK) != CLUTTER_CONTROL_MASK)
  {
    m_guiSelection.clear();
    return true;
  }

  return false;
}

void ArchitectureStageUpdater::deleteSelection()
{
  auto selectedObjects = m_guiSelection.get();
  m_guiSelection.clear();
  for(auto &obj: selectedObjects)
  {
    auto pGuiInstance = static_cast<GuiInstance *>(obj);
    pGuiInstance->discard();
  }
  std::cout << "Number of items in selectedObjects: " << selectedObjects.size() << "\n";
  m_pGuiInstances.erase(std::remove_if(m_pGuiInstances.begin(),
                                       m_pGuiInstances.end(),
                                       [&] (std::unique_ptr<GuiInstance> const &p)
                                       {
                                         std::cout << "uniqp = " << p.get() << " and in selectedObjects is " << selectedObjects.count(p.get()) << "\n";
                                         return selectedObjects.count(p.get()) == 1;
                                       }),
                        m_pGuiInstances.end());
}

