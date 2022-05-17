/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include <tesseract_qt/kinematic_groups/group_tcps_model.h>
#include <tesseract_qt/common/standard_item_type.h>

namespace tesseract_gui
{
GroupTCPsModel::GroupTCPsModel(QObject* parent) : QStandardItemModel(parent) { clear(); }
GroupTCPsModel::GroupTCPsModel(const GroupTCPsModel& other) : QStandardItemModel(other.d_ptr->parent) {}

GroupTCPsModel& GroupTCPsModel::operator=(const GroupTCPsModel& /*other*/) { return *this; }

void GroupTCPsModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  appendRow(new GroupTCPsStandardItem());
}

void GroupTCPsModel::set(const tesseract_srdf::GroupTCPs& group_tcps)
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  appendRow(new GroupTCPsStandardItem(group_tcps));
}

void GroupTCPsModel::addGroupTCP(const QString& group_name, const QString& tcp_name, const Eigen::Isometry3d& tcp)
{
  getRoot()->addGroupTCP(group_name, tcp_name, tcp);
}

void GroupTCPsModel::removeGroupTCP(const QString& group_name, const QString& tcp_name)
{
  getRoot()->removeGroupTCP(group_name, tcp_name);
}

const tesseract_srdf::GroupTCPs& GroupTCPsModel::getGroupTCPs() const { return getRoot()->getGroupTCPs(); }

GroupTCPsStandardItem* GroupTCPsModel::getRoot() { return dynamic_cast<GroupTCPsStandardItem*>(item(0)); }

const GroupTCPsStandardItem* GroupTCPsModel::getRoot() const
{
  return dynamic_cast<const GroupTCPsStandardItem*>(item(0));
}

}  // namespace tesseract_gui
