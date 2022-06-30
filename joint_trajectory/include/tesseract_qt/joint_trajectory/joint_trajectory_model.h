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
#ifndef TESSERACT_QT_TRAJECTORY_JOINT_TRAJECTORY_MODEL_H
#define TESSERACT_QT_TRAJECTORY_JOINT_TRAJECTORY_MODEL_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_common/joint_state.h>
//#include <tesseract_common/joint_trajectory_set.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <QStandardItemModel>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

namespace tesseract_gui
{
class JointTrajectoryModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit JointTrajectoryModel(QObject* parent = nullptr);

  /**
   * @brief Add joint trajectory set
   * @param trajectory_set The trajectory set associated with the key
   * @return The key associated with added trajectory for removal
   */
  QString addJointTrajectorySet(const tesseract_common::JointTrajectorySet& trajectory_set);

  /**
   * @brief Remove the joint trajectory set
   * @param key The key associated with the joint trajectory set to be removed
   */
  void removeJointTrajectorySet(const QString& key);

  /**
   * @brief Check a trajectory set with the provided key exists
   * @param key The key associated with the joint trajectory set to find
   * @return True if a trajectory exists under the provided key, otherwise false
   */
  bool hasJointTrajectorySet(const QString& key);

  const tesseract_common::JointState& getJointState(const QModelIndex& row) const;
  const tesseract_common::JointTrajectoryInfo& getJointTrajectory(const QModelIndex& row) const;
  const tesseract_common::JointTrajectorySet& getJointTrajectorySet(const QModelIndex& row) const;
  std::pair<const QString&, const tesseract_common::JointTrajectorySet&>
  getJointTrajectorySetDetails(const QModelIndex& row) const;

  void clear();

private:
  std::map<QString, QStandardItem*> trajectory_sets_;
};

class JointStateItem : public QStandardItem
{
public:
  JointStateItem(tesseract_common::JointState& state);
  explicit JointStateItem(const QString& text, tesseract_common::JointState& state);
  JointStateItem(const QIcon& icon, const QString& text, tesseract_common::JointState& state);
  int type() const override;

  tesseract_common::JointState& state;

private:
  void ctor();
};

class JointTrajectoryItem : public QStandardItem
{
public:
  JointTrajectoryItem(tesseract_common::JointTrajectoryInfo& trajectory_info);
  explicit JointTrajectoryItem(const QString& text, tesseract_common::JointTrajectoryInfo& trajectory_info);
  JointTrajectoryItem(const QIcon& icon, const QString& text, tesseract_common::JointTrajectoryInfo& trajectory_info);
  int type() const override;

  tesseract_common::JointTrajectoryInfo& trajectory_info;

private:
  void ctor();
};

class JointTrajectorySetItem : public QStandardItem
{
public:
  JointTrajectorySetItem(QString uuid, const tesseract_common::JointTrajectorySet& trajectory_set);
  explicit JointTrajectorySetItem(const QString& text,
                                  QString uuid,
                                  const tesseract_common::JointTrajectorySet& trajectory_set);
  JointTrajectorySetItem(const QIcon& icon,
                         const QString& text,
                         QString uuid,
                         const tesseract_common::JointTrajectorySet& trajectory_set);
  int type() const override;

  QString uuid;
  tesseract_common::JointTrajectorySet trajectory_set;

private:
  void ctor();
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_TRAJECTORY_JOINT_TRAJECTORY_MODEL_H
