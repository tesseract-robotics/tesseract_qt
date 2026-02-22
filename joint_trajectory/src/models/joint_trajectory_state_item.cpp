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

#include <tesseract_qt/joint_trajectory/models/joint_trajectory_state_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_common/joint_state.h>

namespace tesseract::gui
{
JointTrajectoryStateItem::JointTrajectoryStateItem(tesseract::common::JointState& state) : state(state) { ctor(); }

JointTrajectoryStateItem::JointTrajectoryStateItem(const QString& text, tesseract::common::JointState& state)
  : QStandardItem(icons::getRobotArmIcon(), text), state(state)
{
  ctor();
}

JointTrajectoryStateItem::JointTrajectoryStateItem(const QIcon& icon,
                                                   const QString& text,
                                                   tesseract::common::JointState& state)
  : QStandardItem(icon, text), state(state)
{
  ctor();
}

int JointTrajectoryStateItem::type() const { return static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET_STATE); }

void JointTrajectoryStateItem::ctor()
{
  // Add State Joint Names
  auto* state_joint_names = new QStandardItem("joint_names");
  state_joint_names->setColumnCount(2);
  for (std::size_t k = 0; k < state.joint_names.size(); ++k)
    state_joint_names->appendRow(createStandardItemString(QString("[%1]").arg(k).toStdString(), state.joint_names[k]));

  appendRow(state_joint_names);

  // Add State Position
  auto* state_position = new QStandardItem("position");
  state_position->setColumnCount(2);
  for (Eigen::Index k = 0; k < state.position.rows(); ++k)
    state_position->appendRow(createStandardItemFloat(QString("[%1]").arg(k).toStdString(), state.position[k]));

  appendRow(state_position);

  // Add State Velocity
  auto* state_velocity = new QStandardItem("velocity");
  state_velocity->setColumnCount(2);
  for (int k = 0; k < state.velocity.rows(); ++k)
    state_velocity->appendRow(createStandardItemFloat(QString("[%1]").arg(k).toStdString(), state.velocity[k]));

  appendRow(state_velocity);

  // Add State Velocity
  auto* state_acceleration = new QStandardItem("acceleration");
  state_acceleration->setColumnCount(2);
  for (int k = 0; k < state.acceleration.rows(); ++k)
    state_acceleration->appendRow(createStandardItemFloat(QString("[%1]").arg(k).toStdString(), state.acceleration[k]));

  appendRow(state_acceleration);

  // Add time from start
  appendRow(createStandardItemFloat("time", state.time));
}
}  // namespace tesseract::gui
