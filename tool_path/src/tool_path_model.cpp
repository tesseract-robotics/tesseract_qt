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
#include <tesseract_qt/tool_path/tool_path_utils.h>
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/tool_path_standard_item.h>
#include <tesseract_qt/common/tool_path_segment_standard_item.h>
#include <tesseract_qt/common/namespace_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/utils.h>
#include <boost/uuid/uuid_io.hpp>

#include <QApplication>

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
  tool_paths_.clear();
}

void ToolPathModel::addToolPath(const ToolPath& tool_path)
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

  if (tool_paths_.empty())
    clear();
}

bool ToolPathModel::hasToolPath(const boost::uuids::uuid& uuid)
{
  return (tool_paths_.find(uuid) != tool_paths_.end());
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

ToolPathSegment ToolPathModel::getToolPathSegment(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);

  if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH))
    throw std::runtime_error("Cannot get transfrom from selected tool path standard item");

  return findToolPathSegmentItem(item)->getToolPathSegment();
}

ToolPath ToolPathModel::getToolPath(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  return findToolPathItem(item)->getToolPath();
}

bool ToolPathModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // Need emit application event to change visible
  if (role == Qt::CheckStateRole)
  {
    QStandardItem* item = itemFromIndex(index);
    if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH))
    {
      assert(dynamic_cast<ToolPathStandardItem*>(item) != nullptr);
      auto* derived_item = static_cast<ToolPathStandardItem*>(item);
      if (value.value<Qt::CheckState>() == Qt::Checked)
        QApplication::sendEvent(qApp, new events::ToolPathShow(scene_name_, derived_item->getUUID()));
      else
        QApplication::sendEvent(qApp, new events::ToolPathHide(scene_name_, derived_item->getUUID()));
    }
    else if (item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH_SEGMENT))
    {
      assert(dynamic_cast<ToolPathSegmentStandardItem*>(item) != nullptr);
      auto* derived_item = static_cast<ToolPathSegmentStandardItem*>(item);
      if (value.value<Qt::CheckState>() == Qt::Checked)
        QApplication::sendEvent(
            qApp, new events::ToolPathShow(scene_name_, findToolPathItem(item)->getUUID(), derived_item->getUUID()));
      else
        QApplication::sendEvent(
            qApp, new events::ToolPathHide(scene_name_, findToolPathItem(item)->getUUID(), derived_item->getUUID()));
    }
    else if (item->type() == static_cast<int>(StandardItemType::COMMON_TRANSFORM))
    {
      assert(dynamic_cast<TransformStandardItem*>(item) != nullptr);
      auto* derived_item = static_cast<TransformStandardItem*>(item);
      if (value.value<Qt::CheckState>() == Qt::Checked)
        QApplication::sendEvent(
            qApp, new events::ToolPathShow(scene_name_, findToolPathItem(item)->getUUID(), derived_item->getUUID()));
      else
        QApplication::sendEvent(
            qApp, new events::ToolPathHide(scene_name_, findToolPathItem(item)->getUUID(), derived_item->getUUID()));
    }
  }
  return QStandardItemModel::setData(index, value, role);
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
  else if (event->type() == events::ToolPathRemoveAll::kType)
  {
    assert(dynamic_cast<events::ToolPathRemoveAll*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathRemoveAll*>(event);
    if (e->getSceneName() == scene_name_)
      clear();
  }
  else if (event->type() == events::ToolPathHideAll::kType)
  {
    assert(dynamic_cast<events::ToolPathHideAll*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathHideAll*>(event);
    if (e->getSceneName() == scene_name_)
      setCheckedStateRecursive(invisibleRootItem(), Qt::CheckState::Unchecked);
  }
  else if (event->type() == events::ToolPathShowAll::kType)
  {
    assert(dynamic_cast<events::ToolPathShowAll*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathShowAll*>(event);
    if (e->getSceneName() == scene_name_)
      setCheckedStateRecursive(invisibleRootItem(), Qt::CheckState::Checked);
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
