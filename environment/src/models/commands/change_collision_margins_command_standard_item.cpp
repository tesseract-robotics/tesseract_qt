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
#include <tesseract_qt/common/models/pairs_collision_margin_data_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_environment/commands/change_collision_margins_command.h>

namespace tesseract::gui
{
ChangeCollisionMarginsCommandStandardItem::ChangeCollisionMarginsCommandStandardItem(
    std::shared_ptr<const tesseract::environment::ChangeCollisionMarginsCommand> command)
  : QStandardItem(icons::getCommandEntryIcon(), "Change Collision Margins"), command(std::move(command))
{
  ctor();
}

ChangeCollisionMarginsCommandStandardItem::ChangeCollisionMarginsCommandStandardItem(
    const QString& text,
    std::shared_ptr<const tesseract::environment::ChangeCollisionMarginsCommand> command)
  : QStandardItem(icons::getCommandEntryIcon(), text), command(std::move(command))
{
  ctor();
}

ChangeCollisionMarginsCommandStandardItem::ChangeCollisionMarginsCommandStandardItem(
    const QIcon& icon,
    const QString& text,
    std::shared_ptr<const tesseract::environment::ChangeCollisionMarginsCommand> command)
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
  if (command->getDefaultCollisionMargin().has_value())
    appendRow(createStandardItemFloat("Default Margin", command->getDefaultCollisionMargin().value()));
  else
    appendRow(createStandardItemString("Default Margin", "nullopt"));

  if (command->getCollisionMarginPairData().getMaxCollisionMargin())
    appendRow(
        createStandardItemFloat("Max Margin", command->getCollisionMarginPairData().getMaxCollisionMargin().value()));
  else
    appendRow(createStandardItemString("Max Margin", "nullopt"));

  switch (command->getCollisionMarginPairOverrideType())
  {
    case tesseract::common::CollisionMarginPairOverrideType::NONE:
    {
      appendRow(createStandardItemString("Override pair type", "NONE"));
      break;
    }
    case tesseract::common::CollisionMarginPairOverrideType::REPLACE:
    {
      appendRow(createStandardItemString("Override pair type", "REPLACE"));
      break;
    }
    case tesseract::common::CollisionMarginPairOverrideType::MODIFY:
    {
      appendRow(createStandardItemString("Override pair type", "MODIFY"));
      break;
    }
  }

  appendRow(new PairsCollisionMarginDataStandardItem(command->getCollisionMarginPairData().getCollisionMargins()));
}
}  // namespace tesseract::gui
