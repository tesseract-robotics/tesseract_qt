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
#include <tesseract_qt/planning/process_planning_future_standard_item.h>
#include <tesseract_qt/planning/process_planning_problem_standard_item.h>
#include <tesseract_qt/planning/taskflow_interface_standard_item.h>

#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_process_managers/core/process_planning_future.h>

namespace tesseract_gui
{
ProcessPlanningFutureStandardItem::ProcessPlanningFutureStandardItem(
    const tesseract_planning::ProcessPlanningFuture& process_future)
  : QStandardItem(icons::getUnknownIcon(), "Process Planning Future")
{
  ctor(process_future);
}

ProcessPlanningFutureStandardItem::ProcessPlanningFutureStandardItem(
    const QString& text,
    const tesseract_planning::ProcessPlanningFuture& process_future)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(process_future);
}

ProcessPlanningFutureStandardItem::ProcessPlanningFutureStandardItem(
    const QIcon& icon,
    const QString& text,
    const tesseract_planning::ProcessPlanningFuture& process_future)
  : QStandardItem(icon, text)
{
  ctor(process_future);
}

int ProcessPlanningFutureStandardItem::type() const
{
  return static_cast<int>(StandardItemType::MP_PROCESS_PLANNING_FUTURE);
}

void ProcessPlanningFutureStandardItem::ctor(const tesseract_planning::ProcessPlanningFuture& process_future)
{
  if (process_future.interface != nullptr)
    appendRow(new TaskflowInterfaceStandardItem("interface", *process_future.interface));
  else
    appendRow({ new QStandardItem("interface"), new QStandardItem("NULL") });

  if (process_future.problem != nullptr && process_future.problem->input != nullptr)
    appendRow(new ProcessPlanningProblemStandardItem("problem", *process_future.problem));
  else
    appendRow({ new QStandardItem("problem"), new QStandardItem("NULL") });
}
}  // namespace tesseract_gui
