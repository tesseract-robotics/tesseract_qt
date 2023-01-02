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
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_model.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_set_item.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_info_item.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_state_item.h>

#include <tesseract_qt/common/events/joint_trajectory_events.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <tesseract_qt/common/namespace_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/utils.h>

#include <tesseract_common/joint_state.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <QApplication>

namespace tesseract_gui
{
struct JointTrajectoryModel::Implementation
{
  ComponentInfo component_info;
  std::map<boost::uuids::uuid, QStandardItem*> trajectory_sets;
};

JointTrajectoryModel::JointTrajectoryModel(QObject* parent) : JointTrajectoryModel(ComponentInfo(), parent) {}

JointTrajectoryModel::JointTrajectoryModel(ComponentInfo component_info, QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();

  data_->component_info = std::move(component_info);

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}
JointTrajectoryModel::~JointTrajectoryModel() = default;

const ComponentInfo& JointTrajectoryModel::getComponentInfo() const { return data_->component_info; }

void JointTrajectoryModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
}

void JointTrajectoryModel::addJointTrajectorySet(tesseract_common::JointTrajectorySet trajectory_set)
{
  if (trajectory_set.getEnvironment() == nullptr)
  {
    auto env_wrapper = EnvironmentManager::getDefault();
    if (env_wrapper != nullptr)
    {
      auto env = env_wrapper->getEnvironment();
      if (env != nullptr || env->isInitialized())
        trajectory_set.applyEnvironment(env->clone());
    }
  }

  boost::uuids::uuid uuid = trajectory_set.getUUID();
  std::string ns = (trajectory_set.getNamespace().empty()) ? "general" : trajectory_set.getNamespace();
  NamespaceStandardItem* item = createNamespaceItem(*invisibleRootItem(), ns);

  auto* trajectory_container_item = new JointTrajectorySetItem(trajectory_set);
  auto* trajectory_description_item = new QStandardItem(QString::fromStdString(trajectory_set.getDescription()));
  item->appendRow({ trajectory_container_item, trajectory_description_item });
  data_->trajectory_sets[uuid] = trajectory_container_item;
}

void JointTrajectoryModel::removeJointTrajectorySet(const boost::uuids::uuid& uuid)
{
  auto it = data_->trajectory_sets.find(uuid);
  if (it == data_->trajectory_sets.end())
    throw std::runtime_error("Tried to remove trajectory set '" + boost::uuids::to_string(uuid) +
                             "' which does not exist!");

  QModelIndex idx = indexFromItem(it->second);
  data_->trajectory_sets.erase(it);
  removeRow(idx.row(), idx.parent());
}

bool JointTrajectoryModel::hasJointTrajectorySet(const boost::uuids::uuid& uuid)
{
  return (data_->trajectory_sets.find(uuid) != data_->trajectory_sets.end());
}

JointTrajectoryStateItem* findJointStateItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET_STATE))
    return dynamic_cast<JointTrajectoryStateItem*>(item);

  return findJointStateItem(item->parent());
}

tesseract_common::JointState JointTrajectoryModel::getJointState(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);

  if (item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET_TRAJECTORY))
    throw std::runtime_error("Cannot get joint state from selected joint trajectory standard item");

  if (item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET))
    throw std::runtime_error("Cannot get joint state from selected joint trajectory set standard item");

  return findJointStateItem(item)->state;
}

JointTrajectoryInfoItem* findJointTrajectoryInfoItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET_TRAJECTORY))
    return dynamic_cast<JointTrajectoryInfoItem*>(item);

  return findJointTrajectoryInfoItem(item->parent());
}

tesseract_common::JointTrajectoryInfo JointTrajectoryModel::getJointTrajectory(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);

  if (item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET))
    throw std::runtime_error("Cannot get joint trajectory from selected trajectory set standard item");

  return findJointTrajectoryInfoItem(item)->trajectory_info;
}

JointTrajectorySetItem* findJointTrajectorySetItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET))
    return dynamic_cast<JointTrajectorySetItem*>(item);

  return findJointTrajectorySetItem(item->parent());
}

tesseract_common::JointTrajectorySet JointTrajectoryModel::getJointTrajectorySet(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  return findJointTrajectorySetItem(item)->trajectory_set;
}

bool JointTrajectoryModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::JointTrajectoryAdd::kType)
  {
    assert(dynamic_cast<events::JointTrajectoryAdd*>(event) != nullptr);
    auto* e = static_cast<events::JointTrajectoryAdd*>(event);
    if (e->getComponentInfo() == data_->component_info)
      addJointTrajectorySet(e->getJointTrajectory());
  }
  else if (event->type() == events::JointTrajectoryRemove::kType)
  {
    assert(dynamic_cast<events::JointTrajectoryRemove*>(event) != nullptr);
    auto* e = static_cast<events::JointTrajectoryRemove*>(event);
    if (e->getComponentInfo() == data_->component_info)
      removeJointTrajectorySet(e->getUUID());
  }
  else if (event->type() == events::JointTrajectoryRemoveAll::kType)
  {
    assert(dynamic_cast<events::JointTrajectoryRemoveAll*>(event) != nullptr);
    auto* e = static_cast<events::JointTrajectoryRemoveAll*>(event);
    if (e->getComponentInfo() == data_->component_info)
      clear();
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

}  // namespace tesseract_gui
