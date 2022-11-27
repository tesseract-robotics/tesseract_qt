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
#include <tesseract_qt/tool_path/tool_path_selection_model.h>
#include <tesseract_qt/tool_path/tool_path_model.h>
#include <tesseract_qt/tool_path/tool_path_events.h>
#include <tesseract_qt/tool_path/tool_path_utils.h>

#include <QGuiApplication>

namespace tesseract_gui
{
ToolPathSelectionModel::ToolPathSelectionModel(std::string scene_name) : scene_name_(std::move(scene_name))
{
  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

ToolPathSelectionModel::ToolPathSelectionModel(QAbstractItemModel* model, std::string scene_name)
  : QItemSelectionModel(model), scene_name_(std::move(scene_name))
{
  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

bool ToolPathSelectionModel::eventFilter(QObject* obj, QEvent* event)
{
  if (!hasSelection() || model() == nullptr)
    return QObject::eventFilter(obj, event);

  QModelIndexList selected_rows = selectedRows();
  auto* tool_path_model = dynamic_cast<ToolPathModel*>(model());
  if (event->type() == events::ToolPathRemoveSelected::kType)
  {
    assert(dynamic_cast<events::ToolPathRemoveSelected*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathRemoveSelected*>(event);
    if (e->getSceneName() == scene_name_)
    {
      /** @todo Need to implement */
    }
  }
  else if (event->type() == events::ToolPathHideSelected::kType)
  {
    assert(dynamic_cast<events::ToolPathHideSelected*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathHideSelected*>(event);
    if (e->getSceneName() == scene_name_)
    {
      for (const auto& i : selected_rows)
        setCheckedStateRecursive(tool_path_model->itemFromIndex(i), Qt::CheckState::Unchecked);
    }
  }
  else if (event->type() == events::ToolPathShowSelected::kType)
  {
    assert(dynamic_cast<events::ToolPathShowSelected*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathShowSelected*>(event);
    if (e->getSceneName() == scene_name_)
    {
      for (const auto& i : selected_rows)
        setCheckedStateRecursive(tool_path_model->itemFromIndex(i), Qt::CheckState::Checked);
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
