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
#include <tesseract_qt/planning/time_optimal_parameterization_task_info_standard_item.h>

#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_qt/environment/models/environment_standard_item.h>

#include <tesseract_task_composer/planning/nodes/time_optimal_parameterization_task.h>

namespace tesseract_gui
{
TimeOptimalParameterizationTaskInfoStandardItem::TimeOptimalParameterizationTaskInfoStandardItem(
    const tesseract_planning::TimeOptimalParameterizationTaskInfo& info)
  : TaskComposerNodeInfoStandardItem(icons::getUnknownIcon(), "TOTG Task Info", info)
{
  ctor(info);
}

TimeOptimalParameterizationTaskInfoStandardItem::TimeOptimalParameterizationTaskInfoStandardItem(
    const QString& text,
    const tesseract_planning::TimeOptimalParameterizationTaskInfo& info)
  : TaskComposerNodeInfoStandardItem(icons::getUnknownIcon(), text, info)
{
  ctor(info);
}

TimeOptimalParameterizationTaskInfoStandardItem::TimeOptimalParameterizationTaskInfoStandardItem(
    const QIcon& icon,
    const QString& text,
    const tesseract_planning::TimeOptimalParameterizationTaskInfo& info)
  : TaskComposerNodeInfoStandardItem(icon, text, info)
{
  ctor(info);
}

int TimeOptimalParameterizationTaskInfoStandardItem::type() const
{
  return static_cast<int>(StandardItemType::MP_TOTG_TAKS_INFO);
}

void TimeOptimalParameterizationTaskInfoStandardItem::ctor(
    const tesseract_planning::TimeOptimalParameterizationTaskInfo& info)
{
  TaskComposerNodeInfoStandardItem::ctor(info);
  appendRow(createStandardItemFloat("max_velocity_scaling_factor", info.max_velocity_scaling_factor));
  appendRow(createStandardItemFloat("max_acceleration_scaling_factor", info.max_acceleration_scaling_factor));
}
}  // namespace tesseract_gui
