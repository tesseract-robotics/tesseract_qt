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
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/position_standard_item.h>
#include <tesseract_qt/common/quaternion_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/tool_path_pose.h>
#include <boost/uuid/uuid_generators.hpp>

namespace tesseract_gui
{
TransformStandardItem::TransformStandardItem(const Eigen::Isometry3d& transform)
  : QStandardItem(icons::getOriginIcon(), "Transform"), uuid_(boost::uuids::random_generator()())
{
  ctor(transform);
}

TransformStandardItem::TransformStandardItem(const QString& text, const Eigen::Isometry3d& transform)
  : QStandardItem(icons::getOriginIcon(), text), uuid_(boost::uuids::random_generator()())
{
  ctor(transform);
}

TransformStandardItem::TransformStandardItem(const QIcon& icon, const QString& text, const Eigen::Isometry3d& transform)
  : QStandardItem(icon, text), uuid_(boost::uuids::random_generator()())
{
  ctor(transform);
}

TransformStandardItem::TransformStandardItem(const ToolPathPose& transform)
  : QStandardItem(icons::getOriginIcon(), "Transform")
  , uuid_(transform.getUUID())
  , parent_uuid_(transform.getParentUUID())
  , description_(transform.getDescription())
{
  setCheckable(true);
  setCheckState(Qt::CheckState::Checked);
  ctor(transform.getTransform());
}

TransformStandardItem::TransformStandardItem(const QString& text, const ToolPathPose& transform)
  : QStandardItem(icons::getOriginIcon(), text)
  , uuid_(transform.getUUID())
  , parent_uuid_(transform.getParentUUID())
  , description_(transform.getDescription())
{
  setCheckable(true);
  setCheckState(Qt::CheckState::Checked);
  ctor(transform.getTransform());
}

TransformStandardItem::TransformStandardItem(const QIcon& icon, const QString& text, const ToolPathPose& transform)
  : QStandardItem(icon, text)
  , uuid_(transform.getUUID())
  , parent_uuid_(transform.getParentUUID())
  , description_(transform.getDescription())
{
  setCheckable(true);
  setCheckState(Qt::CheckState::Checked);
  ctor(transform.getTransform());
}

int TransformStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_TRANSFORM); }

Eigen::Isometry3d TransformStandardItem::getTransfrom() const
{
  Eigen::Isometry3d tf(orientation_->getQuaternion());
  tf.translation() = position_->getPosition();
  return tf;
}

void TransformStandardItem::setTransform(const Eigen::Isometry3d& transform)
{
  position_->setPosition(transform.translation());

  Eigen::Quaterniond q(transform.rotation());
  orientation_->setQuaternion(q);
}

boost::uuids::uuid TransformStandardItem::getUUID() const { return uuid_; }
const boost::uuids::uuid& TransformStandardItem::getParentUUID() const { return parent_uuid_; }
void TransformStandardItem::setDescription(const std::string& desc) { description_ = desc; }
const std::string& TransformStandardItem::getDescription() const { return description_; }

void TransformStandardItem::ctor(const Eigen::Isometry3d& transform)
{
  position_ = new PositionStandardItem(transform.translation());
  appendRow(position_);

  Eigen::Quaterniond q(transform.rotation());
  orientation_ = new QuaternionStandardItem(q);
  appendRow(orientation_);
}
}  // namespace tesseract_gui
