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
#include <tesseract_qt/tool_path/models/tool_path_selection_model.h>
#include <tesseract_qt/tool_path/models/tool_path_model.h>
#include <tesseract_qt/tool_path/models/tool_path_utils.h>
#include <tesseract_qt/common/events/tool_path_events.h>
#include <tesseract_qt/common/models/tool_path_standard_item.h>
#include <tesseract_qt/common/models/tool_path_segment_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/component_info.h>

#include <QGuiApplication>

namespace tesseract_gui
{
ToolPathSelectionModel::ToolPathSelectionModel() : component_info_(nullptr)
{
  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

ToolPathSelectionModel::ToolPathSelectionModel(QAbstractItemModel* model)
  : QItemSelectionModel(model), component_info_(nullptr)
{
  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

ToolPathSelectionModel::ToolPathSelectionModel(std::shared_ptr<const ComponentInfo> component_info)
  : component_info_(std::move(component_info))
{
  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

ToolPathSelectionModel::ToolPathSelectionModel(QAbstractItemModel* model,
                                               std::shared_ptr<const ComponentInfo> component_info)
  : QItemSelectionModel(model), component_info_(std::move(component_info))
{
  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

ToolPathSelectionModel::~ToolPathSelectionModel() = default;

bool ToolPathSelectionModel::eventFilter(QObject* obj, QEvent* event)
{
  if (!hasSelection() || model() == nullptr)
    return QObject::eventFilter(obj, event);

  QModelIndexList selected_rows = selectedRows();
  auto* tool_path_model = dynamic_cast<ToolPathModel*>(model());
  if (event->type() == events::EventType::TOOL_PATH_REMOVE_SELECTED)
  {
    assert(dynamic_cast<events::ToolPathRemoveSelected*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathRemoveSelected*>(event);
    if (e->getComponentInfo() == component_info_)
    {
      if (selected_rows.size() == 1)
      {
        QStandardItem* item = tool_path_model->itemFromIndex(selected_rows.front());
        if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH))
        {
          assert(dynamic_cast<ToolPathStandardItem*>(item) != nullptr);
          auto* derived_item = static_cast<ToolPathStandardItem*>(item);
          events::ToolPathRemove event(component_info_, derived_item->getUUID());
          QGuiApplication::sendEvent(qApp, &event);
        }
        else if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH_SEGMENT))
        {
          /** @todo Levi, update to support segments */
        }
      }
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
