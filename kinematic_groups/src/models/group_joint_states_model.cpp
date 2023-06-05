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
#include <tesseract_qt/kinematic_groups/models/group_joint_states_model.h>
#include <tesseract_qt/kinematic_groups/models/group_joint_state_standard_item.h>
#include <tesseract_qt/kinematic_groups/models/group_joint_states_standard_item.h>
#include <tesseract_qt/common/events/group_joint_states_events.h>
#include <tesseract_qt/common/models/namespace_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_common/joint_state.h>
#include <tesseract_environment/environment.h>

#include <QApplication>

namespace tesseract_gui
{
GroupJointStatesModel::GroupJointStatesModel(QObject* parent) : GroupJointStatesModel(nullptr, parent) {}

GroupJointStatesModel::GroupJointStatesModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent)
  : QStandardItemModel(parent), component_info_(std::move(component_info))
{
  clear();

  // If an environment has already been assigned load the data
  auto env_wrapper = EnvironmentManager::get(component_info_);
  if (env_wrapper != nullptr && env_wrapper->getEnvironment()->isInitialized())
    set(env_wrapper->getEnvironment()->getKinematicsInformation().group_states);

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

GroupJointStatesModel::GroupJointStatesModel(const GroupJointStatesModel& other)
  : QStandardItemModel(other.d_ptr->parent)
{
}

GroupJointStatesModel& GroupJointStatesModel::operator=(const GroupJointStatesModel& other) { return *this; }

GroupJointStatesModel::~GroupJointStatesModel() = default;

std::shared_ptr<const ComponentInfo> GroupJointStatesModel::getComponentInfo() const { return component_info_; }

bool GroupJointStatesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // Need emit application event to change visible
  if (role == Qt::CheckStateRole)
  {
    QStandardItem* item = itemFromIndex(index);
    if (item->type() == static_cast<int>(StandardItemType::SRDF_GROUP_JOINT_STATE))
    {
      assert(dynamic_cast<GroupJointStateStandardItem*>(item) != nullptr);
      assert(dynamic_cast<NamespaceStandardItem*>(item->parent()) != nullptr);
      auto* derived_item = static_cast<GroupJointStateStandardItem*>(item);
      auto* parent_item = static_cast<NamespaceStandardItem*>(item->parent());

      if (value.value<Qt::CheckState>() == Qt::Checked)
      {
        QApplication::sendEvent(qApp,
                                new events::GroupJointStatesShow(component_info_,
                                                                 parent_item->text().toStdString(),
                                                                 derived_item->getName().toStdString(),
                                                                 derived_item->getState()));
      }
      else
      {
        QApplication::sendEvent(qApp,
                                new events::GroupJointStatesHide(component_info_,
                                                                 parent_item->text().toStdString(),
                                                                 derived_item->getName().toStdString()));
      }
    }
  }
  return QStandardItemModel::setData(index, value, role);
}

void GroupJointStatesModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  appendRow(new GroupJointStatesStandardItem());
}

void GroupJointStatesModel::set(const tesseract_srdf::GroupJointStates& group_joint_states)
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  appendRow(new GroupJointStatesStandardItem(group_joint_states));
}

void GroupJointStatesModel::add(const std::string& group_name,
                                const std::string& state_name,
                                const tesseract_srdf::GroupsJointState& state)
{
  getRoot()->addGroupJointState(QString::fromStdString(group_name), QString::fromStdString(state_name), state);
}

void GroupJointStatesModel::remove(const std::string& group_name, const std::string& state_name)
{
  getRoot()->removeGroupJointState(QString::fromStdString(group_name), QString::fromStdString(state_name));
}

void GroupJointStatesModel::remove(const std::string& group_name)
{
  getRoot()->removeGroup(QString::fromStdString(group_name));
}

tesseract_srdf::GroupJointStates GroupJointStatesModel::getGroupsJointStates() const
{
  return getRoot()->getGroupJointStates();
}

GroupJointStateStandardItem* findGroupJointStateItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::SRDF_GROUP_JOINT_STATE))
    return dynamic_cast<GroupJointStateStandardItem*>(item);

  return findGroupJointStateItem(item->parent());
}

NamespaceStandardItem* findJointStateGroupItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::COMMON_NAMESPACE))
    return dynamic_cast<NamespaceStandardItem*>(item);

  return findJointStateGroupItem(item->parent());
}

tesseract_srdf::GroupsJointState GroupJointStatesModel::getGroupsJointState(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);

  if (item->type() == static_cast<int>(StandardItemType::COMMON_NAMESPACE))
    throw std::runtime_error("Cannot get joint state from selected group standard item");

  GroupJointStateStandardItem* joint_state_item = findGroupJointStateItem(item);
  NamespaceStandardItem* group_item = findJointStateGroupItem(joint_state_item);

  QString state_name = joint_state_item->text();
  QString group_name = group_item->text();
  return getRoot()->getGroupJointStates().at(group_name.toStdString()).at(state_name.toStdString());
}

GroupJointStatesStandardItem* GroupJointStatesModel::getRoot()
{
  return dynamic_cast<GroupJointStatesStandardItem*>(item(0));
}

const GroupJointStatesStandardItem* GroupJointStatesModel::getRoot() const
{
  return dynamic_cast<const GroupJointStatesStandardItem*>(item(0));
}

bool GroupJointStatesModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::GroupJointStatesSet::kType)
  {
    assert(dynamic_cast<events::GroupJointStatesSet*>(event) != nullptr);
    auto* e = static_cast<events::GroupJointStatesSet*>(event);
    if (e->getComponentInfo() == component_info_)
      set(e->getGroupJointStates());
  }
  else if (event->type() == events::GroupJointStatesAdd::kType)
  {
    assert(dynamic_cast<events::GroupJointStatesAdd*>(event) != nullptr);
    auto* e = static_cast<events::GroupJointStatesAdd*>(event);
    if (e->getComponentInfo() == component_info_)
      add(e->getGroupName(), e->getStateName(), e->getJointState());
  }
  else if (event->type() == events::GroupJointStatesClear::kType)
  {
    assert(dynamic_cast<events::GroupJointStatesClear*>(event) != nullptr);
    auto* e = static_cast<events::GroupJointStatesClear*>(event);
    if (e->getComponentInfo() == component_info_)
      clear();
  }
  else if (event->type() == events::GroupJointStatesRemove::kType)
  {
    assert(dynamic_cast<events::GroupJointStatesRemove*>(event) != nullptr);
    auto* e = static_cast<events::GroupJointStatesRemove*>(event);
    if (e->getComponentInfo() == component_info_)
    {
      for (const auto& entry : e->getEntries())
        remove(entry[0], entry[1]);
    }
  }
  else if (event->type() == events::GroupJointStatesRemoveGroup::kType)
  {
    assert(dynamic_cast<events::GroupJointStatesRemoveGroup*>(event) != nullptr);
    auto* e = static_cast<events::GroupJointStatesRemoveGroup*>(event);
    if (e->getComponentInfo() == component_info_)
    {
      for (const auto& link_name : e->getGroupNames())
        remove(link_name);
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
