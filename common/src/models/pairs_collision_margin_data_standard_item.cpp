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
#include <tesseract_qt/common/models/pairs_collision_margin_data_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract::gui
{
PairsCollisionMarginDataStandardItem::PairsCollisionMarginDataStandardItem(
    tesseract::common::PairsCollisionMarginData pairs_margin_data)
  : QStandardItem(icons::getCubeIcon(), "Pairs Collision Margin Data"), pairs_margin_data(std::move(pairs_margin_data))
{
  ctor();
}

PairsCollisionMarginDataStandardItem::PairsCollisionMarginDataStandardItem(
    const QString& text,
    tesseract::common::PairsCollisionMarginData pairs_margin_data)
  : QStandardItem(icons::getCubeIcon(), text), pairs_margin_data(std::move(pairs_margin_data))
{
  ctor();
}

PairsCollisionMarginDataStandardItem::PairsCollisionMarginDataStandardItem(
    const QIcon& icon,
    const QString& text,
    tesseract::common::PairsCollisionMarginData pairs_margin_data)
  : QStandardItem(icon, text), pairs_margin_data(std::move(pairs_margin_data))
{
  ctor();
}

int PairsCollisionMarginDataStandardItem::type() const
{
  return static_cast<int>(StandardItemType::COMMON_PAIRS_COLLISION_MARGIN_DATA);
}

void PairsCollisionMarginDataStandardItem::ctor()
{
  for (const auto& [key, entry] : pairs_margin_data)
    addPairCollisionMargin(entry.name1, entry.name2, entry.margin);

  sortChildren(0);
}

void PairsCollisionMarginDataStandardItem::addPairCollisionMargin(const std::string& link1,
                                                                  const std::string& link2,
                                                                  double collision_margin)
{
  QStandardItem* item;
  auto it = items_.find(link1);
  if (it == items_.end())
  {
    item = new QStandardItem(link1.c_str());
    appendRow({ item, new QStandardItem() });
    items_[link1] = item;
  }
  else
  {
    item = it->second;
  }

  item->appendRow(createStandardItemFloat(link2, collision_margin));
}
}  // namespace tesseract::gui
