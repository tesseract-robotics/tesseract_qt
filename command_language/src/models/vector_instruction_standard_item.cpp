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
#include <tesseract_qt/command_language/models/null_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/instruction_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/factories/instruction_poly_standard_item_factory.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/poly/instruction_poly.h>

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
    if (instruction.isNull())
    {
      appendRow(new NullInstructionStandardItem());  // NOLINT
    }
    else
    {
      QList<QStandardItem*> items = InstructionPolyStandardItemManager::create(instruction);
      if (items.empty())
        appendRow(new InstructionStandardItem(instruction));  // NOLINT
      else
        appendRow(items);
    }
  }
}
}  // namespace tesseract_gui
