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
#include <tesseract_qt/common/models/quaternion_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
QuaternionStandardItem::QuaternionStandardItem(const Eigen::Quaterniond& q)
  : QStandardItem(icons::getOrientationIcon(), "orientation")
{
  ctor(q);
}

QuaternionStandardItem::QuaternionStandardItem(const QString& text, const Eigen::Quaterniond& q)
  : QStandardItem(icons::getOrientationIcon(), text)
{
  ctor(q);
}

QuaternionStandardItem::QuaternionStandardItem(const QIcon& icon, const QString& text, const Eigen::Quaterniond& q)
  : QStandardItem(icon, text)
{
  ctor(q);
}

int QuaternionStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_TRANSFORM_QUATERNION); }

Eigen::Quaterniond QuaternionStandardItem::getQuaternion() const
{
  return Eigen::Quaterniond(child(3, 1)->data(Qt::DisplayRole).toDouble(),
                            child(0, 1)->data(Qt::DisplayRole).toDouble(),
                            child(1, 1)->data(Qt::DisplayRole).toDouble(),
                            child(2, 1)->data(Qt::DisplayRole).toDouble());
}

void QuaternionStandardItem::setQuaternion(const Eigen::Quaterniond& q)
{
  child(0, 1)->setData(q.x(), Qt::DisplayRole);
  child(1, 1)->setData(q.y(), Qt::DisplayRole);
  child(2, 1)->setData(q.z(), Qt::DisplayRole);
  child(3, 1)->setData(q.w(), Qt::DisplayRole);
}

void QuaternionStandardItem::ctor(const Eigen::Quaterniond& q)
{
  setColumnCount(2);

  appendRow(createStandardItemFloat("x", q.x()));
  appendRow(createStandardItemFloat("y", q.y()));
  appendRow(createStandardItemFloat("z", q.z()));
  appendRow(createStandardItemFloat("w", q.w()));
}
}  // namespace tesseract_gui
