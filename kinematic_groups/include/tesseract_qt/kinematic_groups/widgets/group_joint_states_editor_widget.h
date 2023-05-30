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
#ifndef TESSERACT_QT_GROUP_JOINT_STATES_EDITOR_WIDGET_H
#define TESSERACT_QT_GROUP_JOINT_STATES_EDITOR_WIDGET_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_srdf/kinematics_information.h>
#include <tesseract_scene_graph/joint.h>
#include <memory>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QWidget>

class QStringListModel;

namespace Ui
{
class GroupJointStatesEditorWidget;
}

class QItemSelectionModel;

namespace tesseract_gui
{
class ComponentInfo;
class GroupJointStatesModel;
class GroupJointStatesEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit GroupJointStatesEditorWidget(QWidget* parent = nullptr);
  explicit GroupJointStatesEditorWidget(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent = nullptr);
  ~GroupJointStatesEditorWidget();

  void setComponentInfo(std::shared_ptr<const ComponentInfo> component_info);
  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  void setModel(std::shared_ptr<GroupJointStatesModel> model);
  std::shared_ptr<GroupJointStatesModel> getModel();
  std::shared_ptr<const GroupJointStatesModel> getModel() const;

  QItemSelectionModel& getSelectionModel();
  const QItemSelectionModel& getSelectionModel() const;

Q_SIGNALS:
  void jointStateChanged(std::unordered_map<std::string, double> state);

private Q_SLOTS:
  void onGroupNameChanged();
  void onAddJointState();
  void onRemoveJointState();
  void onApply();
  void onUpdateModels();

private:
  struct Implementation;
  std::unique_ptr<Ui::GroupJointStatesEditorWidget> ui_;
  std::unique_ptr<Implementation> data_;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_GROUP_JOINT_STATES_EDITOR_WIDGET_H
