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
#include <tesseract_qt/kinematic_groups/group_tcps_events.h>

namespace tesseract_gui::events
{
GroupTCPsClear::GroupTCPsClear(const std::string& scene_name) : SceneEvent(scene_name, kType) {}

GroupTCPsClear::~GroupTCPsClear() = default;

//////////////////////////////////////////

GroupTCPsSet::GroupTCPsSet(const std::string& scene_name, const tesseract_srdf::GroupTCPs& group_tcps)
  : SceneEvent(scene_name, kType), group_tcps_(group_tcps)
{
}

GroupTCPsSet::~GroupTCPsSet() = default;

const tesseract_srdf::GroupTCPs& GroupTCPsSet::getGroupTCPs() const { return group_tcps_; }

//////////////////////////////////////////

GroupTCPsAdd::GroupTCPsAdd(const std::string& scene_name,
                           std::string group_name,
                           std::string tcp_name,
                           const Eigen::Isometry3d& tcp)
  : SceneEvent(scene_name, kType), group_name_(std::move(group_name)), tcp_name_(std::move(tcp_name)), tcp_(tcp)
{
}

GroupTCPsAdd::~GroupTCPsAdd() = default;

const std::string& GroupTCPsAdd::getGroupName() const { return group_name_; }
const std::string& GroupTCPsAdd::getTCPName() const { return tcp_name_; }
const Eigen::Isometry3d& GroupTCPsAdd::getTCP() const { return tcp_; }

//////////////////////////////////////////

GroupTCPsRemove::GroupTCPsRemove(const std::string& scene_name, const std::vector<std::array<std::string, 2>>& entries)
  : SceneEvent(scene_name, kType), entries_(entries)
{
}

GroupTCPsRemove::~GroupTCPsRemove() = default;

const std::vector<std::array<std::string, 2>>& GroupTCPsRemove::getEntries() const { return entries_; }

//////////////////////////////////////////

GroupTCPsRemoveGroup::GroupTCPsRemoveGroup(const std::string& scene_name, const std::vector<std::string>& group_names)
  : SceneEvent(scene_name, kType), group_names_(group_names)
{
}

GroupTCPsRemoveGroup::~GroupTCPsRemoveGroup() = default;

const std::vector<std::string>& GroupTCPsRemoveGroup::getGroupNames() const { return group_names_; }

//////////////////////////////////////////

}  // namespace tesseract_gui::events
