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
#include <tesseract_qt/planning/task_composer_input_standard_item.h>
#include <tesseract_qt/planning/task_composer_problem_standard_item.h>
#include <tesseract_qt/planning/task_composer_node_info_map_standard_item.h>
#include <tesseract_qt/planning/task_composer_data_storage_standard_item.h>

#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/core/task_composer_input.h>

namespace tesseract_gui
{
TaskComposerInputStandardItem::TaskComposerInputStandardItem(const tesseract_planning::TaskComposerInput& input)
  : QStandardItem(icons::getUnknownIcon(), "Task Composer Input")
{
  ctor(input);
}

TaskComposerInputStandardItem::TaskComposerInputStandardItem(const QString& text,
                                                             const tesseract_planning::TaskComposerInput& input)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(input);
}

TaskComposerInputStandardItem::TaskComposerInputStandardItem(const QIcon& icon,
                                                             const QString& text,
                                                             const tesseract_planning::TaskComposerInput& input)
  : QStandardItem(icon, text)
{
  ctor(input);
}

int TaskComposerInputStandardItem::type() const { return static_cast<int>(StandardItemType::MP_TASK_COMPOSER_INPUT); }

void TaskComposerInputStandardItem::ctor(const tesseract_planning::TaskComposerInput& input)
{
  appendRow(new TaskComposerProblemStandardItem("problem", *input.problem));
  appendRow(new TaskComposerDataStorageStandardItem("data_storage", input.data_storage));
  /** @todo Add profiles */
  appendRow(createStandardItemBool("successful", input.isSuccessful()));
  appendRow(createStandardItemBool("aborted", input.isAborted()));
  appendRow(new TaskComposerNodeInfoMapStandardItem("node_infos", input.task_infos.getInfoMap()));
}
}  // namespace tesseract_gui
