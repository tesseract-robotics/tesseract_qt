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
#include <tesseract_qt/planning/task_composer_problem_standard_item.h>
#include <tesseract_qt/planning/planning_profile_remapping_standard_item.h>
#include <tesseract_qt/planning/task_composer_data_storage_standard_item.h>

#include <tesseract_qt/scene_graph/models/scene_state_standard_item.h>

#include <tesseract_qt/environment/models/environment_commands_standard_item.h>

#include <tesseract_qt/common/models/manipulator_info_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/planning/planning_task_composer_problem.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/poly/instruction_poly.h>

namespace tesseract_gui
{
TaskComposerProblemStandardItem::TaskComposerProblemStandardItem(const tesseract_planning::TaskComposerProblem& problem)
  : QStandardItem(icons::getUnknownIcon(), "Task Composer Problem")
{
  ctor(problem);
}

TaskComposerProblemStandardItem::TaskComposerProblemStandardItem(const QString& text,
                                                                 const tesseract_planning::TaskComposerProblem& problem)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(problem);
}

TaskComposerProblemStandardItem::TaskComposerProblemStandardItem(const QIcon& icon,
                                                                 const QString& text,
                                                                 const tesseract_planning::TaskComposerProblem& problem)
  : QStandardItem(icon, text)
{
  ctor(problem);
}

int TaskComposerProblemStandardItem::type() const
{
  return static_cast<int>(StandardItemType::MP_TASK_COMPOSER_PROBLEM);
}

void TaskComposerProblemStandardItem::ctor(const tesseract_planning::TaskComposerProblem& problem)
{
  appendRow(createStandardItemString("name", problem.name));
  appendRow(new TaskComposerDataStorageStandardItem("input_data", problem.input_data));

  // Get the problem
  const auto* planning_problem = dynamic_cast<const tesseract_planning::PlanningTaskComposerProblem*>(&problem);
  if (planning_problem == nullptr)
    return;

  auto* environment = new QStandardItem(icons::getModelIcon(), "environment");
  environment->appendRow(new EnvironmentCommandsStandardItem("commands", planning_problem->env->getCommandHistory()));
  environment->appendRow(new SceneStateStandardItem("state", planning_problem->env->getState()));
  appendRow(environment);

  appendRow(new ManipulatorInfoStandardItem("global_manip_info", planning_problem->manip_info));

  appendRow(
      new PlannerProfileRemappingStandardItem("move_profile_remapping", planning_problem->move_profile_remapping));
  appendRow(new PlannerProfileRemappingStandardItem("composite_profile_remapping",
                                                    planning_problem->composite_profile_remapping));
}
}  // namespace tesseract_gui
