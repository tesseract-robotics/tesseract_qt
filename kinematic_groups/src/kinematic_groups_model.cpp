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
#include <tesseract_qt/kinematic_groups/kinematic_groups_model.h>
#include <tesseract_qt/kinematic_groups/kinematic_groups_events.h>
#include <tesseract_qt/kinematic_groups/chain_group_standard_item.h>
#include <tesseract_qt/kinematic_groups/joint_group_standard_item.h>
#include <tesseract_qt/kinematic_groups/link_group_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>

#include <QApplication>

static const QString CHAIN_GROUPS_KEY = "Chain Groups";
static const QString JOINT_GROUPS_KEY = "Joint Groups";
static const QString LINK_GROUPS_KEY = "Link Groups";

namespace tesseract_gui
{
KinematicGroupsModel::KinematicGroupsModel(std::string scene_name, QObject* parent)
  : QStandardItemModel(parent), scene_name_(std::move(scene_name))
{
  clear();

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}
KinematicGroupsModel::KinematicGroupsModel(const KinematicGroupsModel& other) : QStandardItemModel(other.d_ptr->parent)
{
  this->group_names_ = other.group_names_;
  this->chain_groups_ = other.chain_groups_;
  this->joint_groups_ = other.joint_groups_;
  this->link_groups_ = other.link_groups_;
}
KinematicGroupsModel& KinematicGroupsModel::operator=(const KinematicGroupsModel& other)
{
  this->group_names_ = other.group_names_;
  this->chain_groups_ = other.chain_groups_;
  this->joint_groups_ = other.joint_groups_;
  this->link_groups_ = other.link_groups_;
  return *this;
}

const std::string& KinematicGroupsModel::getSceneName() const { return scene_name_; }

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
  if (group_names_.find(group_name) != group_names_.end())
    removeGroup(group_name);

  QStandardItem* item = findItems(CHAIN_GROUPS_KEY).at(0);
  item->appendRow(new ChainGroupStandardItem(group_name.c_str(), group));

  chain_groups_[group_name] = group;
  group_names_.insert(group_name);

  Q_EMIT groupAdded(group_name.c_str());
}

void KinematicGroupsModel::addJointGroup(const std::string& group_name, const tesseract_srdf::JointGroup& group)
{
  if (group_names_.find(group_name) != group_names_.end())
    removeGroup(group_name);

  QStandardItem* item = findItems(JOINT_GROUPS_KEY).at(0);
  item->appendRow(new JointGroupStandardItem(group_name.c_str(), group));

  joint_groups_[group_name] = group;
  group_names_.insert(group_name);

  Q_EMIT groupAdded(group_name.c_str());
}

void KinematicGroupsModel::addLinkGroup(const std::string& group_name, const tesseract_srdf::LinkGroup& group)
{
  if (group_names_.find(group_name) != group_names_.end())
    removeGroup(group_name);

  QStandardItem* item = findItems(LINK_GROUPS_KEY).at(0);
  item->appendRow(new LinkGroupStandardItem(group_name.c_str(), group));

  link_groups_[group_name] = group;
  group_names_.insert(group_name);

  Q_EMIT groupAdded(group_name.c_str());
}

void KinematicGroupsModel::removeGroup(const std::string& group_name)
{
  group_names_.erase(group_name);

  {
    QStandardItem* item = findItems(CHAIN_GROUPS_KEY).at(0);
    for (int i = item->rowCount(); i > 0; i--)
    {
      QStandardItem* child_item = item->child(i - 1);
      if (child_item->text().toStdString() == group_name)
        item->removeRow(i - 1);
    }

    auto it = chain_groups_.find(group_name);
    if (it != chain_groups_.end())
      chain_groups_.erase(it);
  }

  {
    QStandardItem* item = findItems(JOINT_GROUPS_KEY).at(0);
    for (int i = item->rowCount(); i > 0; i--)
    {
      QStandardItem* child_item = item->child(i - 1);
      if (child_item->text().toStdString() == group_name)
        item->removeRow(i - 1);
    }

    auto it = joint_groups_.find(group_name);
    if (it != joint_groups_.end())
      joint_groups_.erase(it);
  }

  {
    QStandardItem* item = findItems(LINK_GROUPS_KEY).at(0);
    for (int i = item->rowCount(); i > 0; i--)
    {
      QStandardItem* child_item = item->child(i - 1);
      if (child_item->text().toStdString() == group_name)
        item->removeRow(i - 1);
    }

    auto it = link_groups_.find(group_name);
    if (it != link_groups_.end())
      link_groups_.erase(it);
  }

  Q_EMIT groupRemoved(group_name.c_str());
}

const tesseract_srdf::GroupNames& KinematicGroupsModel::getGroupNames() const { return group_names_; }

const tesseract_srdf::ChainGroups& KinematicGroupsModel::getChainGroups() const { return chain_groups_; }

const tesseract_srdf::JointGroups& KinematicGroupsModel::getJointGroups() const { return joint_groups_; }

const tesseract_srdf::LinkGroups& KinematicGroupsModel::getLinkGroups() const { return link_groups_; }

bool KinematicGroupsModel::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::KinematicGroupsSet::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsSet*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsSet*>(event);
    if (e->getSceneName() == scene_name_)
      set(e->getChainGroups(), e->getJointGroups(), e->getLinkGroups());
  }
  else if (event->type() == events::KinematicGroupsAddChain::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsAddChain*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsAddChain*>(event);
    if (e->getSceneName() == scene_name_)
      addChainGroup(e->getGroupName(), e->getGroup());
  }
  else if (event->type() == events::KinematicGroupsAddJoint::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsAddJoint*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsAddJoint*>(event);
    if (e->getSceneName() == scene_name_)
      addJointGroup(e->getGroupName(), e->getGroup());
  }
  else if (event->type() == events::KinematicGroupsAddLink::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsAddLink*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsAddLink*>(event);
    if (e->getSceneName() == scene_name_)
      addLinkGroup(e->getGroupName(), e->getGroup());
  }
  else if (event->type() == events::KinematicGroupsClear::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsClear*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsClear*>(event);
    if (e->getSceneName() == scene_name_)
      clear();
  }
  else if (event->type() == events::KinematicGroupsRemove::kType)
  {
    assert(dynamic_cast<events::KinematicGroupsRemove*>(event) != nullptr);
    auto* e = static_cast<events::KinematicGroupsRemove*>(event);
    if (e->getSceneName() == scene_name_)
    {
      for (const auto& link_name : e->getGroupNames())
        removeGroup(link_name);
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

}  // namespace tesseract_gui
