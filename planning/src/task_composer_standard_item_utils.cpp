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

#include <tesseract_qt/planning/task_composer_standard_item_utils.h>
#include <tesseract_qt/command_language/composite_instruction_standard_item.h>
#include <tesseract_qt/command_language/move_instruction_standard_item.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/task_composer_data_storage.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/move_instruction.h>
#include <tesseract_common/any_poly.h>

namespace tesseract_gui
{
QList<QStandardItem*> createStandardItemAnyPoly(const QString& key, const tesseract_common::AnyPoly& any_poly)
{
  if (any_poly.getType() == std::type_index(typeid(tesseract_planning::CompositeInstruction)))
    return { new CompositeInstructionStandardItem(key, any_poly.as<tesseract_planning::CompositeInstruction>()) };

  if (any_poly.getType() == std::type_index(typeid(tesseract_planning::InstructionPoly)))
  {
    if (any_poly.getType() == std::type_index(typeid(tesseract_planning::CompositeInstruction)))
      return { new CompositeInstructionStandardItem(key, any_poly.as<tesseract_planning::CompositeInstruction>()) };

    if (any_poly.getType() == std::type_index(typeid(tesseract_planning::MoveInstruction)))
      return { new MoveInstructionStandardItem(key, any_poly.as<tesseract_planning::MoveInstruction>()) };
  }

  auto* name = new QStandardItem(icons::getUnknownIcon(), key);
  auto* value = new QStandardItem();  // NOLINT
  value->setData("Unknown any type", Qt::DisplayRole);
  return { name, value };
}
}  // namespace tesseract_gui
