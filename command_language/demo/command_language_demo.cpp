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
#include <QTreeView>
#include <QDebug>
#include <sstream>

#include <tesseract_qt/command_language/widgets/composite_instruction_widget.h>
#include <tesseract_qt/common/events/command_language_events.h>
#include <tesseract_qt/common/widgets/tree_view.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>

#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/cartesian_waypoint.h>
#include <tesseract_command_language/move_instruction.h>
#include <tesseract_command_language/state_waypoint.h>

using namespace tesseract_planning;
using tesseract_common::ManipulatorInfo;

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  // Create Program
  CompositeInstruction program("cartesian_program", ManipulatorInfo("manipulator", "base_link", "tool0"));

  // Set the robot initial state
  std::vector<std::string> joint_names;
  joint_names.emplace_back("joint_a1");
  joint_names.emplace_back("joint_a2");
  joint_names.emplace_back("joint_a3");
  joint_names.emplace_back("joint_a4");
  joint_names.emplace_back("joint_a5");
  joint_names.emplace_back("joint_a6");
  joint_names.emplace_back("joint_a7");

  Eigen::VectorXd joint_pos(7);
  joint_pos(0) = -0.4;
  joint_pos(1) = 0.2762;
  joint_pos(2) = 0.0;
  joint_pos(3) = -1.3348;
  joint_pos(4) = 0.0;
  joint_pos(5) = 1.4959;
  joint_pos(6) = 0.0;

  // Start Joint Position for the program
  StateWaypointPoly wp0{ StateWaypoint(joint_names, joint_pos) };
  MoveInstruction start_instruction(wp0, MoveInstructionType::FREESPACE);
  program.push_back(start_instruction);

  // Create cartesian waypoint
  CartesianWaypoint wp1{ Eigen::Isometry3d::Identity() * Eigen::Translation3d(0.5, -0.2, 0.62) *
                         Eigen::Quaterniond(0, 0, 1.0, 0) };

  CartesianWaypoint wp2{ Eigen::Isometry3d::Identity() * Eigen::Translation3d(0.5, 0.3, 0.62) *
                         Eigen::Quaterniond(0, 0, 1.0, 0) };

  // Plan freespace from start
  MoveInstruction plan_f0(wp1, MoveInstructionType::FREESPACE, "freespace_profile");
  plan_f0.setDescription("from_start_plan");

  // Plan linear move
  MoveInstruction plan_c0(wp2, MoveInstructionType::LINEAR, "RASTER");

  // Plan freespace to end
  MoveInstruction plan_f1(wp0, MoveInstructionType::FREESPACE, "freespace_profile");
  plan_f1.setDescription("to_end_plan");

  // Add Instructions to program
  program.push_back(start_instruction);
  program.push_back(plan_f0);
  program.push_back(plan_c0);
  program.push_back(plan_f1);

  std::shared_ptr<const tesseract_gui::ComponentInfo> component_info = tesseract_gui::ComponentInfoManager::create("sce"
                                                                                                                   "ne_"
                                                                                                                   "nam"
                                                                                                                   "e");
  tesseract_gui::CompositeInstructionWidget widget(component_info);
  widget.show();

  tesseract_gui::events::CompositeInstructionSet event(component_info, program, "general");
  QApplication::sendEvent(qApp, &event);

  return QApplication::exec();
}
