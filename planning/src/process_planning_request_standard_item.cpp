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
#include <tesseract_qt/planning/process_planning_request_standard_item.h>
#include <tesseract_qt/planning/planning_profile_remapping_standard_item.h>

#include <tesseract_qt/command_language/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/null_instruction_standard_item.h>
#include <tesseract_qt/command_language/instruction_standard_item.h>

#include <tesseract_qt/scene_graph/scene_state_standard_item.h>

#include <tesseract_qt/environment/environment_commands_standard_item.h>

#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_process_managers/core/process_planning_request.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/null_instruction.h>
#include <tesseract_command_language/core/instruction.h>
#include <tesseract_command_language/instruction_type.h>

namespace tesseract_gui
{
ProcessPlanningRequestStandardItem::ProcessPlanningRequestStandardItem(
    const tesseract_planning::ProcessPlanningRequest& request)
  : QStandardItem(icons::getUnknownIcon(), "Process Planning Request")
{
  ctor(request);
}

ProcessPlanningRequestStandardItem::ProcessPlanningRequestStandardItem(
    const QString& text,
    const tesseract_planning::ProcessPlanningRequest& request)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(request);
}

ProcessPlanningRequestStandardItem::ProcessPlanningRequestStandardItem(
    const QIcon& icon,
    const QString& text,
    const tesseract_planning::ProcessPlanningRequest& request)
  : QStandardItem(icon, text)
{
  ctor(request);
}

int ProcessPlanningRequestStandardItem::type() const
{
  return static_cast<int>(StandardItemType::MP_PROCESS_PLANNING_REQUEST);
}

void ProcessPlanningRequestStandardItem::ctor(const tesseract_planning::ProcessPlanningRequest& request)
{
  appendRow(createStandardItemString("name", request.name));
  if (tesseract_planning::isNullInstruction(request.instructions))
    appendRow(new NullInstructionStandardItem("instructions",
                                              request.instructions.as<tesseract_planning::NullInstruction>()));
  else if (tesseract_planning::isCompositeInstruction(request.instructions))
    appendRow(new CompositeInstructionStandardItem(
        "instructions", request.instructions.as<tesseract_planning::CompositeInstruction>()));
  else
    appendRow(new InstructionStandardItem("instructions", request.instructions));

  if (tesseract_planning::isNullInstruction(request.seed))
    appendRow(new NullInstructionStandardItem("seed", request.instructions.as<tesseract_planning::NullInstruction>()));
  else if (tesseract_planning::isCompositeInstruction(request.seed))
    appendRow(new CompositeInstructionStandardItem(
        "seed", request.instructions.as<tesseract_planning::CompositeInstruction>()));
  else
    appendRow(new InstructionStandardItem("seed", request.seed));

  appendRow(new SceneStateStandardItem("env_state", request.env_state));
  appendRow(new EnvironmentCommandsStandardItem("commands", request.commands));

  appendRow(createStandardItemBool("profile", request.profile));
  appendRow(createStandardItemBool("save_io", request.save_io));

  appendRow(new PlannerProfileRemappingStandardItem("plan_profile_remapping", request.plan_profile_remapping));
  appendRow(
      new PlannerProfileRemappingStandardItem("composite_profile_remapping", request.composite_profile_remapping));
}
}  // namespace tesseract_gui
