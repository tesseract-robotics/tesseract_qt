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
#ifndef TESSERACT_QT_GROUPS_JOINT_STATES_EDITOR_WIDGET_H
#define TESSERACT_QT_GROUPS_JOINT_STATES_EDITOR_WIDGET_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <tesseract_srdf/kinematics_information.h>
#include <tesseract_scene_graph/joint.h>
#include <memory>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QWidget>
#include <QStringListModel>

namespace Ui
{
class GroupsJointStatesEditorWidget;
}

namespace tesseract_gui
{
using GroupJointsRetriever = std::function<std::vector<tesseract_scene_graph::Joint::ConstPtr>(QString)>;

class GroupJointStatesModel;
struct GroupsJointStatesEditorWidgetImpl;
class GroupsJointStatesEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit GroupsJointStatesEditorWidget(GroupJointsRetriever group_joints_retriever, QWidget* parent = nullptr);
  ~GroupsJointStatesEditorWidget();

  void setModel(GroupJointStatesModel* model);
  void setGroupNamesModel(QStringListModel* model);

Q_SIGNALS:
  void jointStateChanged(std::unordered_map<std::string, double> state);

private Q_SLOTS:
  void onGroupNameChanged();
  void onAddJointState();
  void onRemoveJointState();

private:
  std::unique_ptr<Ui::GroupsJointStatesEditorWidget> ui_;
  std::unique_ptr<GroupsJointStatesEditorWidgetImpl> data_;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_GROUPS_JOINT_STATES_EDITOR_WIDGET_H
