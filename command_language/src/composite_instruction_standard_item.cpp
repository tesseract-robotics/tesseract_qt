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
#include <tesseract_qt/command_language/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/vector_instruction_standard_item.h>
#include <tesseract_qt/command_language/move_instruction_standard_item.h>
#include <tesseract_qt/command_language/null_instruction_standard_item.h>
#include <tesseract_qt/command_language/instruction_standard_item.h>
#include <tesseract_qt/common/manipulator_info_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/move_instruction.h>
#include <tesseract_command_language/null_instruction.h>
#include <tesseract_command_language/core/instruction.h>

namespace tesseract_gui
{
CompositeInstructionStandardItem::CompositeInstructionStandardItem(const tesseract_planning::CompositeInstruction& ci)
  : QStandardItem(icons::getUnknownIcon(), "Composite Instruction")
{
  ctor(ci);
}

CompositeInstructionStandardItem::CompositeInstructionStandardItem(const QString& text,
                                                                   const tesseract_planning::CompositeInstruction& ci)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(ci);
}

CompositeInstructionStandardItem::CompositeInstructionStandardItem(const QIcon& icon,
                                                                   const QString& text,
                                                                   const tesseract_planning::CompositeInstruction& ci)
  : QStandardItem(icon, text)
{
  ctor(ci);
}

int CompositeInstructionStandardItem::type() const
{
  return static_cast<int>(StandardItemType::CL_COMPOSITE_INSTRUCTION);
}

std::string toString(tesseract_planning::CompositeInstructionOrder order)
{
  switch (order)
  {
    case tesseract_planning::CompositeInstructionOrder::ORDERED:
      return "ORDERED";
    case tesseract_planning::CompositeInstructionOrder::UNORDERED:
      return "UNORDERED";
    case tesseract_planning::CompositeInstructionOrder::ORDERED_AND_REVERABLE:
      return "ORDERED_AND_REVERABLE";
    default:
      throw std::runtime_error("Unhandled tesseract_planning::CompositeInstructionOrder");
  }
}

void CompositeInstructionStandardItem::ctor(const tesseract_planning::CompositeInstruction& ci)
{
  appendRow(createStandardItemString("description", ci.getDescription()));
  appendRow(createStandardItemString("order", toString(ci.getOrder())));
  appendRow(createStandardItemString("profile", ci.getProfile()));
  appendRow(new ManipulatorInfoStandardItem("manip info", ci.getManipulatorInfo()));
  if (tesseract_planning::isMoveInstruction(ci.getStartInstruction()))
  {
    auto* item = new MoveInstructionStandardItem("start instruction",
                                                 ci.getStartInstruction().as<tesseract_planning::MoveInstruction>());
    auto* desc = new QStandardItem("Move Instruction");
    appendRow({ item, desc });
  }
  else if (tesseract_planning::isNullInstruction(ci.getStartInstruction()))
  {
    auto* item = new NullInstructionStandardItem("start instruction",
                                                 ci.getStartInstruction().as<tesseract_planning::NullInstruction>());
    auto* desc = new QStandardItem("Null Instruction");
    appendRow({ item, desc });
  }
  else
  {
    auto* item = new InstructionStandardItem("start instruction", ci.getStartInstruction());
    auto* desc = new QStandardItem("Unknown Instruction");
    appendRow({ item, desc });
  }

  appendRow(new VectorInstructionStandardItem("instructions", ci.getInstructions()));
}
}  // namespace tesseract_gui
