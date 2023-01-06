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
#include <tesseract_qt/common/widgets/load_environment_widget.h>
#include "ui_load_environment_widget.h"

#include <QFileDialog>

namespace tesseract_gui
{
LoadEnvironmentWidget::LoadEnvironmentWidget(QWidget* parent)
  : QWidget(parent), ui_(std::make_unique<Ui::LoadEnvironmentWidget>())
{
  ui_->setupUi(this);

  connect(ui_->urdf_browse_button, SIGNAL(clicked()), this, SLOT(onBrowseURDFClicked()));
  connect(ui_->srdf_browse_button, SIGNAL(clicked()), this, SLOT(onBrowseSRDFClicked()));
}

LoadEnvironmentWidget::~LoadEnvironmentWidget() = default;

const QString& LoadEnvironmentWidget::getURDFFilePath() const { return urdf_filepath_; }
const QString& LoadEnvironmentWidget::getSRDFFilePath() const { return srdf_filepath_; }

void LoadEnvironmentWidget::onBrowseURDFClicked()
{
  QStringList filters;
  filters.append("URDF (*.urdf)");

  QFileDialog dialog(this, "Open URDF");
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  dialog.setModal(true);
  dialog.setNameFilters(filters);
  if (dialog.exec() == 1)
  {
    urdf_filepath_ = dialog.selectedFiles().first();
    ui_->urdf_line_edit->setText(urdf_filepath_);
  }
}

void LoadEnvironmentWidget::onBrowseSRDFClicked()
{
  QStringList filters;
  filters.append("SRDF (*.srdf)");

  QFileDialog dialog(this, "Open SRDF");
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  dialog.setModal(true);
  dialog.setNameFilters(filters);
  if (dialog.exec() == 1)
  {
    srdf_filepath_ = dialog.selectedFiles().first();
    ui_->srdf_line_edit->setText(srdf_filepath_);
  }
}

}  // namespace tesseract_gui
