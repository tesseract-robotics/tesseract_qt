/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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

#include <tesseract_qt/planning/task_composer_node_info_standard_item_factory.h>
#include <tesseract_qt/planning/task_composer_node_info_standard_item.h>
#include <tesseract_qt/planning/continuous_contact_check_task_info_standard_item.h>
#include <tesseract_qt/planning/discrete_contact_check_task_info_standard_item.h>
#include <tesseract_qt/planning/fix_state_collision_task_info_standard_item.h>
#include <tesseract_qt/planning/motion_planner_task_info_standard_item.h>
#include <tesseract_qt/planning/time_optimal_parameterization_task_info_standard_item.h>

#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_task_composer/core/task_composer_node_info.h>
#include <tesseract_task_composer/planning/nodes/continuous_contact_check_task.h>
#include <tesseract_task_composer/planning/nodes/discrete_contact_check_task.h>
#include <tesseract_task_composer/planning/nodes/fix_state_collision_task.h>
#include <tesseract_task_composer/planning/nodes/motion_planner_task_info.h>
#include <tesseract_task_composer/planning/nodes/time_optimal_parameterization_task.h>

namespace tesseract_gui
{
TaskComposerNodeInfoStandardItemManager::TaskComposerNodeInfoStandardItemManager()
{
  {  // tesseract_planning::TaskComposerNodeInfo
    using Type = tesseract_planning::TaskComposerNodeInfo;
    auto fn = [](const tesseract_planning::TaskComposerNodeInfo& node_info) -> QList<QStandardItem*> {
      return { new TaskComposerNodeInfoStandardItem(node_info) };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // tesseract_planning::ContinuousContactCheckTaskInfo
    using Type = tesseract_planning::ContinuousContactCheckTaskInfo;
    auto fn = [](const tesseract_planning::TaskComposerNodeInfo& node_info) -> QList<QStandardItem*> {
      return { new ContinuousContactCheckTaskInfoStandardItem(static_cast<const Type&>(node_info)) };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // tesseract_planning::DiscreteContactCheckTaskInfo
    using Type = tesseract_planning::DiscreteContactCheckTaskInfo;
    auto fn = [](const tesseract_planning::TaskComposerNodeInfo& node_info) -> QList<QStandardItem*> {
      return { new DiscreteContactCheckTaskInfoStandardItem(static_cast<const Type&>(node_info)) };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // tesseract_planning::FixStateCollisionTaskInfo
    using Type = tesseract_planning::FixStateCollisionTaskInfo;
    auto fn = [](const tesseract_planning::TaskComposerNodeInfo& node_info) -> QList<QStandardItem*> {
      return { new FixStateCollisionTaskInfoStandardItem(static_cast<const Type&>(node_info)) };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // tesseract_planning::MotionPlannerTaskInfo
    using Type = tesseract_planning::MotionPlannerTaskInfo;
    auto fn = [](const tesseract_planning::TaskComposerNodeInfo& node_info) -> QList<QStandardItem*> {
      return { new MotionPlannerTaskInfoStandardItem(static_cast<const Type&>(node_info)) };
    };
    registerFactoryFnHelper<Type>(fn);
  }

  {  // tesseract_planning::TimeOptimalParameterizationTaskInfo
    using Type = tesseract_planning::TimeOptimalParameterizationTaskInfo;
    auto fn = [](const tesseract_planning::TaskComposerNodeInfo& node_info) -> QList<QStandardItem*> {
      return { new TimeOptimalParameterizationTaskInfoStandardItem(static_cast<const Type&>(node_info)) };
    };
    registerFactoryFnHelper<Type>(fn);
  }
}
std::shared_ptr<TaskComposerNodeInfoStandardItemManager> TaskComposerNodeInfoStandardItemManager::singleton =
    nullptr;                                                                 // NOLINT
std::once_flag TaskComposerNodeInfoStandardItemManager::init_instance_flag;  // NOLINT
void TaskComposerNodeInfoStandardItemManager::initSingleton()
{
  singleton = std::make_shared<TaskComposerNodeInfoStandardItemManager>();
}

std::shared_ptr<TaskComposerNodeInfoStandardItemManager> TaskComposerNodeInfoStandardItemManager::instance()
{
  std::call_once(init_instance_flag, &TaskComposerNodeInfoStandardItemManager::initSingleton);
  return singleton;
}

QList<QStandardItem*>
TaskComposerNodeInfoStandardItemManager::create(const tesseract_planning::TaskComposerNodeInfo& node_info)
{
  return instance()->createHelper(node_info);
}

QList<QStandardItem*>
TaskComposerNodeInfoStandardItemManager::createHelper(const tesseract_planning::TaskComposerNodeInfo& node_info)
{
  std::shared_lock lock(mutex_);
  auto it = factories_.find(std::type_index(typeid(node_info)));
  if (it == factories_.end())
    return {};

  return it->second(node_info);
}

void TaskComposerNodeInfoStandardItemManager::registerFactoryHelper(
    const TaskComposerNodeInfoStandardItemFactory& factory)
{
  std::unique_lock lock(mutex_);
  auto fm = factory.create();
  factories_.insert(fm.begin(), fm.end());
}

}  // namespace tesseract_gui
