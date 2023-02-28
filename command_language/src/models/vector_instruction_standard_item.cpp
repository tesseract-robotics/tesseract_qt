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
#include <tesseract_qt/command_language/models/vector_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/move_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/null_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/set_analog_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/set_tool_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/timer_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/wait_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/instruction_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/poly/instruction_poly.h>
#include <tesseract_command_language/poly/move_instruction_poly.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/set_analog_instruction.h>
#include <tesseract_command_language/set_tool_instruction.h>
#include <tesseract_command_language/timer_instruction.h>
#include <tesseract_command_language/wait_instruction.h>
#include <tesseract_command_language/instruction_type.h>

namespace tesseract_gui
{
VectorInstructionStandardItem::VectorInstructionStandardItem(const std::vector<tesseract_planning::InstructionPoly>& vi)
  : QStandardItem(icons::getSetIcon(), "Instructions")
{
  ctor(vi);
}

VectorInstructionStandardItem::VectorInstructionStandardItem(const QString& text,
                                                             const std::vector<tesseract_planning::InstructionPoly>& vi)
  : QStandardItem(icons::getSetIcon(), text)
{
  ctor(vi);
}

VectorInstructionStandardItem::VectorInstructionStandardItem(const QIcon& icon,
                                                             const QString& text,
                                                             const std::vector<tesseract_planning::InstructionPoly>& vi)
  : QStandardItem(icon, text)
{
  ctor(vi);
}

int VectorInstructionStandardItem::type() const { return static_cast<int>(StandardItemType::CL_VECTOR_INSTRUCTION); }

void VectorInstructionStandardItem::ctor(const std::vector<tesseract_planning::InstructionPoly>& vi)
{
  for (const auto& instruction : vi)
  {
    if (instruction.isCompositeInstruction())
    {
      appendRow(new CompositeInstructionStandardItem(instruction.as<tesseract_planning::CompositeInstruction>()));
    }
    else if (instruction.isMoveInstruction())
    {
      appendRow(new MoveInstructionStandardItem(instruction.as<tesseract_planning::MoveInstructionPoly>()));
    }
    else if (instruction.isNull())
    {
      appendRow(new NullInstructionStandardItem());
    }
    else if (tesseract_planning::isSetAnalogInstruction(instruction))
    {
      appendRow(new SetAnalogInstructionStandardItem(instruction.as<tesseract_planning::SetAnalogInstruction>()));
    }
    else if (tesseract_planning::isSetToolInstruction(instruction))
    {
      appendRow(new SetToolInstructionStandardItem(instruction.as<tesseract_planning::SetToolInstruction>()));
    }
    else if (tesseract_planning::isTimerInstruction(instruction))
    {
      appendRow(new TimerInstructionStandardItem(instruction.as<tesseract_planning::TimerInstruction>()));
    }
    else if (tesseract_planning::isWaitInstruction(instruction))
    {
      appendRow(new WaitInstructionStandardItem(instruction.as<tesseract_planning::WaitInstruction>()));
    }
    else
    {
      appendRow(new InstructionStandardItem(instruction));
    }
  }
}
}  // namespace tesseract_gui
