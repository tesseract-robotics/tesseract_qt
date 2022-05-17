/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include "ui_allowed_collision_matrix_widget.h"
#include <QDialog>

namespace tesseract_gui
{
AllowedCollisionMatrixWidget::AllowedCollisionMatrixWidget(QWidget* parent)
  : QWidget(parent), ui_(std::make_unique<Ui::AllowedCollisionMatrixWidget>())
{
  ui_->setupUi(this);
  connect(ui_->generatePushButton, SIGNAL(clicked()), this, SLOT(onGenerateButtonClicked()));
  connect(ui_->removePushButton, SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));
  connect(ui_->addPushButton, SIGNAL(clicked()), this, SLOT(onAddButtonClicked()));
  connect(ui_->acmTreeView,
          SIGNAL(entrySelected(tesseract_common::AllowedCollisionEntries)),
          this,
          SIGNAL(entrySelected(tesseract_common::AllowedCollisionEntries)));
  connect(ui_->acmTreeView,
          SIGNAL(selectedLinksChanged(std::vector<std::string>)),
          this,
          SIGNAL(selectedLinksChanged(std::vector<std::string>)));
}

AllowedCollisionMatrixWidget::~AllowedCollisionMatrixWidget() = default;

void AllowedCollisionMatrixWidget::setModel(tesseract_gui::AllowedCollisionMatrixModel* model)
{
  ui_->acmTreeView->setModel(model);
}

void AllowedCollisionMatrixWidget::onRemoveButtonClicked()
{
  QItemSelectionModel* selection = ui_->acmTreeView->selectionModel();
  QModelIndexList indices = selection->selectedRows();
  int row_cnt = indices.count();
  for (int i = row_cnt; i > 0; i--)
    ui_->acmTreeView->model()->removeRow(indices.at(i - 1).row(), QModelIndex());
}

void AllowedCollisionMatrixWidget::onAddButtonClicked()
{
  AddAllowedCollisionEntryDialog dialog;
  if (dialog.exec())
  {
    auto* model = qobject_cast<tesseract_gui::AllowedCollisionMatrixModel*>(ui_->acmTreeView->model());
    model->add(dialog.getLinkName1(), dialog.getLinkName2(), dialog.getReason());
  }
}

void AllowedCollisionMatrixWidget::onGenerateButtonClicked() { emit generateClicked(ui_->resolutionSlider->value()); }

}  // namespace tesseract_gui
