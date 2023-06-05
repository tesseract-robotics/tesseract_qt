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

#include <tesseract_qt/joint_trajectory/models/joint_trajectory_info_item.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_state_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
JointTrajectoryInfoItem::JointTrajectoryInfoItem(tesseract_common::JointTrajectoryInfo& trajectory_info)
  : trajectory_info(trajectory_info)
{
  ctor();
}

JointTrajectoryInfoItem::JointTrajectoryInfoItem(const QString& text,
                                                 tesseract_common::JointTrajectoryInfo& trajectory_info)
  : QStandardItem(icons::getTrajectoryIcon(), text), trajectory_info(trajectory_info)
{
  ctor();
}

JointTrajectoryInfoItem::JointTrajectoryInfoItem(const QIcon& icon,
                                                 const QString& text,
                                                 tesseract_common::JointTrajectoryInfo& trajectory_info)
  : QStandardItem(icon, text), trajectory_info(trajectory_info)
{
  ctor();
}

int JointTrajectoryInfoItem::type() const
{
  return static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET_TRAJECTORY);
}

void JointTrajectoryInfoItem::ctor()
{
  for (std::size_t j = 0; j < trajectory_info.second.size(); ++j)
  {
    QStandardItem* trajectory_state =
        new JointTrajectoryStateItem(QString("state[%1]").arg(j), trajectory_info.second[j]);
    appendRow(trajectory_state);
  }
}
}  // namespace tesseract_gui
