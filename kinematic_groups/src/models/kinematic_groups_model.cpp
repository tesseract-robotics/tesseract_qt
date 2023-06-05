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
#include <tesseract_qt/kinematic_groups/models/kinematic_groups_model.h>
#include <tesseract_qt/kinematic_groups/models/chain_group_standard_item.h>
#include <tesseract_qt/kinematic_groups/models/joint_group_standard_item.h>
#include <tesseract_qt/kinematic_groups/models/link_group_standard_item.h>
#include <tesseract_qt/common/events/kinematic_groups_events.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_environment/environment.h>

#include <QApplication>

static const QString CHAIN_GROUPS_KEY = "Chain Groups";
static const QString JOINT_GROUPS_KEY = "Joint Groups";
static const QString LINK_GROUPS_KEY = "Link Groups";

namespace tesseract_gui
{
struct KinematicGroupsModel::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;
  tesseract_srdf::GroupNames group_names;
  tesseract_srdf::ChainGroups chain_groups;
  tesseract_srdf::JointGroups joint_groups;
  tesseract_srdf::LinkGroups link_groups;
};

KinematicGroupsModel::KinematicGroupsModel(QObject* parent) : KinematicGroupsModel(nullptr, parent) {}

KinematicGroupsModel::KinematicGroupsModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();

  data_->component_info = std::move(component_info);

  // If an environment has already been assigned load the data
  auto env_wrapper = EnvironmentManager::get(data_->component_info);
  if (env_wrapper != nullptr && env_wrapper->getEnvironment()->isInitialized())
  {
    auto kin_info = env_wrapper->getEnvironment()->getKinematicsInformation();
    set(kin_info.chain_groups, kin_info.joint_groups, kin_info.link_groups);
  }

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

KinematicGroupsModel::KinematicGroupsModel(const KinematicGroupsModel& other) : QStandardItemModel(other.d_ptr->parent)
{
  this->data_ = std::make_unique<Implementation>(*other.data_);
}
KinematicGroupsModel& KinematicGroupsModel::operator=(const KinematicGroupsModel& other)
{
  this->data_ = std::make_unique<Implementation>(*other.data_);
  return *this;
}

KinematicGroupsModel::~KinematicGroupsModel() = default;

std::shared_ptr<const ComponentInfo> KinematicGroupsModel::getComponentInfo() const { return data_->component_info; }

void KinematicGroupsModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });

  appendRow(new QStandardItem(CHAIN_GROUPS_KEY));
  appendRow(new QStandardItem(JOINT_GROUPS_KEY));
  appendRow(new QStandardItem(LINK_GROUPS_KEY));

  Q_EMIT cleared();
}

void KinematicGroupsModel::set(const tesseract_srdf::ChainGroups& chain_groups,
                               const tesseract_srdf::JointGroups& joint_groups,
                               const tesseract_srdf::LinkGroups& link_groups)
{
  clear();
  for (const auto& group : chain_groups)
    addChainGroup(group.first, group.second);

  for (const auto& group : joint_groups)
    addJointGroup(group.first, group.second);

  for (const auto& group : link_groups)
    addLinkGroup(group.first, group.second);
}

void KinematicGroupsModel::addChainGroup(const std::string& group_name, const tesseract_srdf::ChainGroup& group)
{
  if (data_->group_names.find(group_name) != data_->group_names.end())
    removeGroup(group_name);

  QStandardItem* item = findItems(CHAIN_GROUPS_KEY).at(0);
  item->appendRow(new ChainGroupStandardItem(group_name.c_str(), group));

  data_->chain_groups[group_name] = group;
  data_->group_names.insert(group_name);

  Q_EMIT groupAdded(group_name.c_str());
}

void KinematicGroupsModel::addJointGroup(const std::string& group_name, const tesseract_srdf::JointGroup& group)
{
  if (data_->group_names.find(group_name) != data_->group_names.end())
    removeGroup(group_name);

  QStandardItem* item = findItems(JOINT_GROUPS_KEY).at(0);
  item->appendRow(new JointGroupStandardItem(group_name.c_str(), group));

  data_->joint_groups[group_name] = group;
  data_->group_names.insert(group_name);

  Q_EMIT groupAdded(group_name.c_str());
}

