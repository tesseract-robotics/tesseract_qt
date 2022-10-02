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
#include <tesseract_qt/common/image_viewer_widget.h>
#include <tesseract_qt/common/icon_utils.h>
#include "ui_image_viewer_widget.h"

#include <QToolBar>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>
#include <QLabel>
#include <QImage>
#include <QScrollBar>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>

namespace tesseract_gui
{
struct ImageViewerWidgetImpl
{
  QImage image;
  QLabel* image_area;
  double scale_factor{1};
  QString default_directory;

  // Toolbar
  QToolBar* toolbar{ nullptr };
  QAction* open_action{ nullptr };
  QAction* save_action{ nullptr };
  QAction* zoom_in_action{ nullptr };
  QAction* zoom_out_action{ nullptr };
  QAction* normal_size_action{ nullptr };
  QAction* fit_to_window_action{ nullptr };

};

ImageViewerWidget::ImageViewerWidget(QWidget *parent) \
  : QWidget(parent)
  , ui(std::make_unique<Ui::ImageViewerWidget>())
  , data_(std::make_unique<ImageViewerWidgetImpl>())
{
  ui->setupUi(this);

  data_->image_area = new QLabel();
  data_->image_area->setBackgroundRole(QPalette::Base);
  data_->image_area->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  data_->image_area->setScaledContents(true);

  ui->scroll_area->setBackgroundRole(QPalette::Dark);
  ui->scroll_area->setWidget(data_->image_area);
  ui->scroll_area->setVisible(false);

  createToolBar();
  ui->verticalLayout->insertWidget(0, data_->toolbar);

  QSettings ms;
  ms.beginGroup("ImageViewerWidget");
  data_->default_directory =
      ms.value("default_directory", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0]).toString();
  ms.endGroup();
}

ImageViewerWidget::~ImageViewerWidget()
{
  QSettings ms;
  ms.beginGroup("ImageViewerWidget");
  ms.setValue("default_directory", data_->default_directory);
  ms.endGroup();
}

bool ImageViewerWidget::loadImage(const QString& filepath)
{
  QImageReader reader(filepath);
  reader.setAutoTransform(true);
  const QImage image = reader.read();
  if (image.isNull())
  {
    QMessageBox::information(this, "ImageViewer", QString("Failed to load file %1: %2").arg(filepath, reader.errorString()));
    return false;
  }
  loadImage(image);
  return true;
}

void ImageViewerWidget::loadImage(const QImage &image)
{
  data_->image = image;
  data_->image_area->setPixmap(QPixmap::fromImage(data_->image));
  data_->scale_factor = 1.0;

  ui->scroll_area->setVisible(true);
  data_->fit_to_window_action->setEnabled(true);
  data_->save_action->setEnabled(!data_->image.isNull());
  data_->zoom_in_action->setEnabled(!data_->fit_to_window_action->isChecked());
  data_->zoom_out_action->setEnabled(!data_->fit_to_window_action->isChecked());
  data_->normal_size_action->setEnabled(!data_->fit_to_window_action->isChecked());

  if (!data_->fit_to_window_action->isChecked())
    data_->image_area->adjustSize();
}

void ImageViewerWidget::scaleImage(double factor)
{
  data_->scale_factor *= factor;
  QSize current_size = data_->image_area->pixmap()->size();
  data_->image_area->resize(data_->scale_factor * current_size);

  QScrollBar *hsb = ui->scroll_area->horizontalScrollBar();
  hsb->setValue(int(data_->scale_factor * hsb->value() + ((data_->scale_factor - 1) * hsb->pageStep()/2)));

  QScrollBar *vsb = ui->scroll_area->verticalScrollBar();
  vsb->setValue(int(data_->scale_factor * vsb->value() + ((data_->scale_factor - 1) * vsb->pageStep()/2)));

  data_->zoom_in_action->setEnabled(data_->scale_factor < 3.0);
  data_->zoom_out_action->setEnabled(data_->scale_factor > 0.333);
}

void ImageViewerWidget::onOpen()
{
  QStringList filters;
  for (const QByteArray &mimeTypeName : QImageReader::supportedMimeTypes())
      filters.append(mimeTypeName);
  filters.sort();

  QFileDialog dialog(this, "Open Image", data_->default_directory);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setNameFilters(filters);
  if (dialog.exec() == 1)
  {
    data_->default_directory = QFileInfo(dialog.selectedFiles()[0]).absoluteDir().path();
    loadImage(dialog.selectedFiles()[0]);
  }
}

void ImageViewerWidget::onSave()
{
  QStringList filters;
  for (const QByteArray &mimeTypeName : QImageReader::supportedMimeTypes())
      filters.append(mimeTypeName);
  filters.sort();

  QFileDialog dialog(this, "Save Image", data_->default_directory);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setNameFilters(filters);
  if (dialog.exec() == 1)
  {
    data_->default_directory = QFileInfo(dialog.selectedFiles()[0]).absoluteDir().path();
    QString save_path = dialog.selectedFiles()[0];
    QImageWriter writer(save_path);

    if (!writer.write(data_->image))
      QMessageBox::information(this, "ImageViewer", QString("Failed to save image %1: %2").arg(save_path, writer.errorString()));
  }
}

void ImageViewerWidget::onZoomIn()
{
  scaleImage(1.25);
}

void ImageViewerWidget::onZoomOut()
{
  scaleImage(0.8);
}

void ImageViewerWidget::onNormalSize()
{
  data_->image_area->adjustSize();
  data_->scale_factor = 1.0;
}

void ImageViewerWidget::onFitToWindow()
{
  bool fit_to_window = data_->fit_to_window_action->isChecked();
  ui->scroll_area->setWidgetResizable(fit_to_window);
  if (!fit_to_window)
    onNormalSize();

  data_->save_action->setEnabled(!data_->image.isNull());
  data_->zoom_in_action->setEnabled(!data_->fit_to_window_action->isChecked());
  data_->zoom_out_action->setEnabled(!data_->fit_to_window_action->isChecked());
  data_->normal_size_action->setEnabled(!data_->fit_to_window_action->isChecked());
}

void ImageViewerWidget::createToolBar()
{
  data_->toolbar = new QToolBar;  // NOLINT
  data_->open_action = data_->toolbar->addAction(icons::getImportIcon(), "Open Image", this, SLOT(onOpen()));
  data_->save_action = data_->toolbar->addAction(icons::getSaveIcon(), "Save Image", this, SLOT(onSave()));
  data_->toolbar->addSeparator();
  data_->zoom_in_action = data_->toolbar->addAction(icons::getZoomInIcon(), "Zoom In", this, SLOT(onZoomIn()));
  data_->zoom_out_action = data_->toolbar->addAction(icons::getZoomOutIcon(), "Zoom Out", this, SLOT(onZoomOut()));
  data_->normal_size_action = data_->toolbar->addAction(icons::getRatioIcon(), "Normal Size", this, SLOT(onNormalSize()));
  data_->fit_to_window_action = data_->toolbar->addAction(icons::getFullscreenIcon(), "Fit To Window", this, SLOT(onFitToWindow()));
}
}

