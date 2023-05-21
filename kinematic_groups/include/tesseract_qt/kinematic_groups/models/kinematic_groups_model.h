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
struct ComponentInfo;
class KinematicGroupsModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit KinematicGroupsModel(QObject* parent = nullptr);
  explicit KinematicGroupsModel(std::shared_ptr<const ComponentInfo> component_info, QObject* parent = nullptr);
  KinematicGroupsModel(const KinematicGroupsModel& other);
  KinematicGroupsModel& operator=(const KinematicGroupsModel& other);
  ~KinematicGroupsModel() override;

  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  const tesseract_srdf::GroupNames& getGroupNames() const;
  const tesseract_srdf::ChainGroups& getChainGroups() const;
  const tesseract_srdf::JointGroups& getJointGroups() const;
  const tesseract_srdf::LinkGroups& getLinkGroups() const;

Q_SIGNALS:
  void groupRemoved(QString group_name);
  void groupAdded(QString group_name);
  void cleared();

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  void set(const tesseract_srdf::ChainGroups& chain_groups,
           const tesseract_srdf::JointGroups& joint_groups,
           const tesseract_srdf::LinkGroups& link_groups);
  void addChainGroup(const std::string& group_name, const tesseract_srdf::ChainGroup& group);
  void addJointGroup(const std::string& group_name, const tesseract_srdf::JointGroup& group);
  void addLinkGroup(const std::string& group_name, const tesseract_srdf::LinkGroup& group);

  void removeGroup(const std::string& group_name);
  void clear();

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_KINEMATIC_GROUP_MODEL_H