void KinematicGroupsModel::addLinkGroup(const std::string& group_name, const tesseract_srdf::LinkGroup& group)
{
  if (data_->group_names.find(group_name) != data_->group_names.end())
    removeGroup(group_name);

  QStandardItem* item = findItems(LINK_GROUPS_KEY).at(0);
  item->appendRow(new LinkGroupStandardItem(group_name.c_str(), group));

  data_->link_groups[group_name] = group;
  data_->group_names.insert(group_name);

  Q_EMIT groupAdded(group_name.c_str());
}

void KinematicGroupsModel::removeGroup(const std::string& group_name)
{
  data_->group_names.erase(group_name);

  {
    QStandardItem* item = findItems(CHAIN_GROUPS_KEY).at(0);
    for (int i = item->rowCount(); i > 0; i--)
    {
      QStandardItem* child_item = item->child(i - 1);
      if (child_item->text().toStdString() == group_name)
        item->removeRow(i - 1);
    }

    auto it = data_->chain_groups.find(group_name);
    if (it != data_->chain_groups.end())
      data_->chain_groups.erase(it);
  }

  {
    QStandardItem* item = findItems(JOINT_GROUPS_KEY).at(0);
    for (int i = item->rowCount(); i > 0; i--)
    {
      QStandardItem* child_item = item->child(i - 1);
      if (child_item->text().toStdString() == group_name)
        item->removeRow(i - 1);
    }

    auto it = data_->joint_groups.find(group_name);
    if (it != data_->joint_groups.end())
      data_->joint_groups.erase(it);
  }

  {
    QStandardItem* item = findItems(LINK_GROUPS_KEY).at(0);
    for (int i = item->rowCount(); i > 0; i--)
    {
      QStandardItem* child_item = item->child(i - 1);
      if (child_item->text().toStdString() == group_name)
        item->removeRow(i - 1);
    }

    auto it = data_->link_groups.find(group_name);
    if (it != data_->link_groups.end())
      data_->link_groups.erase(it);
  }

  Q_EMIT groupRemoved(group_name.c_str());
}

const tesseract_srdf::GroupNames& KinematicGroupsModel::getGroupNames() const { return data_->group_names; }

const tesseract_srdf::ChainGroups& KinematicGroupsModel::getChainGroups() const { return data_->chain_groups; }

const tesseract_srdf::JointGroups& KinematicGroupsModel::getJointGroups() const { return data_->joint_groups; }

const tesseract_srdf::LinkGroups& KinematicGroupsModel::getLinkGroups() const { return data_->link_groups; }

bool KinematicGroupsModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::KinematicGroupsSet::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsSet*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsSet*>(event);
    if (e->getComponentInfo() == data_->component_info)
      set(e->getChainGroups(), e->getJointGroups(), e->getLinkGroups());
  }
  else if (event->type() == events::KinematicGroupsAddChain::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsAddChain*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsAddChain*>(event);
    if (e->getComponentInfo() == data_->component_info)
      addChainGroup(e->getGroupName(), e->getGroup());
  }
  else if (event->type() == events::KinematicGroupsAddJoint::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsAddJoint*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsAddJoint*>(event);
    if (e->getComponentInfo() == data_->component_info)
      addJointGroup(e->getGroupName(), e->getGroup());
  }
  else if (event->type() == events::KinematicGroupsAddLink::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsAddLink*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsAddLink*>(event);
    if (e->getComponentInfo() == data_->component_info)
      addLinkGroup(e->getGroupName(), e->getGroup());
  }
  else if (event->type() == events::KinematicGroupsClear::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsClear*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsClear*>(event);
    if (e->getComponentInfo() == data_->component_info)
      clear();
  }
  else if (event->type() == events::KinematicGroupsRemove::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsRemove*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsRemove*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      for (const auto& link_name : e->getGroupNames())
        removeGroup(link_name);
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

}  // namespace tesseract_gui
