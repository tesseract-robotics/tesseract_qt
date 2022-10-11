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
#include <tesseract_qt/kinematic_groups/kinematic_groups_editor_widget.h>
#include <tesseract_qt/kinematic_groups/kinematic_groups_model.h>
#include <tesseract_qt/common/standard_item_type.h>
#include "ui_kinematic_groups_editor_widget.h"

#include <QStringListModel>

namespace tesseract_gui
{
struct KinematicGroupsEditorWidgetImpl
{
  QStringListModel* link_names_model;
  QStringListModel* joint_names_model;
  KinematicGroupsModel* kin_groups_model;
  ChainGroupValidator chain_group_validator;
  JointGroupValidator joint_group_validator;
  LinkGroupValidator link_group_validator;
};

KinematicGroupsEditorWidget::KinematicGroupsEditorWidget(QWidget* parent)
  : QWidget(parent)
  , ui_(std::make_unique<Ui::KinematicGroupsEditorWidget>())
  , data_(std::make_unique<KinematicGroupsEditorWidgetImpl>())

{
  ui_->setupUi(this);

  connect(ui_->addGroupPushButton, SIGNAL(clicked()), this, SLOT(onAddGroup()));
  connect(ui_->removeGroupPushButton, SIGNAL(clicked()), this, SLOT(onRemoveGroup()));

  connect(ui_->addJointPushButton, SIGNAL(clicked()), this, SLOT(onAddJoint()));
  connect(ui_->removeJointPushButton, SIGNAL(clicked()), this, SLOT(onRemoveJoint()));

  connect(ui_->addLinkPushButton, SIGNAL(clicked()), this, SLOT(onAddLink()));
  connect(ui_->removeLinkPushButton, SIGNAL(clicked()), this, SLOT(onRemoveLink()));
}

KinematicGroupsEditorWidget::~KinematicGroupsEditorWidget() = default;

void KinematicGroupsEditorWidget::setValidators(ChainGroupValidator chain_group_validator,
                                                JointGroupValidator joint_group_validator,
                                                LinkGroupValidator link_group_validator)
{
  data_->chain_group_validator = chain_group_validator;
  data_->joint_group_validator = joint_group_validator;
  data_->link_group_validator = link_group_validator;
}

void KinematicGroupsEditorWidget::setModels(KinematicGroupsModel* kin_groups_model,
                                            QStringListModel* link_names_model,
                                            QStringListModel* joint_names_model)
{
  data_->kin_groups_model = kin_groups_model;
  data_->link_names_model = link_names_model;
  data_->joint_names_model = joint_names_model;
  ui_->kinGroupTreeView->setModel(data_->kin_groups_model);
  ui_->baseLinkNameComboBox->setModel(data_->link_names_model);
  ui_->tipLinkNameComboBox->setModel(data_->link_names_model);
  ui_->linkComboBox->setModel(data_->link_names_model);
  ui_->jointComboBox->setModel(data_->joint_names_model);
}

void KinematicGroupsEditorWidget::onAddGroup()
{
  QString group_name = ui_->groupNameLineEdit->text();
  if (group_name.isEmpty())
    return;

  if (ui_->kinGroupTabWidget->currentIndex() == 0)
  {
    QString base_link = ui_->baseLinkNameComboBox->currentText();
    QString tip_link = ui_->tipLinkNameComboBox->currentText();
    if (base_link == tip_link || base_link.isEmpty() || tip_link.isEmpty())
      return;

    if (data_->chain_group_validator != nullptr && !data_->chain_group_validator(base_link, tip_link))
      return;

    tesseract_srdf::ChainGroup group{ { base_link.toStdString(), tip_link.toStdString() } };
    data_->kin_groups_model->addChainGroup(group_name, group);

    ui_->groupNameLineEdit->clear();
    return;
  }
  else if (ui_->kinGroupTabWidget->currentIndex() == 1)
  {
    QStringList joints;
    for (int i = 0; i < ui_->jointListWidget->count(); ++i)
      joints.append(ui_->jointListWidget->item(i)->text());

    if (data_->joint_group_validator != nullptr && !data_->joint_group_validator(joints))
      return;

    tesseract_srdf::JointGroup group;
    for (auto& joint : joints)
      group.push_back(joint.toStdString());

    data_->kin_groups_model->addJointGroup(group_name, group);

    ui_->groupNameLineEdit->clear();
    ui_->jointListWidget->clear();
    return;
  }
  else if (ui_->kinGroupTabWidget->currentIndex() == 2)
  {
    QStringList links;
    for (int i = 0; i < ui_->linkListWidget->count(); ++i)
      links.append(ui_->linkListWidget->item(i)->text());

    if (data_->link_group_validator != nullptr && !data_->link_group_validator(links))
      return;

    tesseract_srdf::LinkGroup group;
    for (auto& link : links)
      group.push_back(link.toStdString());

    data_->kin_groups_model->addLinkGroup(group_name, group);

    ui_->groupNameLineEdit->clear();
    ui_->linkListWidget->clear();
    return;
  }
}

void KinematicGroupsEditorWidget::onRemoveGroup()
{
  QModelIndexList selection = ui_->kinGroupTreeView->selectionModel()->selectedIndexes();
  int row_cnt = selection.count();
  for (int i = row_cnt; i > 0; i--)
  {
    QStandardItem* item = data_->kin_groups_model->itemFromIndex(selection.at(i - 1));
    if (item->type() == static_cast<int>(StandardItemType::SRDF_CHAIN_GROUP) ||
        item->type() == static_cast<int>(StandardItemType::SRDF_JOINT_GROUP) ||
        item->type() == static_cast<int>(StandardItemType::SRDF_LINK_GROUP))
    {
      data_->kin_groups_model->removeGroup(item->text());
    }
  }
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
}  // namespace tesseract_gui
