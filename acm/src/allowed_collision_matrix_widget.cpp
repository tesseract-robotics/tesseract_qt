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

#include <tesseract_qt/acm/allowed_collision_matrix_widget.h>
#include <tesseract_qt/acm/add_allowed_collision_entry_dialog.h>
#include <tesseract_qt/acm/allowed_collision_matrix_events.h>
#include "ui_allowed_collision_matrix_widget.h"
#include <QDialog>
#include <QApplication>

namespace tesseract_gui
{
AllowedCollisionMatrixWidget::AllowedCollisionMatrixWidget(QWidget* parent)
  : QWidget(parent), ui_(std::make_unique<Ui::AllowedCollisionMatrixWidget>())
{
  ui_->setupUi(this);
  connect(ui_->generatePushButton, SIGNAL(clicked()), this, SLOT(onGenerateButtonClicked()));
  connect(ui_->removePushButton, SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));
  connect(ui_->addPushButton, SIGNAL(clicked()), this, SLOT(onAddButtonClicked()));
  connect(ui_->acmTreeView, &QTreeView::collapsed, [this]() { ui_->acmTreeView->resizeColumnToContents(0); });
  connect(ui_->acmTreeView, &QTreeView::expanded, [this]() { ui_->acmTreeView->resizeColumnToContents(0); });
}

AllowedCollisionMatrixWidget::~AllowedCollisionMatrixWidget() = default;

void AllowedCollisionMatrixWidget::setModel(tesseract_gui::AllowedCollisionMatrixModel* model)
{
  ui_->acmTreeView->setModel(model);
  dialog_ = std::make_unique<AddAllowedCollisionEntryDialog>(model->getComponentInfo());
}

void AllowedCollisionMatrixWidget::onRemoveButtonClicked()
{
  QItemSelectionModel* selection = ui_->acmTreeView->selectionModel();
  QModelIndexList indices = selection->selectedRows();
  if (indices.count() == 1)
  {
    std::vector<std::array<std::string, 2>> remove;
    auto* m = ui_->acmTreeView->model();
    QModelIndex idx = indices.at(0);
    if (idx.isValid() && idx.parent().isValid())
      remove.push_back({ m->data(idx.parent()).toString().toStdString(), m->data(idx).toString().toStdString() });

    ComponentInfo component_info =
        qobject_cast<AllowedCollisionMatrixModel*>(ui_->acmTreeView->model())->getComponentInfo();
    QApplication::sendEvent(qApp, new events::AllowedCollisionMatrixRemove(component_info, remove));
  }
}

void AllowedCollisionMatrixWidget::onAddButtonClicked()
{
  if (dialog_ != nullptr)
    dialog_->show();
}

void AllowedCollisionMatrixWidget::onGenerateButtonClicked() { emit generateClicked(ui_->resolutionSlider->value()); }

}  // namespace tesseract_gui
