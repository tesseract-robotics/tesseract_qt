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
#ifndef TESSERACT_QT_KINEMATIC_GROUP_EDITOR_WIDGET_H
#define TESSERACT_QT_KINEMATIC_GROUP_EDITOR_WIDGET_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_srdf/kinematics_information.h>
#include <memory>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QWidget>
#include <QStandardItemModel>

class QStringListModel;

namespace Ui
{
class KinematicGroupsEditorWidget;
}

namespace tesseract_gui
{
class KinematicGroupsModel;

using ChainGroupValidator = std::function<bool(QString, QString)>;
using JointGroupValidator = std::function<bool(QStringList)>;
using LinkGroupValidator = std::function<bool(QStringList)>;

struct KinematicGroupsEditorWidgetImpl;
class KinematicGroupsEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit KinematicGroupsEditorWidget(QWidget* parent = nullptr);
  ~KinematicGroupsEditorWidget();

  void setValidators(ChainGroupValidator chain_group_validator,
                     JointGroupValidator joint_group_validator,
                     LinkGroupValidator link_group_validator);

  void setModels(KinematicGroupsModel* kin_groups_model,
                 QStringListModel* link_names_model,
                 QStringListModel* joint_names_model);

public Q_SLOTS:
  void onAddGroup();
  void onRemoveGroup();
  void onAddJoint();
  void onRemoveJoint();
  void onAddLink();
  void onRemoveLink();

private:
  std::unique_ptr<Ui::KinematicGroupsEditorWidget> ui_;
  std::unique_ptr<KinematicGroupsEditorWidgetImpl> data_;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_KINEMATIC_GROUP_EDITOR_WIDGET_H
