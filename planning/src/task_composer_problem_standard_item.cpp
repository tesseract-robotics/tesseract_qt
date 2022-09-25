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

#include <tesseract_qt/command_language/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/null_instruction_standard_item.h>
#include <tesseract_qt/command_language/instruction_standard_item.h>

#include <tesseract_qt/scene_graph/scene_state_standard_item.h>

#include <tesseract_qt/environment/environment_commands_standard_item.h>

#include <tesseract_qt/common/manipulator_info_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/task_composer_problem.h>
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

  //  auto* environment = new QStandardItem(icons::getModelIcon(), "environment");
  //  environment->appendRow(new EnvironmentCommandsStandardItem("commands", problem.env->getCommandHistory()));
  //  environment->appendRow(new SceneStateStandardItem("state", problem.env->getState()));
  //  appendRow(environment);

  //  appendRow(new ManipulatorInfoStandardItem("global_manip_info", *problem.global_manip_info));

  //  appendRow(new PlannerProfileRemappingStandardItem("plan_profile_remapping", *problem.plan_profile_remapping));
  //  appendRow(
  //      new PlannerProfileRemappingStandardItem("composite_profile_remapping", *problem.composite_profile_remapping));

  //  if (problem.input == nullptr || (problem.input != nullptr && problem.input->isNull()))
  //    appendRow(new NullInstructionStandardItem("input"));
  //  else if (problem.input->isCompositeInstruction())
  //    appendRow(
  //        new CompositeInstructionStandardItem("input",
  //        problem.input->as<tesseract_planning::CompositeInstruction>()));
  //  else
  //    appendRow(new InstructionStandardItem("input", *problem.input));

  //  if (problem.results == nullptr || (problem.results != nullptr && problem.results->isNull()))
  //    appendRow(new NullInstructionStandardItem("results"));
  //  else if (problem.results->isCompositeInstruction())
  //    appendRow(new CompositeInstructionStandardItem("results",
  //                                                   problem.results->as<tesseract_planning::CompositeInstruction>()));
  //  else
  //    appendRow(new InstructionStandardItem("results", *problem.results));
}
}  // namespace tesseract_gui
