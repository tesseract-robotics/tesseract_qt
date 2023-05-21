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
#ifndef TESSERACT_QT_COMMON_GROUP_TCPS_EVENTS_H
#define TESSERACT_QT_COMMON_GROUP_TCPS_EVENTS_H

#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>
#include <tesseract_srdf/kinematics_information.h>

namespace tesseract_gui::events
{
class GroupTCPsClear : public ComponentEvent
{
public:
  GroupTCPsClear(std::shared_ptr<const ComponentInfo> component_info);
  ~GroupTCPsClear() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_TCPS_CLEAR);
};

class GroupTCPsSet : public ComponentEvent
{
public:
  GroupTCPsSet(std::shared_ptr<const ComponentInfo> component_info, const tesseract_srdf::GroupTCPs& group_tcps);
  ~GroupTCPsSet() override;

  const tesseract_srdf::GroupTCPs& getGroupTCPs() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_TCPS_SET);

private:
  tesseract_srdf::GroupTCPs group_tcps_;
};

class GroupTCPsAdd : public ComponentEvent
{
public:
  // LCOV_EXCL_START
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  // LCOV_EXCL_STOP

  GroupTCPsAdd(std::shared_ptr<const ComponentInfo> component_info,
               std::string group_name,
               std::string tcp_name,
               const Eigen::Isometry3d& tcp);
  ~GroupTCPsAdd() override;

  const std::string& getGroupName() const;
  const std::string& getTCPName() const;
  const Eigen::Isometry3d& getTCP() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_TCPS_ADD);

private:
  std::string group_name_;
  std::string tcp_name_;
  Eigen::Isometry3d tcp_;
};

class GroupTCPsRemove : public ComponentEvent
{
public:
  GroupTCPsRemove(std::shared_ptr<const ComponentInfo> component_info,
                  const std::vector<std::array<std::string, 2>>& entries);
  ~GroupTCPsRemove() override;

  const std::vector<std::array<std::string, 2>>& getEntries() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_TCPS_REMOVE);

private:
  std::vector<std::array<std::string, 2>> entries_;
};

class GroupTCPsRemoveGroup : public ComponentEvent
{
public:
  GroupTCPsRemoveGroup(std::shared_ptr<const ComponentInfo> component_info,
                       const std::vector<std::string>& group_names);
  ~GroupTCPsRemoveGroup() override;

  const std::vector<std::string>& getGroupNames() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::GROUP_TCPS_REMOVE_GROUP);

private:
  std::vector<std::string> group_names_;
};

// class GroupJointStatesShow : public SceneEvent
//{
// public:
//  GroupJointStatesShow(ComponentInfo component_info, QString group_name, QString state_name,
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
#endif  // TESSERACT_QT_COMMON_GROUP_TCPS_EVENTS_H
