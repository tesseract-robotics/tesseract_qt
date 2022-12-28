/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 * @par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * @par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * @par
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <tesseract_qt/environment/environment_wrappers.h>
#include <tesseract_qt/common/component_info.h>

#include <tesseract_environment/environment.h>
#include <tesseract_environment/environment_monitor.h>

#include <QApplication>

namespace tesseract_gui
{
DefaultEnvironmentWrapper::DefaultEnvironmentWrapper(ComponentInfo component_info,
                                                     std::shared_ptr<tesseract_environment::Environment> env)
  : EnvironmentWrapper(std::move(component_info)), env_(std::move(env))
{
}

DefaultEnvironmentWrapper::~DefaultEnvironmentWrapper() = default;

std::shared_ptr<const tesseract_environment::Environment> DefaultEnvironmentWrapper::getEnvironment() const
{
  return env_;
}

void DefaultEnvironmentWrapper::tesseractEventFilter(const tesseract_environment::Event& event)
{
  switch (event.type)
  {
    case tesseract_environment::Events::COMMAND_APPLIED:
    {
      //      QApplication::sendEvent(qApp, new events::ToolPathRemoveAll(component_info));
      //      onUpdateModels();
      //      emit environmentChanged(*data_->environment);
      //      break;
    }
    case tesseract_environment::Events::SCENE_STATE_CHANGED:
    {
      //      onUpdateCurrentStateModel();
      //      emit environmentCurrentStateChanged(*data_->environment);
      //      break;
    }
  }
}

MonitorEnvironmentWrapper::MonitorEnvironmentWrapper(
    ComponentInfo component_info,
    std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor)
  : EnvironmentWrapper(std::move(component_info)), env_monitor_(std::move(env_monitor))
{
}
MonitorEnvironmentWrapper::~MonitorEnvironmentWrapper() = default;

std::shared_ptr<const tesseract_environment::Environment> MonitorEnvironmentWrapper::getEnvironment() const
{
  return env_monitor_->getEnvironment();
}

void MonitorEnvironmentWrapper::tesseractEventFilter(const tesseract_environment::Event& event) {}

}  // namespace tesseract_gui
