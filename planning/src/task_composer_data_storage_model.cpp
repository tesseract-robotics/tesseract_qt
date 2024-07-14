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
#include <tesseract_qt/planning/task_composer_data_storage_model.h>
#include <tesseract_qt/planning/task_composer_data_storage_standard_item.h>
#include <tesseract_qt/common/models/namespace_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/utils.h>

#include <tesseract_task_composer/core/task_composer_data_storage.h>

#include <QUuid>

namespace tesseract_gui
{
struct TaskComposerDataStorageModel::Implementation
{
  std::map<QString, QStandardItem*> items;
  std::map<QStandardItem*, std::unique_ptr<tesseract_planning::TaskComposerDataStorage>> data_storages;
  std::map<QStandardItem*, QString> data_storages_ns;
  void clear()
  {
    items.clear();
    data_storages.clear();
    data_storages_ns.clear();
  }
};

TaskComposerDataStorageModel::TaskComposerDataStorageModel(QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();
}

TaskComposerDataStorageModel::~TaskComposerDataStorageModel() = default;

void TaskComposerDataStorageModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  data_->clear();
}

QString TaskComposerDataStorageModel::add(std::unique_ptr<tesseract_planning::TaskComposerDataStorage> data_storage,
                                          std::string ns)
{
  QString key = QUuid::createUuid().toString();
  ns = (ns.empty()) ? "general" : ns;
  NamespaceStandardItem* item = createNamespaceItem(*invisibleRootItem(), ns);

  auto* data_storage_item = new TaskComposerDataStorageStandardItem(key, *data_storage);
  item->appendRow(data_storage_item);
  data_->items[key] = data_storage_item;
  data_->data_storages[data_storage_item] = std::move(data_storage);
  data_->data_storages_ns[data_storage_item] = ns.c_str();
  return key;
}

void TaskComposerDataStorageModel::remove(const QString& key)
{
  auto it = data_->items.find(key);
  if (it == data_->items.end())
    throw std::runtime_error("Tried to remove data storage '" + key.toStdString() + "' which does not exist!");

  data_->data_storages.erase(it->second);
  data_->data_storages.erase(it->second);
  QModelIndex idx = indexFromItem(it->second);
  data_->items.erase(it);
  removeRow(idx.row(), idx.parent());
}

bool TaskComposerDataStorageModel::has(const QString& key) { return (data_->items.find(key) != data_->items.end()); }

TaskComposerDataStorageStandardItem* findTaskComposerDataStorageItem(QStandardItem* item)
{
  if (item == nullptr)
    return nullptr;

  if (item->type() == static_cast<int>(StandardItemType::MP_TASK_COMPOSER_DATA_STORAGE))
    return dynamic_cast<TaskComposerDataStorageStandardItem*>(item);

  return findTaskComposerDataStorageItem(item->parent());
}

const tesseract_planning::TaskComposerDataStorage& TaskComposerDataStorageModel::get(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  return *data_->data_storages.at(findTaskComposerDataStorageItem(item));
}

const QString& TaskComposerDataStorageModel::getNamespace(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  return data_->data_storages_ns.at(findTaskComposerDataStorageItem(item));
}

}  // namespace tesseract_gui
