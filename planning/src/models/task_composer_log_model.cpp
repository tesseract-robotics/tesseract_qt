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
#include <tesseract_qt/planning/models/task_composer_log_model.h>
#include <tesseract_qt/planning/models/task_composer_log_standard_item.h>
#include <tesseract_qt/common/models/namespace_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/utils.h>

#include <tesseract_task_composer/core/task_composer_log.h>

#include <QUuid>

namespace tesseract_gui
{
struct TaskComposerLogModel::Implementation
{
  std::map<QString, QStandardItem*> items;
  std::map<QStandardItem*, tesseract_planning::TaskComposerLog> logs;
  std::map<QStandardItem*, QString> logs_ns;
  void clear()
  {
    items.clear();
    logs.clear();
    logs_ns.clear();
  }
};

TaskComposerLogModel::TaskComposerLogModel(QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();
}

TaskComposerLogModel::~TaskComposerLogModel() = default;

void TaskComposerLogModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  data_->clear();
}

QString TaskComposerLogModel::add(tesseract_planning::TaskComposerLog log, std::string ns)
{
  QString key = QUuid::createUuid().toString();
  ns = (ns.empty()) ? "general" : ns;
  NamespaceStandardItem* item = createNamespaceItem(*invisibleRootItem(), ns);

  auto* log_item = new TaskComposerLogStandardItem(key, log);
  item->appendRow(log_item);
  data_->items[key] = log_item;
  data_->logs[log_item] = std::move(log);
  data_->logs_ns[log_item] = ns.c_str();
  return key;
}

void TaskComposerLogModel::remove(const QString& key)
{
  auto it = data_->items.find(key);
  if (it == data_->items.end())
    throw std::runtime_error("Tried to remove log '" + key.toStdString() + "' which does not exist!");

  data_->logs.erase(it->second);
  data_->logs_ns.erase(it->second);
  QModelIndex idx = indexFromItem(it->second);
  data_->items.erase(it);
  removeRow(idx.row(), idx.parent());
}

bool TaskComposerLogModel::has(const QString& key) { return (data_->items.find(key) != data_->items.end()); }

TaskComposerLogStandardItem* findTaskComposerLogItem(QStandardItem* item)
{
  if (item == nullptr)
    return nullptr;

  if (item->type() == static_cast<int>(StandardItemType::MP_TASK_COMPOSER_LOG))
    return dynamic_cast<TaskComposerLogStandardItem*>(item);

  return findTaskComposerLogItem(item->parent());
}

const tesseract_planning::TaskComposerLog& TaskComposerLogModel::get(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  return data_->logs.at(findTaskComposerLogItem(item));
}

const QString& TaskComposerLogModel::getNamespace(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  return data_->logs_ns.at(findTaskComposerLogItem(item));
}

}  // namespace tesseract_gui
