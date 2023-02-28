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
#include <tesseract_qt/kinematic_groups/models/group_tcps_standard_item.h>
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
GroupTCPsStandardItem::GroupTCPsStandardItem() : QStandardItem("Group TCPs") { ctor(); }

GroupTCPsStandardItem::GroupTCPsStandardItem(tesseract_srdf::GroupTCPs group_tcps)
  : QStandardItem("Group TCPs"), group_tcps_(std::move(group_tcps))
{
  ctor();
}

GroupTCPsStandardItem::GroupTCPsStandardItem(const QString& text, tesseract_srdf::GroupTCPs group_tcps)
  : QStandardItem(text), group_tcps_(std::move(group_tcps))
{
  ctor();
}

GroupTCPsStandardItem::GroupTCPsStandardItem(const QIcon& icon,
                                             const QString& text,
                                             tesseract_srdf::GroupTCPs group_tcps)
  : QStandardItem(icon, text), group_tcps_(std::move(group_tcps))
{
  ctor();
}

int GroupTCPsStandardItem::type() const { return static_cast<int>(StandardItemType::SRDF_GROUP_TCPS); }

void GroupTCPsStandardItem::addGroupTCPItem(const QString& group_name,
                                            const QString& tcp_name,
                                            const Eigen::Isometry3d& tcp)
{
  QStandardItem* group_item{ nullptr };
  auto it = group_items_.find(group_name.toStdString());
  if (it != group_items_.end())
  {
    group_item = it->second;
  }
  else
  {
    group_item = new QStandardItem(icons::getRobotArmIcon(), group_name);
    group_items_[group_name.toStdString()] = group_item;
    appendRow({ group_item, new QStandardItem() });  // Must have two columns so QTreeView::setRootIndex will have two
                                                     // columns
  }

  group_item->appendRow(new TransformStandardItem(tcp_name, tcp));
}

void GroupTCPsStandardItem::addGroupTCP(const QString& group_name,
                                        const QString& tcp_name,
                                        const Eigen::Isometry3d& tcp)
{
  removeGroupTCP(group_name, tcp_name);
  addGroupTCPItem(group_name, tcp_name, tcp);

  group_tcps_[group_name.toStdString()][tcp_name.toStdString()] = tcp;
}

void GroupTCPsStandardItem::removeGroupTCP(const QString& group_name, const QString& tcp_name)
{
  auto group_it = group_tcps_.find(group_name.toStdString());
  if (group_it != group_tcps_.end())
  {
    QStandardItem* item = group_items_.at(group_name.toStdString());
    for (int i = item->rowCount(); i > 0; i--)
    {
      QStandardItem* child_item = item->child(i - 1);
      if (child_item->text() == tcp_name)
        item->removeRow(i - 1);
    }

    auto state_it = group_it->second.find(tcp_name.toStdString());
    if (state_it != group_it->second.end())
      group_it->second.erase(state_it);

    if (group_it->second.empty())
    {
      group_tcps_.erase(group_it);
      removeRow(item->index().row());
    }
  }
}

void GroupTCPsStandardItem::removeGroup(const QString& group_name)
{
  group_tcps_.erase(group_name.toStdString());

  QStandardItem* item = group_items_.at(group_name.toStdString());
  removeRow(item->index().row());
}

const tesseract_srdf::GroupTCPs& GroupTCPsStandardItem::getGroupTCPs() const { return group_tcps_; }

void GroupTCPsStandardItem::ctor()
{
  for (auto& group : group_tcps_)
  {
    for (auto& tcp : group.second)
      addGroupTCPItem(QString::fromStdString(group.first), QString::fromStdString(tcp.first), tcp.second);
  }
}
}  // namespace tesseract_gui
