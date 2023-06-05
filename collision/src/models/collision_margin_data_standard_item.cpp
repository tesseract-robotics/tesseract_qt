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
#include <tesseract_qt/collision/models/collision_margin_data_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
CollisionMarginDataStandardItem::CollisionMarginDataStandardItem(
    const tesseract_collision::CollisionMarginData& collision_margin_data)
  : QStandardItem(icons::getCollisionIcon(), "Contact Request")
{
  ctor(collision_margin_data);
}

CollisionMarginDataStandardItem::CollisionMarginDataStandardItem(
    const QString& text,
    const tesseract_collision::CollisionMarginData& collision_margin_data)
  : QStandardItem(icons::getCollisionIcon(), text)
{
  ctor(collision_margin_data);
}

CollisionMarginDataStandardItem::CollisionMarginDataStandardItem(
    const QIcon& icon,
    const QString& text,
    const tesseract_collision::CollisionMarginData& collision_margin_data)
  : QStandardItem(icon, text)
{
  ctor(collision_margin_data);
}

int CollisionMarginDataStandardItem::type() const
{
  return static_cast<int>(StandardItemType::COLLISION_COLLISION_MARGIN_DATA);
}

void CollisionMarginDataStandardItem::ctor(const tesseract_collision::CollisionMarginData& collision_margin_data)
{
  appendRow(createStandardItemFloat("default margin", collision_margin_data.getDefaultCollisionMargin()));
  appendRow(createStandardItemFloat("max margin", collision_margin_data.getMaxCollisionMargin()));
  auto* pair_margins = new QStandardItem("pair margins");
  for (const auto& p : collision_margin_data.getPairCollisionMargins())
    pair_margins->appendRow(createStandardItemFloat(p.first.first + "::" + p.first.second, p.second));

  appendRow(pair_margins);
}
}  // namespace tesseract_gui
