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
#ifndef TESSERACT_QT_COMMON_JOINT_TRAJECTORY_EVENTS_H
#define TESSERACT_QT_COMMON_JOINT_TRAJECTORY_EVENTS_H

#include <memory>
#include <tesseract_qt/common/events/event_type.h>
#include <tesseract_qt/common/events/component_events.h>

namespace tesseract_common
{
class JointTrajectorySet;
}

namespace tesseract_gui::events
{
/** @brief Event called to add a joint trajectory */
class JointTrajectoryAdd : public ComponentEvent
{
public:
  JointTrajectoryAdd(std::shared_ptr<const ComponentInfo> component_info,
                     const tesseract_common::JointTrajectorySet& joint_trajectory);
  JointTrajectoryAdd(const JointTrajectoryAdd& other);
  ~JointTrajectoryAdd() override;

  const tesseract_common::JointTrajectorySet& getJointTrajectory() const;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::JOINT_TRAJECTORY_ADD);

private:
  /** @brief Private data pointer */
  class Implementation;
  std::unique_ptr<Implementation> data_;
};

/** @brief Event called to remove a joint trajectory */
class JointTrajectoryRemove : public ComponentEventUUID
{
public:
  JointTrajectoryRemove(std::shared_ptr<const ComponentInfo> component_info, const boost::uuids::uuid& uuid);
  JointTrajectoryRemove(const JointTrajectoryRemove& other);
  ~JointTrajectoryRemove() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::JOINT_TRAJECTORY_REMOVE);
};

/** @brief Event called to remove all a joint trajectories for a given scene */
class JointTrajectoryRemoveAll : public ComponentEvent
{
public:
  JointTrajectoryRemoveAll(std::shared_ptr<const ComponentInfo> component_info);
  JointTrajectoryRemoveAll(const JointTrajectoryRemoveAll& other);
  ~JointTrajectoryRemoveAll() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::JOINT_TRAJECTORY_REMOVE_ALL);
};

/** @brief Event called to remove selected joint trajectories for a given scene */
class JointTrajectoryRemoveSelected : public ComponentEvent
{
public:
  JointTrajectoryRemoveSelected(std::shared_ptr<const ComponentInfo> component_info);
  JointTrajectoryRemoveSelected(const JointTrajectoryRemoveSelected& other);
  ~JointTrajectoryRemoveSelected() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::JOINT_TRAJECTORY_REMOVE_SELECTED);
};

/** @brief Event called to open a joint trajectories for a given scene */
class JointTrajectoryOpen : public ComponentEvent
{
public:
  JointTrajectoryOpen(std::shared_ptr<const ComponentInfo> component_info);
  JointTrajectoryOpen(const JointTrajectoryOpen& other);
  ~JointTrajectoryOpen() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::JOINT_TRAJECTORY_OPEN);
};

/** @brief Event called to save a joint trajectories for a given scene */
class JointTrajectorySave : public ComponentEvent
{
public:
  JointTrajectorySave(std::shared_ptr<const ComponentInfo> component_info);
  JointTrajectorySave(const JointTrajectorySave& other);
  ~JointTrajectorySave() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::JOINT_TRAJECTORY_SAVE);
};

/** @brief Event called to save a joint trajectories for a given scene */
class JointTrajectoryPlot : public ComponentEvent
{
public:
  JointTrajectoryPlot(std::shared_ptr<const ComponentInfo> component_info);
  JointTrajectoryPlot(const JointTrajectoryPlot& other);
  ~JointTrajectoryPlot() override;

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::JOINT_TRAJECTORY_PLOT);
};

/** @brief Event called to modify toolbare state */
class JointTrajectoryToolbarState : public ComponentEvent
{
public:
  JointTrajectoryToolbarState(std::shared_ptr<const ComponentInfo> component_info);
  JointTrajectoryToolbarState(const JointTrajectoryToolbarState& other);
  ~JointTrajectoryToolbarState() override;

  bool remove_all_enabled{ true };
  bool remove_enabled{ true };
  bool open_enabled{ true };
  bool save_enabled{ true };
  bool plot_enabled{ true };

  /** @brief Unique type for this event. */
  static const QEvent::Type kType = QEvent::Type(EventType::JOINT_TRAJECTORY_TOOLBAR_STATE);
};
}  // namespace tesseract_gui::events
#endif  // TESSERACT_QT_COMMON_JOINT_TRAJECTORY_EVENTS_H
