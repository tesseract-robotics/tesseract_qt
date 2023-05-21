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
#ifndef TESSERACT_QT_COMMON_KINEMATIC_GROUPS_EVENTS_H
#define TESSERACT_QT_COMMON_KINEMATIC_GROUPS_EVENTS_H

#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>
#include <tesseract_srdf/kinematics_information.h>

namespace tesseract_gui::events
{
class KinematicGroupsClear : public ComponentEvent
{
public:
  KinematicGroupsClear(std::shared_ptr<const ComponentInfo> component_info);
  ~KinematicGroupsClear() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::KINEMATIC_GROUPS_CLEAR);
};

class KinematicGroupsSet : public ComponentEvent
{
public:
  KinematicGroupsSet(std::shared_ptr<const ComponentInfo> component_info,
                     const tesseract_srdf::ChainGroups& chain_groups,
                     const tesseract_srdf::JointGroups& joint_groups,
                     const tesseract_srdf::LinkGroups& link_groups);
  ~KinematicGroupsSet() override;

  const tesseract_srdf::ChainGroups& getChainGroups() const;
  const tesseract_srdf::JointGroups& getJointGroups() const;
  const tesseract_srdf::LinkGroups& getLinkGroups() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::KINEMATIC_GROUPS_SET);

private:
  tesseract_srdf::ChainGroups chain_groups_;
  tesseract_srdf::JointGroups joint_groups_;
  tesseract_srdf::LinkGroups link_groups_;
};

class KinematicGroupsAddChain : public ComponentEvent
{
public:
  KinematicGroupsAddChain(std::shared_ptr<const ComponentInfo> component_info,
                          std::string group_name,
                          tesseract_srdf::ChainGroup group);
  ~KinematicGroupsAddChain() override;

  const std::string& getGroupName() const;
  const tesseract_srdf::ChainGroup& getGroup() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::KINEMATIC_GROUPS_ADD_CHAIN);

private:
  std::string group_name_;
  tesseract_srdf::ChainGroup group_;
};

class KinematicGroupsAddJoint : public ComponentEvent
{
public:
  KinematicGroupsAddJoint(std::shared_ptr<const ComponentInfo> component_info,
                          std::string group_name,
                          tesseract_srdf::JointGroup group);
  ~KinematicGroupsAddJoint() override;

  const std::string& getGroupName() const;
  const tesseract_srdf::JointGroup& getGroup() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::KINEMATIC_GROUPS_ADD_JOINT);

private:
  std::string group_name_;
  tesseract_srdf::JointGroup group_;
};

class KinematicGroupsAddLink : public ComponentEvent
{
public:
  KinematicGroupsAddLink(std::shared_ptr<const ComponentInfo> component_info,
                         std::string group_name,
                         tesseract_srdf::JointGroup group);
  ~KinematicGroupsAddLink() override;

  const std::string& getGroupName() const;
  const tesseract_srdf::LinkGroup& getGroup() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::KINEMATIC_GROUPS_ADD_LINK);

private:
  std::string group_name_;
  tesseract_srdf::LinkGroup group_;
};

class KinematicGroupsRemove : public ComponentEvent
{
public:
  KinematicGroupsRemove(std::shared_ptr<const ComponentInfo> component_info,
                        const std::vector<std::string>& group_names);
  ~KinematicGroupsRemove() override;

  const std::vector<std::string>& getGroupNames() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::KINEMATIC_GROUPS_REMOVE);

private:
  std::vector<std::string> group_names_;
};

}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_COMMON_KINEMATIC_GROUPS_EVENTS_H
