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
#ifndef TESSERACT_QT_GROUPS_JOINT_STATES_MODEL_H
#define TESSERACT_QT_GROUPS_JOINT_STATES_MODEL_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_srdf/kinematics_information.h>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QStandardItemModel>
#include <tesseract_qt/kinematic_groups/group_joint_states_standard_item.h>

class QModelIndex;

namespace tesseract_gui
{
class GroupJointStatesModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit GroupJointStatesModel(QObject* parent = nullptr);
  GroupJointStatesModel(const GroupJointStatesModel& other);
  GroupJointStatesModel& operator=(const GroupJointStatesModel& other);

  void set(const tesseract_srdf::GroupJointStates& group_joint_states);
  void addGroupJointState(QString group_name, QString state_name, tesseract_srdf::GroupsJointState state);
  void removeGroupJointState(QString group_name, QString state_name);
  void clear();

  const tesseract_srdf::GroupJointStates& getGroupsJointStates() const;

  const tesseract_srdf::GroupsJointState& getGroupsJointState(const QModelIndex& row) const;

private:
  GroupJointStatesStandardItem* getRoot();
  const GroupJointStatesStandardItem* getRoot() const;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_GROUPS_JOINT_STATES_MODEL_H
