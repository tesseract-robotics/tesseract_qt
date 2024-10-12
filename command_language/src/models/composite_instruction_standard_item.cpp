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
#include <tesseract_qt/command_language/models/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/vector_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/move_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/null_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/instruction_standard_item.h>
#include <tesseract_qt/common/models/manipulator_info_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/move_instruction.h>
#include <tesseract_command_language/poly/instruction_poly.h>

#include <boost/uuid/uuid_io.hpp>

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

const tesseract_planning::CompositeInstruction& CompositeInstructionStandardItem::getCompositeInstruction() const
{
  return ci_;
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
  ci_ = ci;
  appendRow(createStandardItemString("description", ci.getDescription()));
  appendRow(createStandardItemString("order", toString(ci.getOrder())));
  appendRow(createStandardItemString("profile", ci.getProfile()));
  appendRow(createStandardItemString("uuid", boost::uuids::to_string(ci.getUUID())));
  appendRow(createStandardItemString("parent uuid", boost::uuids::to_string(ci.getParentUUID())));
  appendRow(new ManipulatorInfoStandardItem("manip info", ci.getManipulatorInfo()));   // NOLINT
  appendRow(new VectorInstructionStandardItem("instructions", ci.getInstructions()));  // NOLINT
}
}  // namespace tesseract_gui
