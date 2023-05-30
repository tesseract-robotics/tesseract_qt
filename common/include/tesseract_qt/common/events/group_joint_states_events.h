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
#ifndef TESSERACT_QT_COMMON_GROUP_JOINT_STATES_EVENTS_H
#define TESSERACT_QT_COMMON_GROUP_JOINT_STATES_EVENTS_H

#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>
#include <tesseract_srdf/kinematics_information.h>

namespace tesseract_gui::events
{
class GroupJointStatesClear : public ComponentEvent
{
public:
  GroupJointStatesClear(std::shared_ptr<const ComponentInfo> component_info);
  ~GroupJointStatesClear() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_CLEAR);
};

class GroupJointStatesSet : public ComponentEvent
{
public:
  GroupJointStatesSet(std::shared_ptr<const ComponentInfo> component_info,
                      const tesseract_srdf::GroupJointStates& group_joint_states);
  ~GroupJointStatesSet() override;

  const tesseract_srdf::GroupJointStates& getGroupJointStates() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_SET);

private:
  tesseract_srdf::GroupJointStates group_joint_states_;
};

class GroupJointStatesAdd : public ComponentEvent
{
public:
  GroupJointStatesAdd(std::shared_ptr<const ComponentInfo> component_info,
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

class GroupJointStatesRemove : public ComponentEvent
{
public:
  GroupJointStatesRemove(std::shared_ptr<const ComponentInfo> component_info,
                         const std::vector<std::array<std::string, 2>>& entries);
  ~GroupJointStatesRemove() override;

  const std::vector<std::array<std::string, 2>>& getEntries() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_REMOVE);

private:
  std::vector<std::array<std::string, 2>> entries_;
};

class GroupJointStatesRemoveGroup : public ComponentEvent
{
public:
  GroupJointStatesRemoveGroup(std::shared_ptr<const ComponentInfo> component_info,
                              const std::vector<std::string>& group_names);
  ~GroupJointStatesRemoveGroup() override;

  const std::vector<std::string>& getGroupNames() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_REMOVE_GROUP);

private:
  std::vector<std::string> group_names_;
};

class GroupJointStatesShow : public ComponentEvent
{
public:
  GroupJointStatesShow(std::shared_ptr<const ComponentInfo> component_info,
                       std::string group_name,
                       std::string state_name,
                       tesseract_srdf::GroupsJointState state);

  ~GroupJointStatesShow() override;

  const std::string& getGroupName() const;
  const std::string& getStateName() const;
  const tesseract_srdf::GroupsJointState& getJointState() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_SHOW);

private:
  std::string group_name_;
  std::string state_name_;
  tesseract_srdf::GroupsJointState state_;
};

class GroupJointStatesHide : public ComponentEvent
{
public:
  GroupJointStatesHide(std::shared_ptr<const ComponentInfo> component_info,
                       std::string group_name,
                       std::string state_name);

  ~GroupJointStatesHide() override;

  const std::string& getGroupName() const;
  const std::string& getStateName() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_JOINT_STATES_HIDE);

private:
  std::string group_name_;
  std::string state_name_;
};

}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_COMMON_GROUP_JOINT_STATES_EVENTS_H
