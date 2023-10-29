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
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <QApplication>
#include <QStandardItemModel>
#include <QDebug>
#include <QMainWindow>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/common/joint_trajectory_set.h>
#include <tesseract_qt/common/events/joint_trajectory_events.h>

#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_widget.h>
#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_tool_bar.h>

#include <tesseract_qt/common/component_info_manager.h>

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

  auto trajectory_set1{ trajectory_set };
  trajectory_set1.setDescription("Demo Tool Path 1");
  trajectory_set1.regenerateUUID();

  auto trajectory_set2{ trajectory_set };
  trajectory_set2.setDescription("Demo Tool Path 2");
  trajectory_set2.regenerateUUID();

  auto trajectory_set3{ trajectory_set };
  trajectory_set3.setDescription("Demo Tool Path 3");
  trajectory_set3.regenerateUUID();

  auto component_info = tesseract_gui::ComponentInfoManager::create("scene_name");

  QMainWindow window;
  window.addToolBar(new tesseract_gui::JointTrajectoryToolBar(component_info));
  window.setCentralWidget(new tesseract_gui::JointTrajectoryWidget(component_info));
  window.show();

  tesseract_gui::events::JointTrajectoryAdd event(component_info, trajectory_set);
  QApplication::sendEvent(qApp, &event);

  tesseract_gui::events::JointTrajectoryAdd event1(component_info, trajectory_set1);
  QApplication::sendEvent(qApp, &event1);

  tesseract_gui::events::JointTrajectoryAdd event2(component_info, trajectory_set2);
  QApplication::sendEvent(qApp, &event2);

  tesseract_gui::events::JointTrajectoryAdd event3(component_info, trajectory_set3);
  QApplication::sendEvent(qApp, &event3);

  tesseract_gui::events::JointTrajectoryRemove event4(component_info, trajectory_set2.getUUID());
  QApplication::sendEvent(qApp, &event4);

  return QApplication::exec();
}
