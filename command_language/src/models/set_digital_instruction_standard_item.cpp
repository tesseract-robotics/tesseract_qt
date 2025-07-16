/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2025 Levi Armstrong <levi.armstrong@gmail.com>
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
#include <tesseract_qt/command_language/models/set_digital_instruction_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/set_digital_instruction.h>

#include <boost/uuid/uuid_io.hpp>

namespace tesseract_gui
{
SetDigitalInstructionStandardItem::SetDigitalInstructionStandardItem(
    const tesseract_planning::SetDigitalInstruction& sdi)
  : QStandardItem(icons::getUnknownIcon(), "Set Digital Instruction")
{
  ctor(sdi);
}

SetDigitalInstructionStandardItem::SetDigitalInstructionStandardItem(
    const QString& text,
    const tesseract_planning::SetDigitalInstruction& sdi)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(sdi);
}

SetDigitalInstructionStandardItem::SetDigitalInstructionStandardItem(
    const QIcon& icon,
    const QString& text,
    const tesseract_planning::SetDigitalInstruction& sdi)
  : QStandardItem(icon, text)
{
  ctor(sdi);
}

int SetDigitalInstructionStandardItem::type() const
{
  return static_cast<int>(StandardItemType::CL_SET_DIGITAL_INSTRUCTION);
}

void SetDigitalInstructionStandardItem::ctor(const tesseract_planning::SetDigitalInstruction& sdi)
{
  appendRow(createStandardItemString("description", sdi.getDescription()));
  appendRow(createStandardItemString("uuid", boost::uuids::to_string(sdi.getUUID())));
  appendRow(createStandardItemString("parent uuid", boost::uuids::to_string(sdi.getParentUUID())));
  appendRow(createStandardItemString("key", sdi.getKey()));
  appendRow(createStandardItemInt("index", sdi.getIndex()));
  appendRow(createStandardItemBool("value", sdi.getValue()));
}
}  // namespace tesseract_gui
