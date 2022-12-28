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
#include <tesseract_qt/common/events/environment_events.h>
#include <tesseract_environment/command.h>

namespace tesseract_gui::events
{
EnvironmentCommandsClear::EnvironmentCommandsClear(ComponentInfo component_info)
  : ComponentEvent(std::move(component_info), kType)
{
}

EnvironmentCommandsClear::~EnvironmentCommandsClear() = default;

//////////////////////////////////////////

EnvironmentCommandsSet::EnvironmentCommandsSet(ComponentInfo component_info,
                                               const tesseract_environment::Commands& commands)
  : ComponentEvent(std::move(component_info), kType), commands_(commands)
{
}

EnvironmentCommandsSet::~EnvironmentCommandsSet() = default;

const tesseract_environment::Commands& EnvironmentCommandsSet::getCommands() const { return commands_; }

//////////////////////////////////////////

EnvironmentCommandsAppend::EnvironmentCommandsAppend(ComponentInfo component_info,
                                                     const tesseract_environment::Commands& commands)
  : ComponentEvent(std::move(component_info), kType), commands_(commands)
{
}

EnvironmentCommandsAppend::~EnvironmentCommandsAppend() = default;

const tesseract_environment::Commands& EnvironmentCommandsAppend::getCommands() const { return commands_; }

//////////////////////////////////////////

EnvironmentApplyCommand::EnvironmentApplyCommand(ComponentInfo component_info,
                                                 const tesseract_environment::Commands& commands,
                                                 bool reset)
  : ComponentEvent(std::move(component_info), kType), commands_(commands), reset_(reset)
{
}

EnvironmentApplyCommand::~EnvironmentApplyCommand() = default;

bool EnvironmentApplyCommand::isReset() const { return reset_; }
const tesseract_environment::Commands& EnvironmentApplyCommand::getCommands() const { return commands_; }

//////////////////////////////////////////

EnvironmentSetState::EnvironmentSetState(ComponentInfo component_info,
                                         const std::unordered_map<std::string, double>& joint_values)
  : ComponentEvent(std::move(component_info), kType), joint_values_(joint_values)
{
}

EnvironmentSetState::~EnvironmentSetState() = default;

const std::unordered_map<std::string, double>& EnvironmentSetState::getJointValues() const { return joint_values_; }

//////////////////////////////////////////

}  // namespace tesseract_gui::events
