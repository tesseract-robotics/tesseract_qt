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
#include <tesseract_qt/tool_path/tool_path_model.h>
#include <tesseract_qt/tool_path/tool_path_events.h>
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/tool_path_standard_item.h>
#include <tesseract_qt/common/tool_path_segment_standard_item.h>
#include <tesseract_qt/common/namespace_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/utils.h>
#include <boost/uuid/uuid_io.hpp>

#include <QGuiApplication>

namespace tesseract_gui
{
ToolPathModel::ToolPathModel(std::string scene_name, QObject* parent)
  : QStandardItemModel(parent), scene_name_(std::move(scene_name))
{
  clear();

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

void ToolPathModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
}

void ToolPathModel::addToolPath(const tesseract_common::ToolPath& tool_path)
{
  std::string ns = (tool_path.getNamespace().empty()) ? "general" : tool_path.getNamespace();
  NamespaceStandardItem* item = createNamespaceItem(*invisibleRootItem(), ns);

  auto* tool_path_item = new ToolPathStandardItem(tool_path);
  auto* tool_path_description_item = new QStandardItem(QString::fromStdString(tool_path.getDescription()));
  item->appendRow({ tool_path_item, tool_path_description_item });
  tool_paths_[tool_path.getUUID()] = tool_path_item;
}

void ToolPathModel::removeToolPath(const boost::uuids::uuid& uuid)
{
  auto it = tool_paths_.find(uuid);
  if (it == tool_paths_.end())
    throw std::runtime_error("Tried to remove tool path '" + boost::uuids::to_string(uuid) + "' which does not exist!");

  QModelIndex idx = indexFromItem(it->second);
  tool_paths_.erase(it);
  removeRow(idx.row(), idx.parent());
}

bool ToolPathModel::hasToolPath(const boost::uuids::uuid& uuid)
{
  return (tool_paths_.find(uuid) != tool_paths_.end());
}

TransformStandardItem* findTransformItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::COMMON_TRANSFORM))
    return dynamic_cast<TransformStandardItem*>(item);

  return findTransformItem(item->parent());
}

Eigen::Isometry3d ToolPathModel::getTransform(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);

  if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH_SEGMENT))
    throw std::runtime_error("Cannot get transfrom from selected tool path segment standard item");

  if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH))
    throw std::runtime_error("Cannot get transfrom from selected tool path standard item");

  return findTransformItem(item)->getTransfrom();
}

ToolPathSegmentStandardItem* findToolPathSegmentItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH_SEGMENT))
    return dynamic_cast<ToolPathSegmentStandardItem*>(item);

  return findToolPathSegmentItem(item->parent());
}

tesseract_common::ToolPathSegment ToolPathModel::getToolPathSegment(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);

  if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH))
    throw std::runtime_error("Cannot get transfrom from selected tool path standard item");

  return findToolPathSegmentItem(item)->getToolPathSegment();
}

ToolPathStandardItem* findToolPathItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH))
    return dynamic_cast<ToolPathStandardItem*>(item);

  return findToolPathItem(item->parent());
}

tesseract_common::ToolPath ToolPathModel::getToolPath(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  return findToolPathItem(item)->getToolPath();
}

bool ToolPathModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::ToolPathAdd::kType)
  {
    assert(dynamic_cast<events::ToolPathAdd*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathAdd*>(event);
    if (e->getSceneName() == scene_name_)
      addToolPath(e->getToolPath());
  }
  else if (event->type() == events::ToolPathRemove::kType)
  {
    assert(dynamic_cast<events::ToolPathRemove*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathRemove*>(event);
    if (e->getSceneName() == scene_name_)
      removeToolPath(e->getUUID());
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
