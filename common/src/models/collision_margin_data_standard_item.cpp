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
#include <tesseract_qt/common/models/collision_margin_data_standard_item.h>
#include <tesseract_qt/common/models/pairs_collision_margin_data_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
CollisionMarginDataStandardItem::CollisionMarginDataStandardItem(
    tesseract_common::CollisionMarginData collision_margin_data)
  : QStandardItem(icons::getCubeIcon(), "Collision Margin Data")
  , collision_margin_data(std::move(collision_margin_data))
{
  ctor();
}

CollisionMarginDataStandardItem::CollisionMarginDataStandardItem(
    const QString& text,
    tesseract_common::CollisionMarginData collision_margin_data)
  : QStandardItem(icons::getCubeIcon(), text), collision_margin_data(std::move(collision_margin_data))
{
  ctor();
}

CollisionMarginDataStandardItem::CollisionMarginDataStandardItem(
    const QIcon& icon,
    const QString& text,
    tesseract_common::CollisionMarginData collision_margin_data)
  : QStandardItem(icon, text), collision_margin_data(std::move(collision_margin_data))
{
  ctor();
}

int CollisionMarginDataStandardItem::type() const
{
  return static_cast<int>(StandardItemType::COMMON_COLLISION_MARGIN_DATA);
}

void CollisionMarginDataStandardItem::ctor()
{
  appendRow(createStandardItemFloat("Default Margin", collision_margin_data.getDefaultCollisionMargin()));
  appendRow(createStandardItemFloat("Max Margin", collision_margin_data.getMaxCollisionMargin()));
  appendRow(new PairsCollisionMarginDataStandardItem(
      collision_margin_data.getCollisionMarginPairData().getCollisionMargins()));
}
}  // namespace tesseract_gui
