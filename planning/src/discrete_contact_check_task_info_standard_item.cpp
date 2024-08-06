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
#include <tesseract_qt/planning/discrete_contact_check_task_info_standard_item.h>

#include <tesseract_qt/common/contact_results_types.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_qt/collision/models/contact_result_map_vector_standard_item.h>
#include <tesseract_qt/environment/models/environment_standard_item.h>

#include <tesseract_task_composer/planning/nodes/discrete_contact_check_task.h>

namespace tesseract_gui
{
DiscreteContactCheckTaskInfoStandardItem::DiscreteContactCheckTaskInfoStandardItem(
    const tesseract_planning::DiscreteContactCheckTaskInfo& info)
  : TaskComposerNodeInfoStandardItem(icons::getUnknownIcon(), "Discrete Contact Check Task Info", info)
{
  ctor(info);
}

DiscreteContactCheckTaskInfoStandardItem::DiscreteContactCheckTaskInfoStandardItem(
    const QString& text,
    const tesseract_planning::DiscreteContactCheckTaskInfo& info)
  : TaskComposerNodeInfoStandardItem(icons::getUnknownIcon(), text, info)
{
  ctor(info);
}

DiscreteContactCheckTaskInfoStandardItem::DiscreteContactCheckTaskInfoStandardItem(
    const QIcon& icon,
    const QString& text,
    const tesseract_planning::DiscreteContactCheckTaskInfo& info)
  : TaskComposerNodeInfoStandardItem(icon, text, info)
{
  ctor(info);
}

int DiscreteContactCheckTaskInfoStandardItem::type() const
{
  return static_cast<int>(StandardItemType::MP_DISCRETE_CONTACT_CHECK_TASK_INFO);
}

void DiscreteContactCheckTaskInfoStandardItem::ctor(const tesseract_planning::DiscreteContactCheckTaskInfo& info)
{
  TaskComposerNodeInfoStandardItem::ctor(info);

  if (info.env != nullptr)
    appendRow(new EnvironmentStandardItem("env", *info.env));  // NOLINT
  else
    appendRow(createStandardItemString(icons::getTesseractIcon(), "env", "nullptr"));

  appendRow(new ContactResultMapVectorStandardItem("contact_results", convert(info.contact_results)));  // NOLINT
}
}  // namespace tesseract_gui
