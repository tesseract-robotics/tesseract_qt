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
#include <tesseract_qt/command_language/models/timer_instruction_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/timer_instruction.h>

#include <boost/uuid/uuid_io.hpp>

namespace tesseract_gui
{
TimerInstructionStandardItem::TimerInstructionStandardItem(const tesseract_planning::TimerInstruction& ti)
  : QStandardItem(icons::getUnknownIcon(), "Timer Instruction")
{
  ctor(ti);
}

TimerInstructionStandardItem::TimerInstructionStandardItem(const QString& text,
                                                           const tesseract_planning::TimerInstruction& ti)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(ti);
}

TimerInstructionStandardItem::TimerInstructionStandardItem(const QIcon& icon,
                                                           const QString& text,
                                                           const tesseract_planning::TimerInstruction& ti)
  : QStandardItem(icon, text)
{
  ctor(ti);
}

int TimerInstructionStandardItem::type() const { return static_cast<int>(StandardItemType::CL_TIMER_INSTRUCTION); }

std::string toString(tesseract_planning::TimerInstructionType t_type)
{
  switch (t_type)
  {
    case tesseract_planning::TimerInstructionType::DIGITAL_OUTPUT_HIGH:
      return "DIGITAL_OUTPUT_HIGH";
    case tesseract_planning::TimerInstructionType::DIGITAL_OUTPUT_LOW:
      return "DIGITAL_OUTPUT_LOW";
    default:
      throw std::runtime_error("Unhandled tesseract_planning::TimerInstructionType");
  }
}

void TimerInstructionStandardItem::ctor(const tesseract_planning::TimerInstruction& ti)
{
  appendRow(createStandardItemString("description", ti.getDescription()));
  appendRow(createStandardItemString("uuid", boost::uuids::to_string(ti.getUUID())));
  appendRow(createStandardItemString("parent uuid", boost::uuids::to_string(ti.getParentUUID())));
  appendRow(createStandardItemString("type", toString(ti.getTimerType())));
  appendRow(createStandardItemFloat("time (sec)", ti.getTimerTime()));
  appendRow(createStandardItemInt("I/O", ti.getTimerIO()));
}
}  // namespace tesseract_gui
