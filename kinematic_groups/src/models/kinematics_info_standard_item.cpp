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
#include <tesseract_qt/kinematic_groups/models/kinematics_info_standard_item.h>
#include <tesseract_qt/kinematic_groups/models/chain_group_standard_item.h>
#include <tesseract_qt/kinematic_groups/models/joint_group_standard_item.h>
#include <tesseract_qt/kinematic_groups/models/link_group_standard_item.h>
#include <tesseract_qt/kinematic_groups/models/group_joint_states_standard_item.h>
#include <tesseract_qt/kinematic_groups/models/group_tcps_standard_item.h>
#include <tesseract_qt/common/models/kinematics_plugin_info_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
KinematicsInfoStandardItem::KinematicsInfoStandardItem(tesseract_srdf::KinematicsInformation kinematics_info)
  : QStandardItem(icons::getCubeIcon(), "Kinematics Info"), kinematics_info(std::move(kinematics_info))
{
  ctor();
}

KinematicsInfoStandardItem::KinematicsInfoStandardItem(const QString& text,
                                                       tesseract_srdf::KinematicsInformation kinematics_info)
  : QStandardItem(icons::getCubeIcon(), text), kinematics_info(std::move(kinematics_info))
{
  ctor();
}

KinematicsInfoStandardItem::KinematicsInfoStandardItem(const QIcon& icon,
                                                       const QString& text,
                                                       tesseract_srdf::KinematicsInformation kinematics_info)
  : QStandardItem(icon, text), kinematics_info(std::move(kinematics_info))
{
  ctor();
}

int KinematicsInfoStandardItem::type() const { return static_cast<int>(StandardItemType::SRDF_KINEMATICS_INFO); }

void KinematicsInfoStandardItem::ctor()
{
  auto* group_names = new QStandardItem("Group Names");
  std::size_t cnt{ 0 };
  for (const auto& group_name : kinematics_info.group_names)
    group_names->appendRow(createStandardItemString(QString("[%1]").arg(cnt++).toStdString(), group_name));

  appendRow({ group_names, new QStandardItem() });

  auto* chain_groups = new QStandardItem("Chain Groups");
  for (const auto& chain_group : kinematics_info.chain_groups)
    chain_groups->appendRow(new ChainGroupStandardItem(chain_group.first.c_str(), chain_group.second));

  appendRow({ chain_groups, new QStandardItem() });

  auto* joint_groups = new QStandardItem("Joint Groups");
  for (const auto& joint_group : kinematics_info.joint_groups)
    joint_groups->appendRow(new JointGroupStandardItem(joint_group.first.c_str(), joint_group.second));

  appendRow({ joint_groups, new QStandardItem() });

  auto* link_groups = new QStandardItem("Link Groups");
  for (const auto& link_group : kinematics_info.link_groups)
    link_groups->appendRow(new LinkGroupStandardItem(link_group.first.c_str(), link_group.second));

  appendRow({ link_groups, new QStandardItem() });

  auto* group_states = new GroupJointStatesStandardItem(kinematics_info.group_states);
  appendRow({ group_states, new QStandardItem() });

  auto* group_tcps = new GroupTCPsStandardItem(kinematics_info.group_tcps);
  appendRow({ group_tcps, new QStandardItem() });

  auto* kin_plugins = new KinematicsPluginInfoStandardItem(kinematics_info.kinematics_plugin_info);
  appendRow({ kin_plugins, new QStandardItem() });
}
}  // namespace tesseract_gui
