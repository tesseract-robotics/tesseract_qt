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

#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QDockWidget>
#include <memory>

#include <tesseract_qt/rendering/render_widget.h>
#include <tesseract_qt/rendering/ign_scene_graph_render_manager.h>
#include <tesseract_qt/rendering/ign_tool_path_render_manager.h>
#include <tesseract_qt/rendering/ign_contact_results_render_manager.h>

#include <tesseract_qt/environment/widgets/environment_widget.h>

#include <tesseract_qt/scene_graph/widgets/scene_graph_tool_bar.h>

#include <tesseract_qt/tool_path/widgets/tool_path_tool_bar.h>
#include <tesseract_qt/tool_path/widgets/tool_path_widget.h>

#include <tesseract_qt/manipulation/manipulation_widget.h>
#include <tesseract_qt/manipulation/manipulation_tool_bar.h>

#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_tool_bar.h>
#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_widget.h>

#include <tesseract_qt/common/events/tool_path_events.h>
#include <tesseract_qt/common/events/joint_trajectory_events.h>

#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/tool_path.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_support/tesseract_support_resource_locator.h>
#include <tesseract_environment/environment.h>
#include <tesseract_common/joint_state.h>

tesseract_gui::ToolPath getToolPath()
{
  tesseract_gui::ToolPath tool_path("Demo Tool Path");
  for (int i = 0; i < 5; ++i)
  {
    tesseract_gui::ToolPathSegment segment("Segment [" + std::to_string(i) + "]");

    Eigen::Isometry3d pose{ Eigen::Isometry3d::Identity() };
    for (Eigen::Index k = 0; k < 10; ++k)
    {
      pose.translation() = Eigen::Vector3d(-0.5 + k * 0.1, 0.5 - i * 0.05, 0.5);
      segment.push_back(pose);
    }

    tool_path.push_back(segment);
  }

  return tool_path;
}

tesseract_common::JointTrajectorySet getJointTrajectorySet()
{
  std::unordered_map<std::string, double> initial_state{ { "joint_a1", 0 }, { "joint_a2", 0 }, { "joint_a3", 0 },
                                                         { "joint_a4", 0 }, { "joint_a5", 0 }, { "joint_a6", 0 },
                                                         { "joint_a7", 0 } };

  tesseract_common::JointTrajectorySet trajectory_set(initial_state);
  for (int i = 0; i < 5; ++i)
  {
    tesseract_common::JointTrajectory trajectory("description");

    for (int j = 0; j < 5; ++j)
    {
      tesseract_common::JointState state;
      state.joint_names = { "joint_a1", "joint_a2", "joint_a3", "joint_a4", "joint_a5", "joint_a6", "joint_a7" };
      state.position.resize(7);
      state.velocity.resize(7);
      state.acceleration.resize(7);
      state.time = j;

      // Add State Position
      for (Eigen::Index k = 0; k < 7; ++k)
      {
        state.position[k] = k + j;
        state.velocity[k] = 2 * (k + j);
        state.acceleration[k] = 3 * (k + j);
      }
      trajectory.push_back(state);
    }
    trajectory_set.appendJointTrajectory(trajectory);
  }

  return trajectory_set;
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  auto locator = std::make_shared<tesseract_common::TesseractSupportResourceLocator>();
  tesseract_common::fs::path urdf_path = std::string(TESSERACT_SUPPORT_DIR) + "/urdf/lbr_iiwa_14_r820.urdf";
  tesseract_common::fs::path srdf_path = std::string(TESSERACT_SUPPORT_DIR) + "/urdf/lbr_iiwa_14_r820.srdf";

  auto env = std::make_shared<tesseract_environment::Environment>();
  env->init(urdf_path, srdf_path, locator);

  auto component_info = tesseract_gui::ComponentInfoManager::create(env->getName());
  auto jt_component_info = component_info->createChild();
  auto entity_manager = std::make_shared<tesseract_gui::EntityManager>();

  tesseract_gui::IgnSceneGraphRenderManager scene_graph_manager(component_info, entity_manager);
  tesseract_gui::IgnToolPathRenderManager tool_path_manager(component_info, entity_manager);
  tesseract_gui::IgnContactResultsRenderManager contact_results_manager(component_info);

  QMainWindow window;
  window.addToolBar(new tesseract_gui::SceneGraphToolBar(component_info));
  window.addToolBar(new tesseract_gui::ToolPathToolBar(component_info));
  window.addToolBar(new tesseract_gui::JointTrajectoryToolBar(jt_component_info));
  window.addToolBar(new tesseract_gui::ManipulationToolBar(component_info));

  auto render_widget = new tesseract_gui::RenderWidget(component_info->getSceneName());
  render_widget->setSkyEnabled(true);
  window.setCentralWidget(render_widget);

  auto* env_widget = new tesseract_gui::EnvironmentWidget(component_info);
  auto* dw = new QDockWidget();
  dw->setWidget(env_widget);
  window.addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dw);

  auto* tool_path_widget = new tesseract_gui::ToolPathWidget(component_info);
  auto* dw2 = new QDockWidget();
  dw2->setWidget(tool_path_widget);
  window.addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dw2);

  auto* joint_traj_widget = new tesseract_gui::JointTrajectoryWidget(jt_component_info);
  auto* dw3 = new QDockWidget();
  dw3->setWidget(joint_traj_widget);
  window.addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dw3);

  auto* manipulation_widget = new tesseract_gui::ManipulationWidget(component_info, true);
  auto* dw4 = new QDockWidget();
  dw4->setWidget(manipulation_widget);
  window.addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dw4);

  window.resize(1200, 800);
  window.show();

  tesseract_gui::EnvironmentManager::set(
      std::make_shared<tesseract_gui::DefaultEnvironmentWrapper>(component_info, env));

  tesseract_gui::ToolPath tool_path = getToolPath();
  tesseract_gui::events::ToolPathAdd event1(component_info, tool_path);
  QApplication::sendEvent(qApp, &event1);

  tesseract_common::JointTrajectorySet trajectory_set = getJointTrajectorySet();
  tesseract_gui::events::JointTrajectoryAdd event2(jt_component_info, trajectory_set);
  QApplication::sendEvent(qApp, &event2);

  return app.exec();
}
