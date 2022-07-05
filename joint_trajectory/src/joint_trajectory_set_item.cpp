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

#include <tesseract_qt/joint_trajectory/joint_trajectory_set_item.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_info_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
JointTrajectorySetItem::JointTrajectorySetItem(QString uuid, const tesseract_common::JointTrajectorySet& trajectory_set)
  : QStandardItem(icons::getSetIcon(), "Trajectory Set"), uuid(std::move(uuid)), trajectory_set(trajectory_set)
{
  ctor();
}

JointTrajectorySetItem::JointTrajectorySetItem(const QString& text,
                                               QString uuid,
                                               const tesseract_common::JointTrajectorySet& trajectory_set)
  : QStandardItem(icons::getSetIcon(), text), uuid(std::move(uuid)), trajectory_set(trajectory_set)
{
  ctor();
}
JointTrajectorySetItem::JointTrajectorySetItem(const QIcon& icon,
                                               const QString& text,
                                               QString uuid,
                                               const tesseract_common::JointTrajectorySet& trajectory_set)
  : QStandardItem(icon, text), uuid(std::move(uuid)), trajectory_set(trajectory_set)
{
  ctor();
}

int JointTrajectorySetItem::type() const { return static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET); }

void JointTrajectorySetItem::ctor()
{
  for (std::size_t i = 0; i < trajectory_set.size(); ++i)
  {
    tesseract_common::JointTrajectoryInfo& traj_info = trajectory_set[i];
    QStandardItem* trajectory_item = new JointTrajectoryInfoItem(QString("trajectory[%1]").arg(i), traj_info);
    appendRow(trajectory_item);
  }
}
}  // namespace tesseract_gui
