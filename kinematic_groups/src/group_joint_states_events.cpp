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
#include <tesseract_qt/kinematic_groups/group_joint_states_events.h>

namespace tesseract_gui::events
{
GroupJointStatesClear::GroupJointStatesClear(const std::string& scene_name) : SceneEvent(scene_name, kType) {}

GroupJointStatesClear::~GroupJointStatesClear() = default;

//////////////////////////////////////////

GroupJointStatesSet::GroupJointStatesSet(const std::string& scene_name,
                                         const tesseract_srdf::GroupJointStates& group_joint_states)
  : SceneEvent(scene_name, kType), group_joint_states_(group_joint_states)
{
}

GroupJointStatesSet::~GroupJointStatesSet() = default;

const tesseract_srdf::GroupJointStates& GroupJointStatesSet::getGroupsJointStates() const
{
  return group_joint_states_;
}

//////////////////////////////////////////

GroupJointStatesAdd::GroupJointStatesAdd(const std::string& scene_name,
                                         std::string group_name,
                                         std::string state_name,
                                         tesseract_srdf::GroupsJointState state)
  : SceneEvent(scene_name, kType)
  , group_name_(std::move(group_name))
  , state_name_(std::move(state_name))
  , state_(std::move(state))
{
}

GroupJointStatesAdd::~GroupJointStatesAdd() = default;

const std::string& GroupJointStatesAdd::getGroupName() const { return group_name_; }
const std::string& GroupJointStatesAdd::getStateName() const { return state_name_; }
const tesseract_srdf::GroupsJointState& GroupJointStatesAdd::getJointState() const { return state_; }

//////////////////////////////////////////

GroupJointStatesRemove::GroupJointStatesRemove(const std::string& scene_name,
                                               const std::vector<std::array<std::string, 2>>& entries)
  : SceneEvent(scene_name, kType), entries_(entries)
{
}

GroupJointStatesRemove::~GroupJointStatesRemove() = default;

const std::vector<std::array<std::string, 2>>& GroupJointStatesRemove::getEntries() const { return entries_; }

//////////////////////////////////////////

GroupJointStatesRemoveGroup::GroupJointStatesRemoveGroup(const std::string& scene_name,
                                                         const std::vector<std::string>& group_names)
  : SceneEvent(scene_name, kType), group_names_(group_names)
{
}

GroupJointStatesRemoveGroup::~GroupJointStatesRemoveGroup() = default;

const std::vector<std::string>& GroupJointStatesRemoveGroup::getGroupNames() const { return group_names_; }

//////////////////////////////////////////

}  // namespace tesseract_gui::events
