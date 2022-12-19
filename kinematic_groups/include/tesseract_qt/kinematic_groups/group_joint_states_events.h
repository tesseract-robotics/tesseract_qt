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
#ifndef TESSERACT_QT_KINEMATIC_GROUPS_GROUP_JOINT_STATES_EVENTS_H
#define TESSERACT_QT_KINEMATIC_GROUPS_GROUP_JOINT_STATES_EVENTS_H

#include <tesseract_qt/common/event_type.h>
#include <tesseract_qt/common/scene_events.h>
#include <tesseract_srdf/kinematics_information.h>

namespace tesseract_gui::events
{
class GroupJointStatesClear : public SceneEvent
{
public:
  GroupJointStatesClear(const std::string& scene_name);
  ~GroupJointStatesClear() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_CLEAR);
};

class GroupJointStatesSet : public SceneEvent
{
public:
  GroupJointStatesSet(const std::string& scene_name, const tesseract_srdf::GroupJointStates& group_joint_states);
  ~GroupJointStatesSet() override;

  const tesseract_srdf::GroupJointStates& getGroupsJointStates() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_SET);

private:
  tesseract_srdf::GroupJointStates group_joint_states_;
};

class GroupJointStatesAdd : public SceneEvent
{
public:
  GroupJointStatesAdd(const std::string& scene_name,
                      std::string group_name,
                      std::string state_name,
                      tesseract_srdf::GroupsJointState state);
  ~GroupJointStatesAdd() override;

  const std::string& getGroupName() const;
  const std::string& getStateName() const;
  const tesseract_srdf::GroupsJointState& getJointState() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_ADD);

private:
  std::string group_name_;
  std::string state_name_;
  tesseract_srdf::GroupsJointState state_;
};

class GroupJointStatesRemove : public SceneEvent
{
public:
  GroupJointStatesRemove(const std::string& scene_name, const std::vector<std::array<std::string, 2>>& entries);
  ~GroupJointStatesRemove() override;

  const std::vector<std::array<std::string, 2>>& getEntries() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_REMOVE);

private:
  std::vector<std::array<std::string, 2>> entries_;
};

class GroupJointStatesRemoveGroup : public SceneEvent
{
public:
  GroupJointStatesRemoveGroup(const std::string& scene_name, const std::vector<std::string>& group_names);
  ~GroupJointStatesRemoveGroup() override;

  const std::vector<std::string>& getGroupNames() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_REMOVE_GROUP);

private:
  std::vector<std::string> group_names_;
};

// class GroupJointStatesShow : public SceneEvent
//{
// public:
//  GroupJointStatesShow(const std::string& scene_name, QString group_name, QString state_name,
//  tesseract_srdf::GroupsJointState state); ~GroupJointStatesShow() override;

//  const std::string& getGroupName() const;
//  const std::string& getStateName() const;
//  const tesseract_srdf::GroupsJointState& getJointState() const;

//  /** @brief Unique type for this event. */
//  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_SHOW);

// private:
//  struct Implementation;
//  std::unique_ptr<Implementation> data_;
//};

}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_KINEMATIC_GROUPS_GROUP_JOINT_STATES_EVENTS_H
