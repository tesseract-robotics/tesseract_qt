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
#include <tesseract_qt/planning/task_composer_problem_model.h>
#include <tesseract_qt/planning/task_composer_problem_standard_item.h>
#include <tesseract_qt/planning/task_composer_data_storage_standard_item.h>
#include <tesseract_qt/common/models/namespace_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/utils.h>

#include <QUuid>

namespace tesseract_gui
{
struct TaskComposerProblemModelPrivate
{
  std::map<QString, QStandardItem*> items;
  std::map<QStandardItem*, TaskComposerProblemModel::ProblemData> problems;
  std::map<QStandardItem*, QString> problems_ns;
  void clear()
  {
    items.clear();
    problems.clear();
    problems_ns.clear();
  }
};

TaskComposerProblemModel::TaskComposerProblemModel(QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<TaskComposerProblemModelPrivate>())
{
  clear();
}

TaskComposerProblemModel::~TaskComposerProblemModel() = default;

void TaskComposerProblemModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  data_->clear();
}

QString TaskComposerProblemModel::addProblem(ProblemData data, std::string ns)
{
  QString key = QUuid::createUuid().toString();
  ns = (ns.empty()) ? "general" : ns;
  NamespaceStandardItem* item = createNamespaceItem(*invisibleRootItem(), ns);

  auto* problem_data_storage = new TaskComposerDataStorageStandardItem(key, *data.data_storage);
  auto* problem_item = new TaskComposerProblemStandardItem(key, *data.problem);
  problem_item->appendRow({ new QStandardItem("data_storage"), problem_data_storage });

  auto* problem_description_item = new QStandardItem(QString::fromStdString(data.problem->name));
  item->appendRow({ problem_item, problem_description_item });
  data_->items[key] = problem_item;
  data_->problems[problem_item] = std::move(data);
  data_->problems_ns[problem_item] = ns.c_str();
  return key;
}

void TaskComposerProblemModel::removeProblem(const QString& key)
{
  auto it = data_->items.find(key);
  if (it == data_->items.end())
    throw std::runtime_error("Tried to remove problem '" + key.toStdString() + "' which does not exist!");

  data_->problems.erase(it->second);
  data_->problems_ns.erase(it->second);
  QModelIndex idx = indexFromItem(it->second);
  data_->items.erase(it);
  removeRow(idx.row(), idx.parent());
}

bool TaskComposerProblemModel::hasProblem(const QString& key) { return (data_->items.find(key) != data_->items.end()); }

TaskComposerProblemStandardItem* findTaskComposerProblemItem(QStandardItem* item)
{
  if (item == nullptr)
    return nullptr;

  if (item->type() == static_cast<int>(StandardItemType::MP_TASK_COMPOSER_PROBLEM))
    return dynamic_cast<TaskComposerProblemStandardItem*>(item);

  return findTaskComposerProblemItem(item->parent());
}

const TaskComposerProblemModel::ProblemData& TaskComposerProblemModel::getProblem(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  return data_->problems.at(findTaskComposerProblemItem(item));
}

const QString& TaskComposerProblemModel::getProblemNamespace(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  return data_->problems_ns.at(findTaskComposerProblemItem(item));
}

}  // namespace tesseract_gui
