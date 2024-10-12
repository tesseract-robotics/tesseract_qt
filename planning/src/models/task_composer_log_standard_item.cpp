/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2024 Levi Armstrong <levi.armstrong@gmail.com>
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

#include <tesseract_qt/planning/models/task_composer_log_standard_item.h>
#include <tesseract_qt/planning/models/task_composer_context_standard_item.h>
#include <tesseract_qt/planning/models/task_composer_data_storage_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/core/task_composer_log.h>

namespace tesseract_gui
{
TaskComposerLogStandardItem::TaskComposerLogStandardItem(const tesseract_planning::TaskComposerLog& data)
  : QStandardItem(icons::getLogIcon(), "Task Composer Log")
{
  ctor(data);
}

TaskComposerLogStandardItem::TaskComposerLogStandardItem(const QString& text,
                                                         const tesseract_planning::TaskComposerLog& data)
  : QStandardItem(icons::getLogIcon(), text)
{
  ctor(data);
}

TaskComposerLogStandardItem::TaskComposerLogStandardItem(const QIcon& icon,
                                                         const QString& text,
                                                         const tesseract_planning::TaskComposerLog& data)
  : QStandardItem(icon, text)
{
  ctor(data);
}

int TaskComposerLogStandardItem::type() const { return static_cast<int>(StandardItemType::MP_TASK_COMPOSER_LOG); }

void TaskComposerLogStandardItem::ctor(const tesseract_planning::TaskComposerLog& data)
{
  appendRow(createStandardItemString("description", data.description));
  appendRow(new TaskComposerDataStorageStandardItem("initial_data", data.initial_data));  // NOLINT
  appendRow(new TaskComposerContextStandardItem("context", *data.context));               // NOLINT
}
}  // namespace tesseract_gui
