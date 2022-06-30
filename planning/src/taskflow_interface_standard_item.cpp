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
#include <tesseract_qt/planning/taskflow_interface_standard_item.h>
#include <tesseract_qt/planning/task_info_map_standard_item.h>

#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_process_managers/core/taskflow_interface.h>

namespace tesseract_gui
{
TaskflowInterfaceStandardItem::TaskflowInterfaceStandardItem(
    const tesseract_planning::TaskflowInterface& taskflow_interface)
  : QStandardItem(icons::getUnknownIcon(), "Task Info Map")
{
  ctor(taskflow_interface);
}

TaskflowInterfaceStandardItem::TaskflowInterfaceStandardItem(
    const QString& text,
    const tesseract_planning::TaskflowInterface& taskflow_interface)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(taskflow_interface);
}

TaskflowInterfaceStandardItem::TaskflowInterfaceStandardItem(
    const QIcon& icon,
    const QString& text,
    const tesseract_planning::TaskflowInterface& taskflow_interface)
  : QStandardItem(icon, text)
{
  ctor(taskflow_interface);
}

int TaskflowInterfaceStandardItem::type() const { return static_cast<int>(StandardItemType::MP_TASKFLOW_INTERFACE); }

void TaskflowInterfaceStandardItem::ctor(const tesseract_planning::TaskflowInterface& taskflow_interface)
{
  appendRow(createStandardItemBool("successful", taskflow_interface.isSuccessful()));
  appendRow(createStandardItemBool("aborted", taskflow_interface.isAborted()));
  appendRow(new TaskInfoMapStandardItem("task_infos", taskflow_interface.getTaskInfoMap()));
}
}  // namespace tesseract_gui
