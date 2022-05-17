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
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/position_standard_item.h>
#include <tesseract_qt/common/quaternion_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
TransformStandardItem::TransformStandardItem(const Eigen::Isometry3d& transform)
  : QStandardItem(icons::getOriginIcon(), "Transform")
{
  ctor(transform);
}

TransformStandardItem::TransformStandardItem(const QString& text, const Eigen::Isometry3d& transform)
  : QStandardItem(icons::getOriginIcon(), text)
{
  ctor(transform);
}

TransformStandardItem::TransformStandardItem(const QIcon& icon, const QString& text, const Eigen::Isometry3d& transform)
  : QStandardItem(icon, text)
{
  ctor(transform);
}

int TransformStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_TRANSFORM); }

void TransformStandardItem::setTransform(const Eigen::Isometry3d& transform)
{
  position_->setPosition(transform.translation());

  Eigen::Quaterniond q(transform.rotation());
  orientation_->setQuaternion(q);
}

void TransformStandardItem::ctor(const Eigen::Isometry3d& transform)
{
  position_ = new PositionStandardItem(transform.translation());
  appendRow(position_);

  Eigen::Quaterniond q(transform.rotation());
  orientation_ = new QuaternionStandardItem(q);
  appendRow(orientation_);
}
}  // namespace tesseract_gui
