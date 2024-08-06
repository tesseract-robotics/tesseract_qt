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
#ifndef TESSERACT_QT_PLANNING_TASK_COMPOSER_NODE_INFO_STANDARD_ITEM_FACTORY_H
#define TESSERACT_QT_PLANNING_TASK_COMPOSER_NODE_INFO_STANDARD_ITEM_FACTORY_H

#include <tesseract_task_composer/core/fwd.h>

#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <typeindex>

#include <QStandardItem>

namespace tesseract_gui
{
using TaskComposerNodeInfoStandardItemFactoryFn =
    std::function<QList<QStandardItem*>(const tesseract_planning::TaskComposerNodeInfo& node_info)>;

class TaskComposerNodeInfoStandardItemFactory
{
public:
  virtual ~TaskComposerNodeInfoStandardItemFactory() = default;
  virtual std::map<std::type_index, TaskComposerNodeInfoStandardItemFactoryFn> create() const = 0;
};

class TaskComposerNodeInfoStandardItemManager
{
public:
  TaskComposerNodeInfoStandardItemManager();
  ~TaskComposerNodeInfoStandardItemManager() = default;
  TaskComposerNodeInfoStandardItemManager(const TaskComposerNodeInfoStandardItemManager&) = delete;
  TaskComposerNodeInfoStandardItemManager& operator=(const TaskComposerNodeInfoStandardItemManager&) = delete;
  TaskComposerNodeInfoStandardItemManager(TaskComposerNodeInfoStandardItemManager&&) = delete;
  TaskComposerNodeInfoStandardItemManager& operator=(TaskComposerNodeInfoStandardItemManager&&) = delete;

  static QList<QStandardItem*> create(const tesseract_planning::TaskComposerNodeInfo& node_info);

  static void registerFactory(const TaskComposerNodeInfoStandardItemFactory& factory);

  template <typename NodeInfoType>
  static void registerFactory(const TaskComposerNodeInfoStandardItemFactoryFn& factory)
  {
    auto manager = instance();
    manager->registerFactoryFnHelper<NodeInfoType>(factory);
  }

private:
  std::shared_mutex mutex_;
  std::map<std::type_index, TaskComposerNodeInfoStandardItemFactoryFn> factories_;

  static std::shared_ptr<TaskComposerNodeInfoStandardItemManager> singleton;
  static std::once_flag init_instance_flag;
  static void initSingleton();
  static std::shared_ptr<TaskComposerNodeInfoStandardItemManager> instance();

  QList<QStandardItem*> createHelper(const tesseract_planning::TaskComposerNodeInfo& node_info);

  void registerFactoryHelper(const TaskComposerNodeInfoStandardItemFactory& factory);

  template <typename NodeInfoType>
  void registerFactoryFnHelper(const TaskComposerNodeInfoStandardItemFactoryFn& factory)
  {
    std::unique_lock lock(mutex_);
    factories_[std::type_index(typeid(NodeInfoType))] = factory;
  }
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_PLANNING_TASK_COMPOSER_NODE_INFO_STANDARD_ITEM_FACTORY_H
