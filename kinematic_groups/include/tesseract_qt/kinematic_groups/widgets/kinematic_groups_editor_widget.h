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

class QItemSelectionModel;

namespace tesseract_gui
{
class ComponentInfo;
class KinematicGroupsModel;
class KinematicGroupsEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit KinematicGroupsEditorWidget(QWidget* parent = nullptr);
  explicit KinematicGroupsEditorWidget(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent = nullptr);
  ~KinematicGroupsEditorWidget();

  void setComponentInfo(std::shared_ptr<const ComponentInfo> component_info);
  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  void setModel(std::shared_ptr<KinematicGroupsModel> model);
  std::shared_ptr<KinematicGroupsModel> getModel();
  std::shared_ptr<const KinematicGroupsModel> getModel() const;

  QItemSelectionModel& getSelectionModel();
  const QItemSelectionModel& getSelectionModel() const;

public Q_SLOTS:
  void onAddGroup();
  void onRemoveGroup();
  void onAddJoint();
  void onRemoveJoint();
  void onAddLink();
  void onRemoveLink();
  void onApply();
  void onUpdateLinkNamesModel();
  void onUpdateJointNamesModel();

private:
  struct Implementation;
  std::unique_ptr<Ui::KinematicGroupsEditorWidget> ui_;
  std::unique_ptr<Implementation> data_;

  bool validateChainGroup(QString base_link, QString tip_link);
  bool validateJointChain(QStringList joints);
  bool validateLinkChain(QStringList links);
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_KINEMATIC_GROUP_EDITOR_WIDGET_H
