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
#include <tesseract_qt/environment/models/commands/change_joint_position_limits_command_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_environment/commands/change_joint_position_limits_command.h>

namespace tesseract_gui
{
ChangeJointPositionLimitsCommandStandardItem::ChangeJointPositionLimitsCommandStandardItem(
    std::shared_ptr<const tesseract_environment::ChangeJointPositionLimitsCommand> command)
  : QStandardItem(icons::getCommandEntryIcon(), "Change Joint Position Limits"), command(std::move(command))
{
  ctor();
}

ChangeJointPositionLimitsCommandStandardItem::ChangeJointPositionLimitsCommandStandardItem(
    const QString& text,
    std::shared_ptr<const tesseract_environment::ChangeJointPositionLimitsCommand> command)
  : QStandardItem(icons::getCommandEntryIcon(), text), command(std::move(command))
{
  ctor();
}

ChangeJointPositionLimitsCommandStandardItem::ChangeJointPositionLimitsCommandStandardItem(
    const QIcon& icon,
    const QString& text,
    std::shared_ptr<const tesseract_environment::ChangeJointPositionLimitsCommand> command)
  : QStandardItem(icon, text), command(std::move(command))
{
  ctor();
}

int ChangeJointPositionLimitsCommandStandardItem::type() const
{
  return static_cast<int>(StandardItemType::ENV_COMMAND_CHANGE_JOINT_POSITION_LIMITS);
}

void ChangeJointPositionLimitsCommandStandardItem::ctor()
{
  for (const auto& joint : command->getLimits())
  {
    auto* item = new QStandardItem(icons::getJointIcon(), joint.first.c_str());

    item->appendRow(createStandardItemFloat("lower", joint.second.first));
    item->appendRow(createStandardItemFloat("upper", joint.second.second));

    appendRow({ item, new QStandardItem() });
  }
}
}  // namespace tesseract_gui
