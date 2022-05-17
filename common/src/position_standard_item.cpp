/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include <tesseract_qt/common/position_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
PositionStandardItem::PositionStandardItem(const Eigen::Vector3d& position)
  : QStandardItem(icons::getPositionIcon(), "position")
{
  ctor(position);
}

PositionStandardItem::PositionStandardItem(const QString& text, const Eigen::Vector3d& position)
  : QStandardItem(icons::getPositionIcon(), text)
{
  ctor(position);
}

PositionStandardItem::PositionStandardItem(const QIcon& icon, const QString& text, const Eigen::Vector3d& position)
  : QStandardItem(icon, text)
{
  ctor(position);
}

int PositionStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_TRANSFORM_POSITION); }

void PositionStandardItem::setPosition(const Eigen::Vector3d& position)
{
  child(0, 1)->setData(position.x(), Qt::DisplayRole);
  child(1, 1)->setData(position.y(), Qt::DisplayRole);
  child(2, 1)->setData(position.z(), Qt::DisplayRole);
}

void PositionStandardItem::ctor(const Eigen::Vector3d& position)
{
  setColumnCount(2);

  appendRow(createStandardItemFloat("x", position.x()));
  appendRow(createStandardItemFloat("y", position.y()));
  appendRow(createStandardItemFloat("z", position.z()));
}
}  // namespace tesseract_gui
