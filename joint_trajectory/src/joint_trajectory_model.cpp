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
#include <tesseract_qt/joint_trajectory/joint_trajectory_model.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_set_item.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_info_item.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_state_item.h>
#include <tesseract_qt/common/namespace_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/utils.h>

#include <QUuid>

namespace tesseract_gui
{
JointTrajectoryModel::JointTrajectoryModel(QObject* parent) : QStandardItemModel(parent) { clear(); }

void JointTrajectoryModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
}

QString JointTrajectoryModel::addJointTrajectorySet(const tesseract_common::JointTrajectorySet& trajectory_set)
{
  QString key = QUuid::createUuid().toString();
  std::string ns = (trajectory_set.getNamespace().empty()) ? "general" : trajectory_set.getNamespace();
  NamespaceStandardItem* item = createNamespaceItem(*invisibleRootItem(), ns);

  auto* trajectory_container_item = new JointTrajectorySetItem(key, trajectory_set);
  auto* trajectory_description_item = new QStandardItem(QString::fromStdString(trajectory_set.getDescription()));
  item->appendRow({ trajectory_container_item, trajectory_description_item });
  trajectory_sets_[key] = trajectory_container_item;
  return key;
}

void JointTrajectoryModel::removeJointTrajectorySet(const QString& key)
{
  auto it = trajectory_sets_.find(key);
  if (it == trajectory_sets_.end())
    throw std::runtime_error("Tried to remove trajectory set '" + key.toStdString() + "' which does not exist!");

  QModelIndex idx = indexFromItem(it->second);
  trajectory_sets_.erase(it);
  removeRow(idx.row());
}

bool JointTrajectoryModel::hasJointTrajectorySet(const QString& key)
{
  return (trajectory_sets_.find(key) != trajectory_sets_.end());
}

JointTrajectoryStateItem* findJointStateItem(QStandardItem* item)
{
  if (item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET_STATE))
    return dynamic_cast<JointTrajectoryStateItem*>(item);

  return findJointStateItem(item->parent());
}

const tesseract_common::JointState& JointTrajectoryModel::getJointState(const QModelIndex& row) const
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

const tesseract_common::JointTrajectoryInfo& JointTrajectoryModel::getJointTrajectory(const QModelIndex& row) const
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

const tesseract_common::JointTrajectorySet& JointTrajectoryModel::getJointTrajectorySet(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  return findJointTrajectorySetItem(item)->trajectory_set;
}

std::pair<const QString&, const tesseract_common::JointTrajectorySet&>
JointTrajectoryModel::getJointTrajectorySetDetails(const QModelIndex& row) const
{
  QStandardItem* item = itemFromIndex(row);
  JointTrajectorySetItem* jts_item = findJointTrajectorySetItem(item);
  return std::pair<const QString&, const tesseract_common::JointTrajectorySet&>(jts_item->uuid,
                                                                                jts_item->trajectory_set);
}

}  // namespace tesseract_gui
