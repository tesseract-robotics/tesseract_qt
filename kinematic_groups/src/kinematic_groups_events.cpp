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
#include <tesseract_qt/kinematic_groups/kinematic_groups_events.h>

namespace tesseract_gui::events
{
KinematicGroupsClear::KinematicGroupsClear(const std::string& scene_name) : SceneEvent(scene_name, kType) {}

KinematicGroupsClear::~KinematicGroupsClear() = default;

//////////////////////////////////////////

KinematicGroupsSet::KinematicGroupsSet(const std::string& scene_name,
                                       const tesseract_srdf::ChainGroups& chain_groups,
                                       const tesseract_srdf::JointGroups& joint_groups,
                                       const tesseract_srdf::LinkGroups& link_groups)
  : SceneEvent(scene_name, kType), chain_groups_(chain_groups), joint_groups_(joint_groups), link_groups_(link_groups)
{
}

KinematicGroupsSet::~KinematicGroupsSet() = default;

const tesseract_srdf::ChainGroups& KinematicGroupsSet::getChainGroups() const { return chain_groups_; }
const tesseract_srdf::JointGroups& KinematicGroupsSet::getJointGroups() const { return joint_groups_; }
const tesseract_srdf::LinkGroups& KinematicGroupsSet::getLinkGroups() const { return link_groups_; }

//////////////////////////////////////////

KinematicGroupsAddChain::KinematicGroupsAddChain(const std::string& scene_name,
                                                 std::string group_name,
                                                 tesseract_srdf::ChainGroup group)
  : SceneEvent(scene_name, kType), group_name_(std::move(group_name)), group_(std::move(group))
{
}

KinematicGroupsAddChain::~KinematicGroupsAddChain() = default;

const std::string& KinematicGroupsAddChain::getGroupName() const { return group_name_; }
const tesseract_srdf::ChainGroup& KinematicGroupsAddChain::getGroup() const { return group_; }

//////////////////////////////////////////

KinematicGroupsAddJoint::KinematicGroupsAddJoint(const std::string& scene_name,
                                                 std::string group_name,
                                                 tesseract_srdf::JointGroup group)
  : SceneEvent(scene_name, kType), group_name_(std::move(group_name)), group_(std::move(group))
{
}

KinematicGroupsAddJoint::~KinematicGroupsAddJoint() = default;

const std::string& KinematicGroupsAddJoint::getGroupName() const { return group_name_; }
const tesseract_srdf::JointGroup& KinematicGroupsAddJoint::getGroup() const { return group_; }

//////////////////////////////////////////

KinematicGroupsAddLink::KinematicGroupsAddLink(const std::string& scene_name,
                                               std::string group_name,
                                               tesseract_srdf::LinkGroup group)
  : SceneEvent(scene_name, kType), group_name_(std::move(group_name)), group_(std::move(group))
{
}

KinematicGroupsAddLink::~KinematicGroupsAddLink() = default;

const std::string& KinematicGroupsAddLink::getGroupName() const { return group_name_; }
const tesseract_srdf::LinkGroup& KinematicGroupsAddLink::getGroup() const { return group_; }

//////////////////////////////////////////

KinematicGroupsRemove::KinematicGroupsRemove(const std::string& scene_name, const std::vector<std::string>& group_names)
  : SceneEvent(scene_name, kType), group_names_(group_names)
{
}

KinematicGroupsRemove::~KinematicGroupsRemove() = default;

const std::vector<std::string>& KinematicGroupsRemove::getGroupNames() const { return group_names_; }

//////////////////////////////////////////

}  // namespace tesseract_gui::events
