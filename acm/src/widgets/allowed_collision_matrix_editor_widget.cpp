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

#include <tesseract_qt/acm/widgets/allowed_collision_matrix_editor_widget.h>
#include <tesseract_qt/acm/widgets/add_allowed_collision_entry_dialog.h>
#include <tesseract_qt/common/events/allowed_collision_matrix_events.h>
#include <tesseract_qt/common/events/environment_events.h>

#include "ui_allowed_collision_matrix_editor_widget.h"

#include <tesseract_environment/commands/modify_allowed_collisions_command.h>

#include <QDialog>
#include <QApplication>
#include <QItemSelection>

namespace tesseract_gui
{
struct AllowedCollisionMatrixEditorWidget::Implementation
{
  std::unique_ptr<AddAllowedCollisionEntryDialog> dialog;
};

AllowedCollisionMatrixEditorWidget::AllowedCollisionMatrixEditorWidget(QWidget* parent)
  : QWidget(parent), ui_(std::make_unique<Ui::AllowedCollisionMatrixEditorWidget>())
{
  ctor(ComponentInfo());
}

AllowedCollisionMatrixEditorWidget::AllowedCollisionMatrixEditorWidget(ComponentInfo component_info, QWidget* parent)
  : QWidget(parent), ui_(std::make_unique<Ui::AllowedCollisionMatrixEditorWidget>())
{
  ctor(std::move(component_info));
}

AllowedCollisionMatrixEditorWidget::~AllowedCollisionMatrixEditorWidget() = default;

void AllowedCollisionMatrixEditorWidget::setComponentInfo(ComponentInfo component_info)
{
  ui_->acm_widget->setComponentInfo(component_info);
  data_->dialog = std::make_unique<AddAllowedCollisionEntryDialog>(component_info);
}

const ComponentInfo& AllowedCollisionMatrixEditorWidget::getComponentInfo() const
{
  return ui_->acm_widget->getComponentInfo();
}

void AllowedCollisionMatrixEditorWidget::setModel(std::shared_ptr<AllowedCollisionMatrixModel> model)
{
  ui_->acm_widget->setModel(model);
  data_->dialog = std::make_unique<AddAllowedCollisionEntryDialog>(model->getComponentInfo());
}
std::shared_ptr<AllowedCollisionMatrixModel> AllowedCollisionMatrixEditorWidget::getModel()
{
  return ui_->acm_widget->getModel();
}
std::shared_ptr<const AllowedCollisionMatrixModel> AllowedCollisionMatrixEditorWidget::getModel() const
{
  return ui_->acm_widget->getModel();
}

QItemSelectionModel& AllowedCollisionMatrixEditorWidget::getSelectionModel()
{
  return ui_->acm_widget->getSelectionModel();
}
const QItemSelectionModel& AllowedCollisionMatrixEditorWidget::getSelectionModel() const
{
  return ui_->acm_widget->getSelectionModel();
}

void AllowedCollisionMatrixEditorWidget::onRemoveButtonClicked()
{
  QItemSelectionModel& selection = ui_->acm_widget->getSelectionModel();
  QModelIndexList indices = selection.selectedRows();
  if (indices.count() == 1)
  {
    std::vector<std::array<std::string, 2>> remove;
    auto m = ui_->acm_widget->getModel();
    QModelIndex idx = indices.at(0);
    if (idx.isValid() && idx.parent().isValid())
      remove.push_back({ m->data(idx.parent()).toString().toStdString(), m->data(idx).toString().toStdString() });

    QApplication::sendEvent(qApp, new events::AllowedCollisionMatrixRemove(m->getComponentInfo(), remove));
  }
}

void AllowedCollisionMatrixEditorWidget::onAddButtonClicked()
{
  if (data_->dialog != nullptr)
    data_->dialog->show();
}

void AllowedCollisionMatrixEditorWidget::onGenerateButtonClicked()
{
  QApplication::sendEvent(
      qApp, new events::AllowedCollisionMatrixGenerate(getComponentInfo(), ui_->resolutionSlider->value()));
}

void AllowedCollisionMatrixEditorWidget::onApplyButtonClicked()
{
  auto cmd = std::make_shared<tesseract_environment::ModifyAllowedCollisionsCommand>(
      ui_->acm_widget->getModel()->getAllowedCollisionMatrix(),
      tesseract_environment::ModifyAllowedCollisionsType::REPLACE);
  QApplication::sendEvent(qApp, new events::EnvironmentApplyCommand(getComponentInfo(), { cmd }));
}

void AllowedCollisionMatrixEditorWidget::ctor(ComponentInfo component_info)
{
  ui_->setupUi(this);

  ui_->acm_widget->setComponentInfo(component_info);

  connect(ui_->generatePushButton, SIGNAL(clicked()), this, SLOT(onGenerateButtonClicked()));
  connect(ui_->removePushButton, SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));
  connect(ui_->addPushButton, SIGNAL(clicked()), this, SLOT(onAddButtonClicked()));
  connect(ui_->applyPushButton, SIGNAL(clicked()), this, SLOT(onApplyButtonClicked()));
}

}  // namespace tesseract_gui
