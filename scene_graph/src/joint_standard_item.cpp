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
#include <tesseract_qt/scene_graph/joint_standard_item.h>
#include <tesseract_qt/scene_graph/dynamics_standard_item.h>
#include <tesseract_qt/scene_graph/limits_standard_item.h>
#include <tesseract_qt/scene_graph/safety_standard_item.h>
#include <tesseract_qt/scene_graph/calibration_standard_item.h>
#include <tesseract_qt/scene_graph/mimic_standard_item.h>
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
JointStandardItem::JointStandardItem(tesseract_scene_graph::Joint::Ptr joint)
  : QStandardItem(icons::getJointIcon(), "Joint"), joint(std::move(joint))
{
  ctor();
}

JointStandardItem::JointStandardItem(const QString& text, tesseract_scene_graph::Joint::Ptr joint)
  : QStandardItem(icons::getJointIcon(), text), joint(std::move(joint))
{
  ctor();
}

JointStandardItem::JointStandardItem(const QIcon& icon, const QString& text, tesseract_scene_graph::Joint::Ptr joint)
  : QStandardItem(icon, text), joint(std::move(joint))
{
  ctor();
}

int JointStandardItem::type() const { return static_cast<int>(StandardItemType::SG_JOINT); }

void JointStandardItem::ctor()
{
  appendRow(createStandardItemString("name", joint->getName()));

  {
    QStandardItem* item;
    if (joint->type == tesseract_scene_graph::JointType::FIXED)
    {
      item = new QStandardItem(icons::getAnchorIcon(), "type");
      setIcon(icons::getJointFixedIcon());
    }
    else if (joint->type == tesseract_scene_graph::JointType::REVOLUTE)
    {
      item = new QStandardItem(icons::getRevoluteIcon(), "type");
      setIcon(icons::getJointRevoluteIcon());
    }
    else if (joint->type == tesseract_scene_graph::JointType::CONTINUOUS)
    {
      item = new QStandardItem(icons::getContinuousIcon(), "type");
      setIcon(icons::getJointContinuousIcon());
    }
    else if (joint->type == tesseract_scene_graph::JointType::PRISMATIC)
    {
      item = new QStandardItem(icons::getPrismaticIcon(), "type");
      setIcon(icons::getJointPrismaticIcon());
    }
    else
    {
      item = new QStandardItem(icons::getUnknownIcon(), "type");
      setIcon(icons::getJointUnknownIcon());
    }

    std::stringstream ss;
    ss << joint->type;
    auto* value = new QStandardItem(QString::fromStdString(ss.str()));
    appendRow({ item, value });
  }

  if (joint->type == tesseract_scene_graph::JointType::REVOLUTE ||
      joint->type == tesseract_scene_graph::JointType::PRISMATIC ||
      joint->type == tesseract_scene_graph::JointType::PLANAR)
  {
    auto* item = new QStandardItem(icons::getAxisIcon(), "axis");
    item->setColumnCount(2);

    item->appendRow(createStandardItemFloat("x", joint->axis.x()));
    item->appendRow(createStandardItemFloat("y", joint->axis.y()));
    item->appendRow(createStandardItemFloat("z", joint->axis.z()));

    appendRow(item);
  }

  appendRow(createStandardItemString("child_link_name", joint->child_link_name));
  appendRow(createStandardItemString("parent_link_name", joint->parent_link_name));
  appendRow(new TransformStandardItem(joint->parent_to_joint_origin_transform));

  if (joint->dynamics != nullptr)
    appendRow(new DynamicsStandardItem(joint->dynamics));

  if (joint->limits != nullptr)
    appendRow(new LimitsStandardItem(joint->limits));

  if (joint->safety != nullptr)
    appendRow(new SafetyStandardItem(joint->safety));

  if (joint->calibration != nullptr)
    appendRow(new CalibrationStandardItem(joint->calibration));

  if (joint->mimic != nullptr)
    appendRow(new MimicStandardItem(joint->mimic));
}
}  // namespace tesseract_gui
