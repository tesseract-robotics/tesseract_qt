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
#include <tesseract_qt/command_language/wait_instruction_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/wait_instruction.h>

namespace tesseract_gui
{
WaitInstructionStandardItem::WaitInstructionStandardItem(const tesseract_planning::WaitInstruction& wi)
  : QStandardItem(icons::getUnknownIcon(), "Wait Instruction")
{
  ctor(wi);
}

WaitInstructionStandardItem::WaitInstructionStandardItem(const QString& text,
                                                         const tesseract_planning::WaitInstruction& wi)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(wi);
}

WaitInstructionStandardItem::WaitInstructionStandardItem(const QIcon& icon,
                                                         const QString& text,
                                                         const tesseract_planning::WaitInstruction& wi)
  : QStandardItem(icon, text)
{
  ctor(wi);
}

int WaitInstructionStandardItem::type() const { return static_cast<int>(StandardItemType::CL_WAIT_INSTRUCTION); }

std::string toString(tesseract_planning::WaitInstructionType w_type)
{
  switch (w_type)
  {
    case tesseract_planning::WaitInstructionType::TIME:
      return "TIME";
    case tesseract_planning::WaitInstructionType::DIGITAL_INPUT_HIGH:
      return "DIGITAL_INPUT_HIGH";
    case tesseract_planning::WaitInstructionType::DIGITAL_INPUT_LOW:
      return "DIGITAL_INPUT_LOW";
    case tesseract_planning::WaitInstructionType::DIGITAL_OUTPUT_HIGH:
      return "DIGITAL_OUTPUT_HIGH";
    case tesseract_planning::WaitInstructionType::DIGITAL_OUTPUT_LOW:
      return "DIGITAL_OUTPUT_LOW";
    default:
      throw std::runtime_error("Unhandled tesseract_planning::WaitInstructionType");
  }
}

void WaitInstructionStandardItem::ctor(const tesseract_planning::WaitInstruction& wi)
{
  appendRow(createStandardItemString("description", wi.getDescription()));
  appendRow(createStandardItemString("type", toString(wi.getWaitType())));
  appendRow(createStandardItemFloat("time (sec)", wi.getWaitTime()));
  appendRow(createStandardItemInt("I/O", wi.getWaitIO()));
}
}  // namespace tesseract_gui
