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
#ifndef TESSERACT_QT_JOINT_TRAJECTORY_JOINT_TRAJECTORY_MODEL_H
#define TESSERACT_QT_JOINT_TRAJECTORY_JOINT_TRAJECTORY_MODEL_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <memory>
#include <QStandardItemModel>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

namespace tesseract_common
{
class JointTrajectorySet;
class JointState;
class JointTrajectory;
using JointTrajectoryInfo = std::pair<JointState, JointTrajectory>;
}  // namespace tesseract_common

namespace boost::uuids
{
class uuid;
}

namespace tesseract_gui
{
struct ComponentInfo;
class JointTrajectoryModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit JointTrajectoryModel(QObject* parent = nullptr);
  explicit JointTrajectoryModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent = nullptr);
  ~JointTrajectoryModel() override;

  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  tesseract_common::JointState getJointState(const QModelIndex& row) const;
  tesseract_common::JointTrajectoryInfo getJointTrajectory(const QModelIndex& row) const;
  tesseract_common::JointTrajectorySet getJointTrajectorySet(const QModelIndex& row) const;

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  /**
   * @brief Add joint trajectory set
   * @param trajectory_set The trajectory set associated with the key
   * @return The key associated with added trajectory for removal
   */
  void addJointTrajectorySet(tesseract_common::JointTrajectorySet trajectory_set);

  /**
   * @brief Remove the joint trajectory set
   * @param uuid The uuid associated with the joint trajectory set to be removed
   */
  void removeJointTrajectorySet(const boost::uuids::uuid& uuid);

  /**
   * @brief Check a trajectory set with the provided key exists
   * @param uuid The uuid associated with the joint trajectory set to find
   * @return True if a trajectory exists under the provided key, otherwise false
   */
  bool hasJointTrajectorySet(const boost::uuids::uuid& uuid);

  void clear();

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_JOINT_TRAJECTORY_JOINT_TRAJECTORY_MODEL_H
