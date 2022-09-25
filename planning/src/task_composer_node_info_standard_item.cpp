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
#include <tesseract_qt/planning/task_composer_node_info_standard_item.h>

#include <tesseract_qt/command_language/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/null_instruction_standard_item.h>
#include <tesseract_qt/command_language/instruction_standard_item.h>

#include <tesseract_qt/scene_graph/scene_state_standard_item.h>

#include <tesseract_qt/environment/environment_commands_standard_item.h>

#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/task_composer_node_info.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/poly/instruction_poly.h>

#include <boost/uuid/uuid_io.hpp>

namespace tesseract_gui
{
TaskComposerNodeInfoStandardItem::TaskComposerNodeInfoStandardItem(const tesseract_planning::TaskComposerNodeInfo& info)
  : QStandardItem(icons::getUnknownIcon(), "Task Composer Node Info")
{
  ctor(info);
}

TaskComposerNodeInfoStandardItem::TaskComposerNodeInfoStandardItem(const QString& text,
                                                                   const tesseract_planning::TaskComposerNodeInfo& info)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(info);
}

TaskComposerNodeInfoStandardItem::TaskComposerNodeInfoStandardItem(const QIcon& icon,
                                                                   const QString& text,
                                                                   const tesseract_planning::TaskComposerNodeInfo& info)
  : QStandardItem(icon, text)
{
  ctor(info);
}

int TaskComposerNodeInfoStandardItem::type() const
{
  return static_cast<int>(StandardItemType::MP_TASK_COMPOSER_NODE_INFO);
}

void TaskComposerNodeInfoStandardItem::ctor(const tesseract_planning::TaskComposerNodeInfo& info)
{
  appendRow(createStandardItemString("name", info.name));
  appendRow(createStandardItemString("uuid", boost::uuids::to_string(info.uuid)));
  appendRow(createStandardItemInt("return_value", info.return_value));
  appendRow(createStandardItemString("message", info.message));
  appendRow(createStandardItemFloat("elapsed_time", info.elapsed_time));

  //  if (task_info.instructions_input.isNull())
  //    appendRow(new NullInstructionStandardItem("instructions_input"));
  //  else if (task_info.instructions_input.isCompositeInstruction())
  //    appendRow(new CompositeInstructionStandardItem(
  //        "instructions_input", task_info.instructions_input.as<tesseract_planning::CompositeInstruction>()));
  //  else
  //    appendRow(new InstructionStandardItem("instructions_input", task_info.instructions_input));

  //  if (task_info.instructions_output.isNull())
  //    appendRow(new NullInstructionStandardItem("instructions_output"));
  //  else if (task_info.instructions_output.isCompositeInstruction())
  //    appendRow(new CompositeInstructionStandardItem(
  //        "instructions_output", task_info.instructions_output.as<tesseract_planning::CompositeInstruction>()));
  //  else
  //    appendRow(new InstructionStandardItem("instructions_output", task_info.instructions_output));

  //  if (task_info.results_input.isNull())
  //    appendRow(new NullInstructionStandardItem("results_input"));
  //  else if (task_info.results_input.isCompositeInstruction())
  //    appendRow(new CompositeInstructionStandardItem(
  //        "results_input", task_info.results_input.as<tesseract_planning::CompositeInstruction>()));
  //  else
  //    appendRow(new InstructionStandardItem("results_input", task_info.results_input));

  //  if (task_info.results_output.isNull())
  //    appendRow(new NullInstructionStandardItem("results_output"));
  //  else if (task_info.results_output.isCompositeInstruction())
  //    appendRow(new CompositeInstructionStandardItem(
  //        "results_output", task_info.results_output.as<tesseract_planning::CompositeInstruction>()));
  //  else
  //    appendRow(new InstructionStandardItem("results_output", task_info.results_output));

  //  auto* environment = new QStandardItem(icons::getModelIcon(), "environment");
  //  QStandardItem* environment_desc;
  //  if (task_info.environment != nullptr)
  //  {
  //    environment_desc = new QStandardItem(task_info.environment->getName().c_str());
  //    environment->appendRow(new EnvironmentCommandsStandardItem("commands",
  //    task_info.environment->getCommandHistory())); environment->appendRow(new SceneStateStandardItem("state",
  //    task_info.environment->getState()));
  //  }
  //  else
  //  {
  //    environment_desc = new QStandardItem("NULL");
  //  }
  //  appendRow({ environment, environment_desc });
}
}  // namespace tesseract_gui
