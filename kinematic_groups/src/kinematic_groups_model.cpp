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
#include <tesseract_qt/kinematic_groups/chain_group_standard_item.h>
#include <tesseract_qt/kinematic_groups/joint_group_standard_item.h>
#include <tesseract_qt/kinematic_groups/link_group_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>

static const QString CHAIN_GROUPS_KEY = "Chain Groups";
static const QString JOINT_GROUPS_KEY = "Joint Groups";
static const QString LINK_GROUPS_KEY = "Link Groups";

namespace tesseract_gui
{
KinematicGroupsModel::KinematicGroupsModel(QObject* parent) : QStandardItemModel(parent) { clear(); }
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
    addChainGroup(QString::fromStdString(group.first), group.second);

  for (const auto& group : joint_groups)
    addJointGroup(QString::fromStdString(group.first), group.second);

  for (const auto& group : link_groups)
    addLinkGroup(QString::fromStdString(group.first), group.second);
}

void KinematicGroupsModel::addChainGroup(QString group_name, tesseract_srdf::ChainGroup group)
{
  if (group_names_.find(group_name.toStdString()) != group_names_.end())
    removeGroup(group_name);

  QStandardItem* item = findItems(CHAIN_GROUPS_KEY).at(0);
  item->appendRow(new ChainGroupStandardItem(group_name, group));

  chain_groups_[group_name.toStdString()] = group;
  group_names_.insert(group_name.toStdString());

  Q_EMIT groupAdded(group_name);
}

void KinematicGroupsModel::addJointGroup(QString group_name, tesseract_srdf::JointGroup group)
{
  if (group_names_.find(group_name.toStdString()) != group_names_.end())
    removeGroup(group_name);

  QStandardItem* item = findItems(JOINT_GROUPS_KEY).at(0);
  item->appendRow(new JointGroupStandardItem(group_name, group));

  joint_groups_[group_name.toStdString()] = group;
  group_names_.insert(group_name.toStdString());

  Q_EMIT groupAdded(group_name);
}

void KinematicGroupsModel::addLinkGroup(QString group_name, tesseract_srdf::LinkGroup group)
{
  if (group_names_.find(group_name.toStdString()) != group_names_.end())
    removeGroup(group_name);

  QStandardItem* item = findItems(LINK_GROUPS_KEY).at(0);
  item->appendRow(new LinkGroupStandardItem(group_name, group));

  link_groups_[group_name.toStdString()] = group;
  group_names_.insert(group_name.toStdString());

  Q_EMIT groupAdded(group_name);
}

void KinematicGroupsModel::removeGroup(QString group_name)
{
  group_names_.erase(group_name.toStdString());

  {
    QStandardItem* item = findItems(CHAIN_GROUPS_KEY).at(0);
    for (int i = item->rowCount(); i > 0; i--)
    {
      QStandardItem* child_item = item->child(i - 1);
      if (child_item->text() == group_name)
        item->removeRow(i - 1);
    }

    auto it = chain_groups_.find(group_name.toStdString());
    if (it != chain_groups_.end())
      chain_groups_.erase(it);
  }

  {
    QStandardItem* item = findItems(JOINT_GROUPS_KEY).at(0);
    for (int i = item->rowCount(); i > 0; i--)
    {
      QStandardItem* child_item = item->child(i - 1);
      if (child_item->text() == group_name)
        item->removeRow(i - 1);
    }

    auto it = joint_groups_.find(group_name.toStdString());
    if (it != joint_groups_.end())
      joint_groups_.erase(it);
  }

  {
    QStandardItem* item = findItems(LINK_GROUPS_KEY).at(0);
    for (int i = item->rowCount(); i > 0; i--)
    {
      QStandardItem* child_item = item->child(i - 1);
      if (child_item->text() == group_name)
        item->removeRow(i - 1);
    }

    auto it = link_groups_.find(group_name.toStdString());
    if (it != link_groups_.end())
      link_groups_.erase(it);
  }

  Q_EMIT groupRemoved(group_name);
}

const tesseract_srdf::GroupNames& KinematicGroupsModel::getGroupNames() const { return group_names_; }

const tesseract_srdf::ChainGroups& KinematicGroupsModel::getChainGroups() const { return chain_groups_; }

const tesseract_srdf::JointGroups& KinematicGroupsModel::getJointGroups() const { return joint_groups_; }

const tesseract_srdf::LinkGroups& KinematicGroupsModel::getLinkGroups() const { return link_groups_; }

}  // namespace tesseract_gui
