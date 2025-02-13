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
#include <tesseract_qt/planning/models/task_composer_node_info_map_standard_item.h>
#include <tesseract_qt/planning/models/task_composer_node_info_standard_item.h>

#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/core/task_composer_node_info.h>

#include <boost/uuid/uuid_io.hpp>

namespace tesseract_gui
{
TaskComposerNodeInfoMapStandardItem::TaskComposerNodeInfoMapStandardItem(
    const std::map<boost::uuids::uuid, tesseract_planning::TaskComposerNodeInfo>& info_map)
  : QStandardItem(icons::getUnknownIcon(), "Task Composer Node Info Map")
{
  ctor(info_map);
}

TaskComposerNodeInfoMapStandardItem::TaskComposerNodeInfoMapStandardItem(
    const QString& text,
    const std::map<boost::uuids::uuid, tesseract_planning::TaskComposerNodeInfo>& info_map)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(info_map);
}

TaskComposerNodeInfoMapStandardItem::TaskComposerNodeInfoMapStandardItem(
    const QIcon& icon,
    const QString& text,
    const std::map<boost::uuids::uuid, tesseract_planning::TaskComposerNodeInfo>& info_map)
  : QStandardItem(icon, text)
{
  ctor(info_map);
}

int TaskComposerNodeInfoMapStandardItem::type() const
{
  return static_cast<int>(StandardItemType::MP_TASK_COMPOSER_NODE_INFO_MAP);
}

void TaskComposerNodeInfoMapStandardItem::ctor(
    const std::map<boost::uuids::uuid, tesseract_planning::TaskComposerNodeInfo>& info_map)
{
  for (const auto& pair : info_map)
  {
    auto* item_desc = new QStandardItem(pair.second.name.c_str());
    QString item_text = QString::fromStdString(boost::uuids::to_string(pair.first));
    appendRow({ new TaskComposerNodeInfoStandardItem(item_text, pair.second), item_desc });
  }
}
}  // namespace tesseract_gui
