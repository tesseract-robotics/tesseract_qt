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
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <QApplication>
#include <QStandardItemModel>
#include <QDebug>
#include <QTreeView>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/joint_trajectory/joint_trajectory_model.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_widget.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  std::unordered_map<std::string, double> initial_state{
    { "joint_1", 0 }, { "joint_2", 0 }, { "joint_3", 0 }, { "joint_4", 0 }, { "joint_5", 0 }
  };

  tesseract_common::JointTrajectorySet trajectory_set(initial_state);
  for (int i = 0; i < 5; ++i)
  {
    tesseract_common::JointTrajectory trajectory("description");

    for (int j = 0; j < 5; ++j)
    {
      tesseract_common::JointState state;
      state.joint_names = { "joint_1", "joint_2", "joint_3", "joint_4", "joint_5" };
      state.position.resize(5);
      state.velocity.resize(5);
      state.acceleration.resize(5);
      state.time = j;

      // Add State Position
      for (Eigen::Index k = 0; k < 5; ++k)
      {
        state.position[k] = k + j;
        state.velocity[k] = 2 * (k + j);
        state.acceleration[k] = 3 * (k + j);
      }
      trajectory.push_back(state);
    }
    trajectory_set.appendJointTrajectory(trajectory);
  }

  auto* model = new tesseract_gui::JointTrajectoryModel();  // NOLINT
  QString set1_key = model->addJointTrajectorySet(trajectory_set);
  QString set2_key = model->addJointTrajectorySet(trajectory_set);
  QString set3_key = model->addJointTrajectorySet(trajectory_set);

  model->removeJointTrajectorySet(set2_key);

  tesseract_gui::JointTrajectoryWidget widget;
  widget.setModel(model);
  widget.show();

  return QApplication::exec();
}
