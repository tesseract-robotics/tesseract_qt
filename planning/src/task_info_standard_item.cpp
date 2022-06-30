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
#include <tesseract_qt/planning/task_info_standard_item.h>

#include <tesseract_qt/command_language/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/null_instruction_standard_item.h>
#include <tesseract_qt/command_language/instruction_standard_item.h>

#include <tesseract_qt/scene_graph/scene_state_standard_item.h>

#include <tesseract_qt/environment/environment_commands_standard_item.h>

#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_process_managers/core/task_info.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/null_instruction.h>
#include <tesseract_command_language/core/instruction.h>
#include <tesseract_command_language/instruction_type.h>

namespace tesseract_gui
{
TaskInfoStandardItem::TaskInfoStandardItem(const tesseract_planning::TaskInfo& task_info)
  : QStandardItem(icons::getUnknownIcon(), "Task Info")
{
  ctor(task_info);
}

TaskInfoStandardItem::TaskInfoStandardItem(const QString& text, const tesseract_planning::TaskInfo& task_info)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(task_info);
}

TaskInfoStandardItem::TaskInfoStandardItem(const QIcon& icon,
                                           const QString& text,
                                           const tesseract_planning::TaskInfo& task_info)
  : QStandardItem(icon, text)
{
  ctor(task_info);
}

int TaskInfoStandardItem::type() const { return static_cast<int>(StandardItemType::MP_TASK_INFO); }

void TaskInfoStandardItem::ctor(const tesseract_planning::TaskInfo& task_info)
{
  appendRow(createStandardItemInt("return_value", task_info.return_value));
  appendRow(createStandardItemInt("unique_id", static_cast<int>(task_info.unique_id)));
  appendRow(createStandardItemString("task_name", task_info.task_name));
  appendRow(createStandardItemString("message", task_info.message));
  appendRow(createStandardItemFloat("elapsed_time", task_info.elapsed_time));

  if (tesseract_planning::isNullInstruction(task_info.instructions_input))
    appendRow(new NullInstructionStandardItem("instructions_input",
                                              task_info.instructions_input.as<tesseract_planning::NullInstruction>()));
  else if (tesseract_planning::isCompositeInstruction(task_info.instructions_input))
    appendRow(new CompositeInstructionStandardItem(
        "instructions_input", task_info.instructions_input.as<tesseract_planning::CompositeInstruction>()));
  else
    appendRow(new InstructionStandardItem("instructions_input", task_info.instructions_input));

  if (tesseract_planning::isNullInstruction(task_info.instructions_output))
    appendRow(new NullInstructionStandardItem("instructions_output",
                                              task_info.instructions_output.as<tesseract_planning::NullInstruction>()));
  else if (tesseract_planning::isCompositeInstruction(task_info.instructions_output))
    appendRow(new CompositeInstructionStandardItem(
        "instructions_output", task_info.instructions_output.as<tesseract_planning::CompositeInstruction>()));
  else
    appendRow(new InstructionStandardItem("instructions_output", task_info.instructions_output));

  if (tesseract_planning::isNullInstruction(task_info.results_input))
    appendRow(new NullInstructionStandardItem("results_input",
                                              task_info.results_input.as<tesseract_planning::NullInstruction>()));
  else if (tesseract_planning::isCompositeInstruction(task_info.results_input))
    appendRow(new CompositeInstructionStandardItem(
        "results_input", task_info.results_input.as<tesseract_planning::CompositeInstruction>()));
  else
    appendRow(new InstructionStandardItem("results_input", task_info.results_input));

  if (tesseract_planning::isNullInstruction(task_info.results_output))
    appendRow(new NullInstructionStandardItem("results_output",
                                              task_info.results_output.as<tesseract_planning::NullInstruction>()));
  else if (tesseract_planning::isCompositeInstruction(task_info.results_output))
    appendRow(new CompositeInstructionStandardItem(
        "results_output", task_info.results_output.as<tesseract_planning::CompositeInstruction>()));
  else
    appendRow(new InstructionStandardItem("results_output", task_info.results_output));

  auto* environment = new QStandardItem(icons::getModelIcon(), "environment");
  QStandardItem* environment_desc;
  if (task_info.environment != nullptr)
  {
    environment_desc = new QStandardItem(task_info.environment->getName().c_str());
    environment->appendRow(new EnvironmentCommandsStandardItem("commands", task_info.environment->getCommandHistory()));
    environment->appendRow(new SceneStateStandardItem("state", task_info.environment->getState()));
  }
  else
  {
    environment_desc = new QStandardItem("NULL");
  }
  appendRow({ environment, environment_desc });
}
}  // namespace tesseract_gui
