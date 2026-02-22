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

#include <tesseract_qt/planning/models/task_composer_keys_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/core/task_composer_keys.h>

namespace tesseract::gui
{
TaskComposerKeysStandardItem::TaskComposerKeysStandardItem(const tesseract::task_composer::TaskComposerKeys& data)
  : QStandardItem(icons::getDataIcon(), "Task Composer Keys")
{
  ctor(data);
}

TaskComposerKeysStandardItem::TaskComposerKeysStandardItem(const QString& text,
                                                           const tesseract::task_composer::TaskComposerKeys& data)
  : QStandardItem(icons::getDataIcon(), text)
{
  ctor(data);
}

TaskComposerKeysStandardItem::TaskComposerKeysStandardItem(const QIcon& icon,
                                                           const QString& text,
                                                           const tesseract::task_composer::TaskComposerKeys& data)
  : QStandardItem(icon, text)
{
  ctor(data);
}

int TaskComposerKeysStandardItem::type() const { return static_cast<int>(StandardItemType::MP_TASK_COMPOSER_KEYS); }

void TaskComposerKeysStandardItem::ctor(const tesseract::task_composer::TaskComposerKeys& data)
{
  for (const auto& pair : data.data())
  {
    if (pair.second.index() == 0)
    {
      appendRow(createStandardItemString(pair.first, std::get<std::string>(pair.second)));
    }
    else
    {
      auto* item = new QStandardItem(icons::getArrayIcon(), QString::fromStdString(pair.first));
      const auto& keys = std::get<std::vector<std::string>>(pair.second);
      for (std::size_t i = 0; i < keys.size(); ++i)
        item->appendRow(createStandardItemString(QString("[%1]").arg(i).toStdString(), (keys[i])));

      appendRow(item);
    }
  }
}
}  // namespace tesseract::gui
