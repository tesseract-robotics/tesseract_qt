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
#include <tesseract_qt/environment/models/commands/change_collision_margins_command_standard_item.h>
#include <tesseract_qt/common/models/collision_margin_data_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_environment/commands/change_collision_margins_command.h>

namespace tesseract_gui
{
ChangeCollisionMarginsCommandStandardItem::ChangeCollisionMarginsCommandStandardItem(
    std::shared_ptr<const tesseract_environment::ChangeCollisionMarginsCommand> command)
  : QStandardItem(icons::getCommandEntryIcon(), "Change Collision Margins"), command(std::move(command))
{
  ctor();
}

ChangeCollisionMarginsCommandStandardItem::ChangeCollisionMarginsCommandStandardItem(
    const QString& text,
    std::shared_ptr<const tesseract_environment::ChangeCollisionMarginsCommand> command)
  : QStandardItem(icons::getCommandEntryIcon(), text), command(std::move(command))
{
  ctor();
}

ChangeCollisionMarginsCommandStandardItem::ChangeCollisionMarginsCommandStandardItem(
    const QIcon& icon,
    const QString& text,
    std::shared_ptr<const tesseract_environment::ChangeCollisionMarginsCommand> command)
  : QStandardItem(icon, text), command(std::move(command))
{
  ctor();
}

int ChangeCollisionMarginsCommandStandardItem::type() const
{
  return static_cast<int>(StandardItemType::ENV_COMMAND_CHANGE_COLLISION_MARGINS);
}

void ChangeCollisionMarginsCommandStandardItem::ctor()
{
  switch (command->getCollisionMarginOverrideType())
  {
    case tesseract_common::CollisionMarginOverrideType::NONE:
    {
      appendRow(createStandardItemString("Override type", "NONE"));
      break;
    }
    case tesseract_common::CollisionMarginOverrideType::REPLACE:
    {
      appendRow(createStandardItemString("Override type", "REPLACE"));
      break;
    }
    case tesseract_common::CollisionMarginOverrideType::MODIFY:
    {
      appendRow(createStandardItemString("Override type", "MODIFY"));
      break;
    }
    case tesseract_common::CollisionMarginOverrideType::OVERRIDE_DEFAULT_MARGIN:
    {
      appendRow(createStandardItemString("Override type", "OVERRIDE_DEFAULT_MARGIN"));
      break;
    }
    case tesseract_common::CollisionMarginOverrideType::OVERRIDE_PAIR_MARGIN:
    {
      appendRow(createStandardItemString("Override type", "OVERRIDE_PAIR_MARGIN"));
      break;
    }
    case tesseract_common::CollisionMarginOverrideType::MODIFY_PAIR_MARGIN:
    {
      appendRow(createStandardItemString("Override type", "MODIFY_PAIR_MARGIN"));
      break;
    }
  }

  auto* cmd_item = new CollisionMarginDataStandardItem(command->getCollisionMarginData());
  appendRow({ cmd_item, new QStandardItem() });
}
}  // namespace tesseract_gui
