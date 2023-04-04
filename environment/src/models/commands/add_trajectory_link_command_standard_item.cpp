/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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
#include <tesseract_qt/environment/models/commands/add_trajectory_link_command_standard_item.h>
#include <tesseract_qt/common/joint_trajectory_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
AddTrajectoryLinkCommandStandardItem::AddTrajectoryLinkCommandStandardItem(
    tesseract_environment::AddTrajectoryLinkCommand::ConstPtr command)
  : QStandardItem(icons::getCommandEntryIcon(), "Add Trajectory Link"), command(std::move(command))
{
  ctor();
}

AddTrajectoryLinkCommandStandardItem::AddTrajectoryLinkCommandStandardItem(
    const QString& text,
    tesseract_environment::AddTrajectoryLinkCommand::ConstPtr command)
  : QStandardItem(icons::getCommandEntryIcon(), text), command(std::move(command))
{
  ctor();
}

AddTrajectoryLinkCommandStandardItem::AddTrajectoryLinkCommandStandardItem(
    const QIcon& icon,
    const QString& text,
    tesseract_environment::AddTrajectoryLinkCommand::ConstPtr command)
  : QStandardItem(icon, text), command(std::move(command))
{
  ctor();
}

int AddTrajectoryLinkCommandStandardItem::type() const
{
  return static_cast<int>(StandardItemType::ENV_COMMAND_ADD_TRAJECTORY_LINK);
}

void AddTrajectoryLinkCommandStandardItem::ctor()
{
  appendRow(createStandardItemString(icons::getTextIcon(), "link_name", command->getLinkName()));
  appendRow(createStandardItemString(icons::getTextIcon(), "parent_link_name", command->getParentLinkName()));
  appendRow(createStandardItemString("replace allowed", (command->replaceAllowed()) ? "True" : "False"));
  appendRow(new JointTrajectoryStandardItem("trajectory", command->getTrajectory()));
}
}  // namespace tesseract_gui
