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
#include <tesseract_qt/kinematic_groups/widgets/kinematic_groups_editor_widget.h>
#include <tesseract_qt/kinematic_groups/models/kinematic_groups_model.h>
#include <tesseract_qt/common/events/kinematic_groups_events.h>
#include <tesseract_qt/common/events/environment_events.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include "ui_kinematic_groups_editor_widget.h"

#include <tesseract_environment/environment.h>
#include <tesseract_environment/commands/add_kinematics_information_command.h>

#include <QStringListModel>
#include <QApplication>

namespace tesseract_gui
{
struct KinematicGroupsEditorWidget::Implementation
{
  QStringListModel link_names_model;
  QStringListModel joint_names_model;
};

KinematicGroupsEditorWidget::KinematicGroupsEditorWidget(QWidget* parent)
  : KinematicGroupsEditorWidget(ComponentInfo(), parent)
{
}

KinematicGroupsEditorWidget::KinematicGroupsEditorWidget(ComponentInfo component_info, QWidget* parent)
  : QWidget(parent), ui_(std::make_unique<Ui::KinematicGroupsEditorWidget>()), data_(std::make_unique<Implementation>())
{
  ui_->setupUi(this);

  ui_->kinGroupsWidget->setComponentInfo(std::move(component_info));

  ui_->baseLinkNameComboBox->setModel(&data_->link_names_model);
  ui_->tipLinkNameComboBox->setModel(&data_->link_names_model);
  ui_->linkComboBox->setModel(&data_->link_names_model);
  ui_->jointComboBox->setModel(&data_->joint_names_model);

  connect(ui_->addGroupPushButton, SIGNAL(clicked()), this, SLOT(onAddGroup()));
  connect(ui_->removeGroupPushButton, SIGNAL(clicked()), this, SLOT(onRemoveGroup()));

  connect(ui_->addJointPushButton, SIGNAL(clicked()), this, SLOT(onAddJoint()));
  connect(ui_->removeJointPushButton, SIGNAL(clicked()), this, SLOT(onRemoveJoint()));

  connect(ui_->addLinkPushButton, SIGNAL(clicked()), this, SLOT(onAddLink()));
  connect(ui_->removeLinkPushButton, SIGNAL(clicked()), this, SLOT(onRemoveLink()));

  connect(ui_->applyPushButton, SIGNAL(clicked()), this, SLOT(onApply()));

  onUpdateJointNamesModel();
  onUpdateLinkNamesModel();
}

KinematicGroupsEditorWidget::~KinematicGroupsEditorWidget() = default;

void KinematicGroupsEditorWidget::setComponentInfo(ComponentInfo component_info)
{
  ui_->kinGroupsWidget->setComponentInfo(component_info);
  onUpdateJointNamesModel();
  onUpdateLinkNamesModel();
}

const ComponentInfo& KinematicGroupsEditorWidget::getComponentInfo() const
{
  return ui_->kinGroupsWidget->getComponentInfo();
}

void KinematicGroupsEditorWidget::setModel(std::shared_ptr<KinematicGroupsModel> model)
{
  ui_->kinGroupsWidget->setModel(model);
  onUpdateJointNamesModel();
  onUpdateLinkNamesModel();
}
std::shared_ptr<KinematicGroupsModel> KinematicGroupsEditorWidget::getModel()
{
  return ui_->kinGroupsWidget->getModel();
}
std::shared_ptr<const KinematicGroupsModel> KinematicGroupsEditorWidget::getModel() const
{
  return ui_->kinGroupsWidget->getModel();
}

QItemSelectionModel& KinematicGroupsEditorWidget::getSelectionModel()
{
  return ui_->kinGroupsWidget->getSelectionModel();
}
const QItemSelectionModel& KinematicGroupsEditorWidget::getSelectionModel() const
{
  return ui_->kinGroupsWidget->getSelectionModel();
}

void KinematicGroupsEditorWidget::onAddGroup()
{
  std::string group_name = ui_->groupNameLineEdit->text().toStdString();
  if (group_name.empty())
    return;

  if (ui_->kinGroupTabWidget->currentIndex() == 0)
  {
    QString base_link = ui_->baseLinkNameComboBox->currentText();
    QString tip_link = ui_->tipLinkNameComboBox->currentText();
    if (base_link == tip_link || base_link.isEmpty() || tip_link.isEmpty())
      return;

    if (!validateChainGroup(base_link, tip_link))
      return;

    tesseract_srdf::ChainGroup group{ { base_link.toStdString(), tip_link.toStdString() } };

    QApplication::sendEvent(qApp, new events::KinematicGroupsAddChain(getComponentInfo(), group_name, group));

    ui_->groupNameLineEdit->clear();
    return;
  }
  else if (ui_->kinGroupTabWidget->currentIndex() == 1)
  {
    QStringList joints;
    for (int i = 0; i < ui_->jointListWidget->count(); ++i)
      joints.append(ui_->jointListWidget->item(i)->text());

    if (!validateJointChain(joints))
      return;

    tesseract_srdf::JointGroup group;
    for (auto& joint : joints)
      group.push_back(joint.toStdString());

    QApplication::sendEvent(qApp, new events::KinematicGroupsAddJoint(getComponentInfo(), group_name, group));

    ui_->groupNameLineEdit->clear();
    ui_->jointListWidget->clear();
    return;
  }
  else if (ui_->kinGroupTabWidget->currentIndex() == 2)
  {
    QStringList links;
    for (int i = 0; i < ui_->linkListWidget->count(); ++i)
      links.append(ui_->linkListWidget->item(i)->text());

    if (!validateLinkChain(links))
      return;

    tesseract_srdf::LinkGroup group;
    for (auto& link : links)
      group.push_back(link.toStdString());

    QApplication::sendEvent(qApp, new events::KinematicGroupsAddLink(getComponentInfo(), group_name, group));

    ui_->groupNameLineEdit->clear();
    ui_->linkListWidget->clear();
    return;
  }
}

void KinematicGroupsEditorWidget::onRemoveGroup()
{
  QModelIndexList selection = getSelectionModel().selectedIndexes();
  auto model = getModel();
  int row_cnt = selection.count();
  std::vector<std::string> remove_groups;
  remove_groups.reserve(row_cnt);
  for (int i = row_cnt; i > 0; i--)
  {
    QStandardItem* item = model->itemFromIndex(selection.at(i - 1));
    if (item->type() == static_cast<int>(StandardItemType::SRDF_CHAIN_GROUP) ||
        item->type() == static_cast<int>(StandardItemType::SRDF_JOINT_GROUP) ||
        item->type() == static_cast<int>(StandardItemType::SRDF_LINK_GROUP))
    {
      remove_groups.push_back(item->text().toStdString());
    }
  }

  QApplication::sendEvent(qApp, new events::KinematicGroupsRemove(getComponentInfo(), remove_groups));
}

void KinematicGroupsEditorWidget::onAddJoint() { ui_->jointListWidget->addItem(ui_->jointComboBox->currentText()); }

void KinematicGroupsEditorWidget::onRemoveJoint()
{
  QList<QListWidgetItem*> items = ui_->jointListWidget->selectedItems();
  for (auto* item : items)
  {
    ui_->jointListWidget->takeItem(ui_->jointListWidget->row(item));
    delete item;
  }
}

void KinematicGroupsEditorWidget::onAddLink() { ui_->linkListWidget->addItem(ui_->linkComboBox->currentText()); }

void KinematicGroupsEditorWidget::onRemoveLink()
{
  QList<QListWidgetItem*> items = ui_->linkListWidget->selectedItems();
  for (auto* item : items)
  {
    ui_->linkListWidget->takeItem(ui_->linkListWidget->row(item));
    delete item;
  }
}

void KinematicGroupsEditorWidget::onApply()
{
  tesseract_srdf::KinematicsInformation info;
  info.group_names = ui_->kinGroupsWidget->getModel()->getGroupNames();
  info.chain_groups = ui_->kinGroupsWidget->getModel()->getChainGroups();
  info.joint_groups = ui_->kinGroupsWidget->getModel()->getJointGroups();
  info.link_groups = ui_->kinGroupsWidget->getModel()->getLinkGroups();

  auto cmd = std::make_shared<tesseract_environment::AddKinematicsInformationCommand>(info);
  QApplication::sendEvent(qApp, new events::EnvironmentApplyCommand(getComponentInfo(), { cmd }));
}

void KinematicGroupsEditorWidget::onUpdateLinkNamesModel()
{
  QStringList list;
  data_->link_names_model.setStringList(list);

  auto env_wrapper = EnvironmentManager::get(getComponentInfo());
  if (env_wrapper == nullptr)
    return;

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
    return;

  for (const auto& link_name : env->getLinkNames())
    list.append(QString::fromStdString(link_name));

  data_->link_names_model.setStringList(list);
}

void KinematicGroupsEditorWidget::onUpdateJointNamesModel()
{
  QStringList list;
  data_->joint_names_model.setStringList(list);

  auto env_wrapper = EnvironmentManager::get(getComponentInfo());
  if (env_wrapper == nullptr)
    return;

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
    return;

  for (const auto& link_name : env->getActiveJointNames())
    list.append(QString::fromStdString(link_name));

  data_->joint_names_model.setStringList(list);
}

bool KinematicGroupsEditorWidget::validateChainGroup(QString base_link, QString tip_link)
{
  auto env_wrapper = EnvironmentManager::get(getComponentInfo());
  if (env_wrapper == nullptr)
    return true;

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
    return true;

  /** @todo add check */

  return true;
}

bool KinematicGroupsEditorWidget::validateJointChain(QStringList joints)
{
  auto env_wrapper = EnvironmentManager::get(getComponentInfo());
  if (env_wrapper == nullptr)
    return true;

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
    return true;

  /** @todo add check */

  return true;
}

bool KinematicGroupsEditorWidget::validateLinkChain(QStringList links)
{
  auto env_wrapper = EnvironmentManager::get(getComponentInfo());
  if (env_wrapper == nullptr)
    return true;

  auto env = env_wrapper->getEnvironment();
  if (env == nullptr || !env->isInitialized())
    return true;

  /** @todo add check */

  return true;
}

}  // namespace tesseract_gui
