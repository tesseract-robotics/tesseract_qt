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
#include <tesseract_qt/common/events/joint_trajectory_events.h>
#include <tesseract_qt/common/joint_trajectory_set.h>

#include <string>
#include <boost/uuid/uuid.hpp>

#include <QString>

namespace tesseract_gui::events
{
class JointTrajectoryAdd::Implementation
{
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  tesseract_common::JointTrajectorySet joint_trajectory_set;
  bool clear_namespace{ false };
};

JointTrajectoryAdd::JointTrajectoryAdd(std::shared_ptr<const ComponentInfo> component_info,
                                       const tesseract_common::JointTrajectorySet& joint_trajectory_set,
                                       bool clear_namespace)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::JOINT_TRAJECTORY_ADD))
  , data_(std::make_unique<Implementation>())
{
  data_->joint_trajectory_set = joint_trajectory_set;
  data_->clear_namespace = clear_namespace;
}
JointTrajectoryAdd::JointTrajectoryAdd(const JointTrajectoryAdd& other)
  : JointTrajectoryAdd(other.getComponentInfo(), other.getJointTrajectory(), other.clearNamespace())
{
}
JointTrajectoryAdd::~JointTrajectoryAdd() = default;

const tesseract_common::JointTrajectorySet& JointTrajectoryAdd::getJointTrajectory() const
{
  return data_->joint_trajectory_set;
}

bool JointTrajectoryAdd::clearNamespace() const { return data_->clear_namespace; }

//////////////////////////////////////////

JointTrajectoryRemove::JointTrajectoryRemove(std::shared_ptr<const ComponentInfo> component_info,
                                             const boost::uuids::uuid& uuid)
  : ComponentEventUUID(std::move(component_info), uuid, QEvent::Type(EventType::JOINT_TRAJECTORY_REMOVE))
{
}
JointTrajectoryRemove::JointTrajectoryRemove(const JointTrajectoryRemove& other)
  : JointTrajectoryRemove(other.getComponentInfo(), other.getUUID())
{
}
JointTrajectoryRemove::~JointTrajectoryRemove() = default;

//////////////////////////////////////////

JointTrajectoryRemoveNamespace::JointTrajectoryRemoveNamespace(std::shared_ptr<const ComponentInfo> component_info,
                                                               std::string ns)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::JOINT_TRAJECTORY_REMOVE_NAMESPACE))
  , ns_(std::move(ns))
{
}
JointTrajectoryRemoveNamespace::JointTrajectoryRemoveNamespace(const JointTrajectoryRemoveNamespace& other)
  : JointTrajectoryRemoveNamespace(other.getComponentInfo(), other.ns_)
{
}
JointTrajectoryRemoveNamespace::~JointTrajectoryRemoveNamespace() = default;

const std::string& JointTrajectoryRemoveNamespace::getNamespace() const { return ns_; }

//////////////////////////////////////////

JointTrajectoryRemoveAll::JointTrajectoryRemoveAll(std::shared_ptr<const ComponentInfo> component_info)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::JOINT_TRAJECTORY_REMOVE_ALL))
{
}
JointTrajectoryRemoveAll::JointTrajectoryRemoveAll(const JointTrajectoryRemoveAll& other)
  : JointTrajectoryRemoveAll(other.getComponentInfo())
{
}
JointTrajectoryRemoveAll::~JointTrajectoryRemoveAll() = default;

//////////////////////////////////////////

JointTrajectoryRemoveSelected::JointTrajectoryRemoveSelected(std::shared_ptr<const ComponentInfo> component_info)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::JOINT_TRAJECTORY_REMOVE_SELECTED))
{
}
JointTrajectoryRemoveSelected::JointTrajectoryRemoveSelected(const JointTrajectoryRemoveSelected& other)
  : JointTrajectoryRemoveSelected(other.getComponentInfo())
{
}
JointTrajectoryRemoveSelected::~JointTrajectoryRemoveSelected() = default;

//////////////////////////////////////////

JointTrajectoryOpen::JointTrajectoryOpen(std::shared_ptr<const ComponentInfo> component_info)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::JOINT_TRAJECTORY_OPEN))
{
}
JointTrajectoryOpen::JointTrajectoryOpen(const JointTrajectoryOpen& other)
  : JointTrajectoryOpen(other.getComponentInfo())
{
}
JointTrajectoryOpen::~JointTrajectoryOpen() = default;

//////////////////////////////////////////

JointTrajectorySave::JointTrajectorySave(std::shared_ptr<const ComponentInfo> component_info)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::JOINT_TRAJECTORY_SAVE))
{
}
JointTrajectorySave::JointTrajectorySave(const JointTrajectorySave& other)
  : JointTrajectorySave(other.getComponentInfo())
{
}
JointTrajectorySave::~JointTrajectorySave() = default;

//////////////////////////////////////////

JointTrajectoryPlot::JointTrajectoryPlot(std::shared_ptr<const ComponentInfo> component_info)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::JOINT_TRAJECTORY_PLOT))
{
}
JointTrajectoryPlot::JointTrajectoryPlot(const JointTrajectoryPlot& other)
  : JointTrajectoryPlot(other.getComponentInfo())
{
}
JointTrajectoryPlot::~JointTrajectoryPlot() = default;

//////////////////////////////////////////

JointTrajectoryToolbarState::JointTrajectoryToolbarState(std::shared_ptr<const ComponentInfo> component_info)
  : ComponentEvent(std::move(component_info), QEvent::Type(EventType::JOINT_TRAJECTORY_TOOLBAR_STATE))
{
}
JointTrajectoryToolbarState::JointTrajectoryToolbarState(const JointTrajectoryToolbarState& other)
  : JointTrajectoryToolbarState(other.getComponentInfo())
{
}
JointTrajectoryToolbarState::~JointTrajectoryToolbarState() = default;
}  // namespace tesseract_gui::events
