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
#include <tesseract_qt/environment/models/commands/move_joint_command_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_environment/commands/move_joint_command.h>

namespace tesseract::gui
{
MoveJointCommandStandardItem::MoveJointCommandStandardItem(
    std::shared_ptr<const tesseract::environment::MoveJointCommand> command)
  : QStandardItem(icons::getCommandEntryIcon(), "Move Joint"), command(std::move(command))
{
  ctor();
}

MoveJointCommandStandardItem::MoveJointCommandStandardItem(
    const QString& text,
    std::shared_ptr<const tesseract::environment::MoveJointCommand> command)
  : QStandardItem(icons::getCommandEntryIcon(), text), command(std::move(command))
{
  ctor();
}

MoveJointCommandStandardItem::MoveJointCommandStandardItem(
    const QIcon& icon,
    const QString& text,
    std::shared_ptr<const tesseract::environment::MoveJointCommand> command)
  : QStandardItem(icon, text), command(std::move(command))
{
  ctor();
}

int MoveJointCommandStandardItem::type() const { return static_cast<int>(StandardItemType::ENV_COMMAND_MOVE_JOINT); }

void MoveJointCommandStandardItem::ctor()
{
  appendRow(createStandardItemString("joint name", command->getJointName()));
  appendRow(createStandardItemString("parent link", command->getParentLink()));
}
}  // namespace tesseract::gui
