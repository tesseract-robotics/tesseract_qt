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
#ifndef TESSERACT_QT_KINEMATIC_GROUP_GROUP_JOINT_STATES_STANDARD_ITEM_H
#define TESSERACT_QT_KINEMATIC_GROUP_GROUP_JOINT_STATES_STANDARD_ITEM_H
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_srdf/kinematics_information.h>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QStandardItem>

namespace tesseract_gui
{
class GroupJointStatesStandardItem : public QStandardItem
{
public:
  GroupJointStatesStandardItem();
  explicit GroupJointStatesStandardItem(const tesseract_srdf::GroupJointStates& group_joint_states);
  explicit GroupJointStatesStandardItem(const QString& text,
                                        const tesseract_srdf::GroupJointStates& group_joint_states);
  explicit GroupJointStatesStandardItem(const QIcon& icon,
                                        const QString& text,
                                        const tesseract_srdf::GroupJointStates& group_joint_states);
  int type() const override;

  void addGroupJointState(const QString& group_name,
                          const QString& state_name,
                          const tesseract_srdf::GroupsJointState& state);
  void removeGroupJointState(const QString& group_name, const QString& state_name);
  void removeGroup(const QString& group_name);

  tesseract_srdf::GroupJointStates getGroupJointStates() const;

private:
  void ctor(const tesseract_srdf::GroupJointStates& group_joint_states);

  std::unordered_map<std::string, QStandardItem*> group_items_;

  void addGroupJointStateItem(const QString& group_name,
                              const QString& tcp_name,
                              const tesseract_srdf::GroupsJointState& state);
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_KINEMATIC_GROUP_GROUP_JOINT_STATES_STANDARD_ITEM_H
