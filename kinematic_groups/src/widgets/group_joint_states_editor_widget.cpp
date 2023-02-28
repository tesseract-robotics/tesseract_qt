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
#include <tesseract_qt/kinematic_groups/widgets/group_joint_states_editor_widget.h>
#include <tesseract_qt/kinematic_groups/models/group_joint_states_model.h>

#include <tesseract_qt/common/events/group_joint_states_events.h>
#include <tesseract_qt/common/events/environment_events.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include "ui_group_joint_states_editor_widget.h"

#include <tesseract_environment/environment.h>
#include <tesseract_environment/commands/add_kinematics_information_command.h>

#include <QStringListModel>
#include <QItemSelectionModel>

namespace tesseract_gui
{
struct GroupJointStatesEditorWidget::Implementation
{
  QStringListModel group_names_model;
};

GroupJointStatesEditorWidget::GroupJointStatesEditorWidget(QWidget* parent)
  : GroupJointStatesEditorWidget(ComponentInfo(), parent)
{
}

GroupJointStatesEditorWidget::GroupJointStatesEditorWidget(ComponentInfo component_info, QWidget* parent)
  : QWidget(parent)
  , ui_(std::make_unique<Ui::GroupJointStatesEditorWidget>())
  , data_(std::make_unique<Implementation>())
{
  ui_->setupUi(this);

  ui_->groupJointStatesWidget->setComponentInfo(component_info);

  connect(ui_->groupNamesComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onGroupNameChanged()));
  connect(ui_->groupNamesComboBox, SIGNAL(aboutToShowPopup()), this, SLOT(onUpdateModels()));
  connect(ui_->addPushButton, SIGNAL(clicked()), this, SLOT(onAddJointState()));
  connect(ui_->removePushButton, SIGNAL(clicked()), this, SLOT(onRemoveJointState()));
  connect(ui_->applyPushButton, SIGNAL(clicked()), this, SLOT(onApply()));
  connect(ui_->jointSliderWidget,
          &tesseract_gui::JointStateSliderWidget::jointStateChanged,
          this,
          &tesseract_gui::GroupJointStatesEditorWidget::jointStateChanged);

  onUpdateModels();
  onGroupNameChanged();
}

GroupJointStatesEditorWidget::~GroupJointStatesEditorWidget() = default;

void GroupJointStatesEditorWidget::setComponentInfo(ComponentInfo component_info)
{
  ui_->groupJointStatesWidget->setComponentInfo(component_info);
  onUpdateModels();
}

const ComponentInfo& GroupJointStatesEditorWidget::getComponentInfo() const
{
  return ui_->groupJointStatesWidget->getComponentInfo();
}

void GroupJointStatesEditorWidget::setModel(std::shared_ptr<GroupJointStatesModel> model)
{
  ui_->groupJointStatesWidget->setModel(model);
  onUpdateModels();
}
std::shared_ptr<GroupJointStatesModel> GroupJointStatesEditorWidget::getModel()
{
  return ui_->groupJointStatesWidget->getModel();
}
std::shared_ptr<const GroupJointStatesModel> GroupJointStatesEditorWidget::getModel() const
{
  return ui_->groupJointStatesWidget->getModel();
}

QItemSelectionModel& GroupJointStatesEditorWidget::getSelectionModel()
{
  return ui_->groupJointStatesWidget->getSelectionModel();
}
const QItemSelectionModel& GroupJointStatesEditorWidget::getSelectionModel() const
{
  return ui_->groupJointStatesWidget->getSelectionModel();
}

void GroupJointStatesEditorWidget::onGroupNameChanged()
{
  auto env_wrapper = EnvironmentManager::get(ComponentInfo());
  if (env_wrapper == nullptr)
    return;

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
    return;

  std::vector<tesseract_scene_graph::Joint::ConstPtr> joints;
  auto jg = env->getJointGroup(ui_->groupNamesComboBox->currentText().toStdString());
  for (const auto& joint_name : jg->getJointNames())
    joints.push_back(env->getSceneGraph()->getJoint(joint_name));

  ui_->jointSliderWidget->setJoints(joints);
}

void GroupJointStatesEditorWidget::onAddJointState()
{
  std::string group_name = ui_->groupNamesComboBox->currentText().toStdString();
  std::string state_name = ui_->jointStateNameLineEdit->text().toStdString();
  if (state_name.empty())
    return;

  QApplication::sendEvent(
      qApp,
      new tesseract_gui::events::GroupJointStatesAdd(ui_->groupJointStatesWidget->getComponentInfo(),
                                                     group_name,
                                                     state_name,
                                                     ui_->jointSliderWidget->getJointState()));

  ui_->jointStateNameLineEdit->clear();
}

void GroupJointStatesEditorWidget::onRemoveJointState()
{
  QModelIndexList selection = ui_->groupJointStatesWidget->getSelectionModel().selectedIndexes();
  int row_cnt = selection.count();

  auto model = ui_->groupJointStatesWidget->getModel();
  std::vector<std::array<std::string, 2>> remove_items;
  remove_items.reserve(row_cnt);
  for (int i = row_cnt; i > 0; i--)
  {
    QStandardItem* item = model->itemFromIndex(selection.at(i - 1));
    if (item->type() == static_cast<int>(StandardItemType::SRDF_GROUP_JOINT_STATE))
      remove_items.push_back({ item->parent()->text().toStdString(), item->text().toStdString() });
  }

  if (!remove_items.empty())
    QApplication::sendEvent(qApp,
                            new tesseract_gui::events::GroupJointStatesRemove(model->getComponentInfo(), remove_items));
}

void GroupJointStatesEditorWidget::onApply()
{
  tesseract_srdf::KinematicsInformation info;
  info.group_states = ui_->groupJointStatesWidget->getModel()->getGroupsJointStates();

  auto cmd = std::make_shared<tesseract_environment::AddKinematicsInformationCommand>(info);
  QApplication::sendEvent(qApp, new events::EnvironmentApplyCommand(getComponentInfo(), { cmd }));
}

void GroupJointStatesEditorWidget::onUpdateModels()
{
  QStringList list;
  data_->group_names_model.setStringList(list);

  auto env_wrapper = EnvironmentManager::get(tesseract_gui::ComponentInfo());
  if (env_wrapper == nullptr)
    return;

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
    return;

  for (const auto& group_name : env->getGroupNames())
    list.append(QString::fromStdString(group_name));

  data_->group_names_model.setStringList(list);
}

}  // namespace tesseract_gui
