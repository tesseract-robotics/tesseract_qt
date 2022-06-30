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
#include <tesseract_qt/kinematic_groups/chain_group_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
ChainGroupStandardItem::ChainGroupStandardItem(tesseract_srdf::ChainGroup group)
  : QStandardItem(icons::getRobotArmIcon(), "Chain Group"), group(std::move(group))
{
  ctor();
}

ChainGroupStandardItem::ChainGroupStandardItem(const QString& text, tesseract_srdf::ChainGroup group)
  : QStandardItem(icons::getRobotArmIcon(), text), group(std::move(group))
{
  ctor();
}

ChainGroupStandardItem::ChainGroupStandardItem(const QIcon& icon, const QString& text, tesseract_srdf::ChainGroup group)
  : QStandardItem(icon, text), group(std::move(group))
{
  ctor();
}

int ChainGroupStandardItem::type() const { return static_cast<int>(StandardItemType::SRDF_CHAIN_GROUP); }

void ChainGroupStandardItem::ctor()
{
  appendRow(createStandardItemString("base link", group.front().first));
  appendRow(createStandardItemString("tip link", group.front().second));
}
}  // namespace tesseract_gui
