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
#ifndef TESSERACT_QT_KINEMATIC_GROUP_GROUP_TCPS_STANDARD_ITEM_H
#define TESSERACT_QT_KINEMATIC_GROUP_GROUP_TCPS_STANDARD_ITEM_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_srdf/kinematics_information.h>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QStandardItem>

namespace tesseract_gui
{
class GroupTCPsStandardItem : public QStandardItem
{
public:
  GroupTCPsStandardItem();
  explicit GroupTCPsStandardItem(tesseract_srdf::GroupTCPs group_tcps);
  explicit GroupTCPsStandardItem(const QString& text, tesseract_srdf::GroupTCPs group_tcps);
  explicit GroupTCPsStandardItem(const QIcon& icon, const QString& text, tesseract_srdf::GroupTCPs group_tcps);
  int type() const override;

  void addGroupTCP(const QString& group_name, const QString& tcp_name, const Eigen::Isometry3d& tcp);
  void removeGroupTCP(const QString& group_name, const QString& tcp_name);
  void removeGroup(const QString& group_name);

  const tesseract_srdf::GroupTCPs& getGroupTCPs() const;

private:
  void ctor();

  tesseract_srdf::GroupTCPs group_tcps_;
  std::unordered_map<std::string, QStandardItem*> group_items_;

  void addGroupTCPItem(const QString& group_name, const QString& tcp_name, const Eigen::Isometry3d& tcp);
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_KINEMATIC_GROUP_GROUPS_TCPS_STANDARD_ITEM_H
