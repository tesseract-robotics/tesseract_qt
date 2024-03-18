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
#include <tesseract_qt/environment/models/environment_commands_standard_item.h>
#include <tesseract_qt/environment/models/commands/add_contact_managers_plugin_info_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/add_kinematics_information_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/add_link_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/add_scene_graph_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/add_trajectory_link_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/change_collision_margins_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/change_joint_acceleration_limits_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/change_joint_origin_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/change_joint_position_limits_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/change_joint_velocity_limits_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/change_link_collision_enabled_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/change_link_origin_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/change_link_visibility_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/modify_allowed_collisions_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/move_joint_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/move_link_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/remove_allowed_collision_link_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/remove_joint_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/remove_link_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/replace_joint_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/set_active_continuous_contact_manager_command_standard_item.h>
#include <tesseract_qt/environment/models/commands/set_active_discrete_contact_manager_command_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_environment/commands.h>

namespace tesseract_gui
{
EnvironmentCommandsStandardItem::EnvironmentCommandsStandardItem() : QStandardItem(icons::getCubeIcon(), "Commands")
{
  ctor();
}

EnvironmentCommandsStandardItem::EnvironmentCommandsStandardItem(
    std::vector<std::shared_ptr<const tesseract_environment::Command>> commands)
  : QStandardItem(icons::getCubeIcon(), "Commands"), commands_(std::move(commands))
{
  ctor();
}

EnvironmentCommandsStandardItem::EnvironmentCommandsStandardItem(
    const QString& text,
    std::vector<std::shared_ptr<const tesseract_environment::Command>> commands)
  : QStandardItem(icons::getCubeIcon(), text), commands_(std::move(commands))
{
  ctor();
}

EnvironmentCommandsStandardItem::EnvironmentCommandsStandardItem(
    const QIcon& icon,
    const QString& text,
    std::vector<std::shared_ptr<const tesseract_environment::Command>> commands)
  : QStandardItem(icon, text), commands_(std::move(commands))
{
  ctor();
}

int EnvironmentCommandsStandardItem::type() const { return static_cast<int>(StandardItemType::ENV_COMMAND_COMMANDS); }

void EnvironmentCommandsStandardItem::appendCommand(
    const std::shared_ptr<const tesseract_environment::Command>& command)
{
  addCommand(QString("[%1]").arg(commands_.size()), command);
  commands_.push_back(command);
}

const std::vector<std::shared_ptr<const tesseract_environment::Command>>&
EnvironmentCommandsStandardItem::getCommands() const
{
  return commands_;
}

void EnvironmentCommandsStandardItem::ctor()
{
  for (std::size_t i = 0; i < commands_.size(); ++i)
    addCommand(QString("[%1]").arg(i), commands_[i]);
}

void EnvironmentCommandsStandardItem::addCommand(const QString& text,
                                                 const std::shared_ptr<const tesseract_environment::Command>& command)
{
  switch (command->getType())
  {
    case tesseract_environment::CommandType::ADD_LINK:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::AddLinkCommand>(command);
      appendRow({ new AddLinkCommandStandardItem(text, cmd), new QStandardItem("Add Link") });
      break;
    }
    case tesseract_environment::CommandType::MOVE_LINK:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::MoveLinkCommand>(command);
      appendRow({ new MoveLinkCommandStandardItem(text, cmd), new QStandardItem("Move Link") });
      break;
    }
    case tesseract_environment::CommandType::MOVE_JOINT:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::MoveJointCommand>(command);
      appendRow({ new MoveJointCommandStandardItem(text, cmd), new QStandardItem("Move Joint") });
      break;
    }
    case tesseract_environment::CommandType::REMOVE_LINK:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::RemoveLinkCommand>(command);
      appendRow({ new RemoveLinkCommandStandardItem(text, cmd), new QStandardItem("Remove Link") });
      break;
    }
    case tesseract_environment::CommandType::REMOVE_JOINT:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::RemoveJointCommand>(command);
      appendRow({ new RemoveJointCommandStandardItem(text, cmd), new QStandardItem("Remove Joint") });
      break;
    }
    case tesseract_environment::CommandType::REPLACE_JOINT:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::ReplaceJointCommand>(command);
      appendRow({ new ReplaceJointCommandStandardItem(text, cmd), new QStandardItem("Replace Joint") });
      break;
    }
    case tesseract_environment::CommandType::CHANGE_LINK_ORIGIN:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::ChangeLinkOriginCommand>(command);
      appendRow({ new ChangeLinkOriginCommandStandardItem(text, cmd), new QStandardItem("Change Link Origin") });
      break;
    }
    case tesseract_environment::CommandType::CHANGE_JOINT_ORIGIN:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::ChangeJointOriginCommand>(command);
      appendRow({ new ChangeJointOriginCommandStandardItem(text, cmd), new QStandardItem("Change Joint Origin") });
      break;
    }
    case tesseract_environment::CommandType::CHANGE_LINK_COLLISION_ENABLED:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::ChangeLinkCollisionEnabledCommand>(command);
      appendRow({ new ChangeLinkCollisionEnabledCommandStandardItem(text, cmd),
                  new QStandardItem("Change Link Collision Enabled") });
      break;
    }
    case tesseract_environment::CommandType::CHANGE_LINK_VISIBILITY:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::ChangeLinkVisibilityCommand>(command);
      appendRow(
          { new ChangeLinkVisibilityCommandStandardItem(text, cmd), new QStandardItem("Change Link Visibility") });
      break;
    }
    case tesseract_environment::CommandType::MODIFY_ALLOWED_COLLISIONS:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::ModifyAllowedCollisionsCommand>(command);
      appendRow({ new ModifyAllowedCollisionsCommandStandardItem(text, cmd),
                  new QStandardItem("Modify Allowed Collisions") });
      break;
    }
    case tesseract_environment::CommandType::REMOVE_ALLOWED_COLLISION_LINK:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::RemoveAllowedCollisionLinkCommand>(command);
      appendRow({ new RemoveAllowedCollisionLinkCommandStandardItem(text, cmd),
                  new QStandardItem("Remove Allowed Collision Link") });
      break;
    }
    case tesseract_environment::CommandType::ADD_SCENE_GRAPH:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::AddSceneGraphCommand>(command);
      appendRow({ new AddSceneGraphCommandStandardItem(text, cmd), new QStandardItem("Add Scene Graph") });
      break;
    }
    case tesseract_environment::CommandType::CHANGE_JOINT_POSITION_LIMITS:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::ChangeJointPositionLimitsCommand>(command);
      appendRow({ new ChangeJointPositionLimitsCommandStandardItem(text, cmd),
                  new QStandardItem("Change Joint Position Limits") });
      break;
    }
    case tesseract_environment::CommandType::CHANGE_JOINT_VELOCITY_LIMITS:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::ChangeJointVelocityLimitsCommand>(command);
      appendRow({ new ChangeJointVelocityLimitsCommandStandardItem(text, cmd),
                  new QStandardItem("Change Joint Velocity Limits") });
      break;
    }
    case tesseract_environment::CommandType::CHANGE_JOINT_ACCELERATION_LIMITS:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::ChangeJointAccelerationLimitsCommand>(command);
      appendRow({ new ChangeJointAccelerationLimitsCommandStandardItem(text, cmd),
                  new QStandardItem("Change Joint Acceleration Limits") });
      break;
    }
    case tesseract_environment::CommandType::ADD_KINEMATICS_INFORMATION:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::AddKinematicsInformationCommand>(command);
      appendRow({ new AddKinematicsInformationCommandStandardItem(text, cmd),
                  new QStandardItem("Add Kinematics Information") });
      break;
    }
    case tesseract_environment::CommandType::CHANGE_COLLISION_MARGINS:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::ChangeCollisionMarginsCommand>(command);
      appendRow(
          { new ChangeCollisionMarginsCommandStandardItem(text, cmd), new QStandardItem("Change Collision Margins") });
      break;
    }
    case tesseract_environment::CommandType::ADD_CONTACT_MANAGERS_PLUGIN_INFO:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::AddContactManagersPluginInfoCommand>(command);
      appendRow({ new AddContactManagersPluginInfoCommandStandardItem(text, cmd),
                  new QStandardItem("Add Contact Manager Plugin Info") });
      break;
    }
    case tesseract_environment::CommandType::SET_ACTIVE_CONTINUOUS_CONTACT_MANAGER:
    {
      auto cmd =
          std::static_pointer_cast<const tesseract_environment::SetActiveContinuousContactManagerCommand>(command);
      appendRow({ new SetActiveContinuousContactManagerCommandStandardItem(text, cmd),
                  new QStandardItem("Set Active Continuous Contact Manager") });
      break;
    }
    case tesseract_environment::CommandType::SET_ACTIVE_DISCRETE_CONTACT_MANAGER:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::SetActiveDiscreteContactManagerCommand>(command);
      appendRow({ new SetActiveDiscreteContactManagerCommandStandardItem(text, cmd),
                  new QStandardItem("Set Active Discrete Contact Manager") });
      break;
    }
    case tesseract_environment::CommandType::ADD_TRAJECTORY_LINK:
    {
      auto cmd = std::static_pointer_cast<const tesseract_environment::AddTrajectoryLinkCommand>(command);
      appendRow({ new AddTrajectoryLinkCommandStandardItem(text, cmd), new QStandardItem("Add Trajectory Link") });
      break;
    }
    default:
      throw std::runtime_error("Unsupported command type!");
  }
}
}  // namespace tesseract_gui
