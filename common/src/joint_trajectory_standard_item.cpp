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

#include <tesseract_qt/common/joint_trajectory_standard_item.h>
#include <tesseract_qt/common/joint_state_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_common/joint_state.h>

namespace tesseract_gui
{
JointTrajectoryStandardItem::JointTrajectoryStandardItem(const tesseract_common::JointTrajectory& joint_trajectory)
  : QStandardItem(icons::getTrajectoryIcon(), "Joint Trajectory")
{
  ctor(joint_trajectory);
}

JointTrajectoryStandardItem::JointTrajectoryStandardItem(const QString& text,
                                                         const tesseract_common::JointTrajectory& joint_trajectory)
  : QStandardItem(icons::getTrajectoryIcon(), text)
{
  ctor(joint_trajectory);
}

JointTrajectoryStandardItem::JointTrajectoryStandardItem(const QIcon& icon,
                                                         const QString& text,
                                                         const tesseract_common::JointTrajectory& joint_trajectory)
  : QStandardItem(icon, text)
{
  ctor(joint_trajectory);
}

int JointTrajectoryStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_JOINT_TRAJECTORY); }

void JointTrajectoryStandardItem::ctor(const tesseract_common::JointTrajectory& joint_trajectory)
{
  for (std::size_t j = 0; j < joint_trajectory.size(); ++j)
    appendRow(new JointStateStandardItem(QString("state[%1]").arg(j), joint_trajectory[j]));
}
}  // namespace tesseract_gui
