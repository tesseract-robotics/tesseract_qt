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
#ifndef TESSERACT_QT_KINEMATIC_GROUP_MODEL_H
#define TESSERACT_QT_KINEMATIC_GROUP_MODEL_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_srdf/kinematics_information.h>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QStandardItem>

namespace tesseract_gui
{
class KinematicGroupsModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit KinematicGroupsModel(QObject* parent = nullptr);
  KinematicGroupsModel(const KinematicGroupsModel& other);
  KinematicGroupsModel& operator=(const KinematicGroupsModel& other);

  void set(const tesseract_srdf::ChainGroups& chain_groups,
           const tesseract_srdf::JointGroups& joint_groups,
           const tesseract_srdf::LinkGroups& link_groups);
  void addChainGroup(QString group_name, tesseract_srdf::ChainGroup group);
  void addJointGroup(QString group_name, tesseract_srdf::JointGroup group);
  void addLinkGroup(QString group_name, tesseract_srdf::LinkGroup group);

  void removeGroup(QString group_name);
  void clear();

  const tesseract_srdf::GroupNames& getGroupNames() const;
  const tesseract_srdf::ChainGroups& getChainGroups() const;
  const tesseract_srdf::JointGroups& getJointGroups() const;
  const tesseract_srdf::LinkGroups& getLinkGroups() const;

Q_SIGNALS:
  void groupRemoved(QString group_name);
  void groupAdded(QString group_name);
  void cleared();

private:
  tesseract_srdf::GroupNames group_names_;
  tesseract_srdf::ChainGroups chain_groups_;
  tesseract_srdf::JointGroups joint_groups_;
  tesseract_srdf::LinkGroups link_groups_;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_KINEMATIC_GROUP_MODEL_H
