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
#include <tesseract_qt/common/widgets/image_viewer_widget.h>
#include <tesseract_qt/common/icon_utils.h>

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
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QWheelEvent>

#include <cmath>

namespace tesseract_gui
{
struct ImageViewerWidgetImpl
{
  // Image & scene
  QImage image;
  QGraphicsScene* scene{ nullptr };
  QGraphicsPixmapItem* pix{ nullptr };

  // View state
  double scale_factor{ 1.0 };  // only used when not in "fit" mode

  // Default Directory
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

ImageViewerWidget::ImageViewerWidget(QWidget* parent)
  : QGraphicsView(parent), data_(std::make_unique<ImageViewerWidgetImpl>())
{
  data_->scene = new QGraphicsScene(this);
  data_->pix = data_->scene->addPixmap(QPixmap());

  setScene(data_->scene);
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  setResizeAnchor(QGraphicsView::AnchorViewCenter);
  setDragMode(QGraphicsView::ScrollHandDrag);  // pan with mouse drag

  createToolBar();
  data_->toolbar->setParent(this);
  data_->toolbar->raise();
  data_->toolbar->show();

  QSettings ms;
  ms.beginGroup("ImageViewerWidget");
  const QStringList standard_locations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
  data_->default_directory = ms.value("default_directory", standard_locations[0]).toString();
  ms.endGroup();

  updateActionEnables();
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
    QMessageBox::information(
        this, "ImageViewer", QString("Failed to load file %1: %2").arg(filepath, reader.errorString()));
    return false;
  }
  setImage(image);
  return true;
}

void ImageViewerWidget::setImage(const QImage& image)
{
  data_->image = image;
  data_->pix->setPixmap(QPixmap::fromImage(image));
  data_->pix->setTransformationMode(Qt::SmoothTransformation);

  data_->scene->setSceneRect(data_->pix->pixmap().rect());
  data_->scale_factor = 1.0;

  setTransform(QTransform());

  applyFitIfEnabled();

  updateActionEnables();
}

void ImageViewerWidget::onOpen()
{
  QStringList filters;
  for (const QByteArray& mimeTypeName : QImageReader::supportedMimeTypes())
    filters.append(mimeTypeName);
  filters.sort();

  QFileDialog dialog(this, "Open Image", data_->default_directory);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setNameFilters(filters);
  if (dialog.exec() == 1)
  {
    const QStringList selected_files = dialog.selectedFiles();
    data_->default_directory = QFileInfo(selected_files[0]).absoluteDir().path();
    loadImage(selected_files[0]);
  }
}

void ImageViewerWidget::onSave()
{
  if (data_->image.isNull())
    return;

  QStringList filters;
  for (const QByteArray& mimeTypeName : QImageReader::supportedMimeTypes())
    filters.append(mimeTypeName);
  filters.sort();

  QFileDialog dialog(this, "Save Image", data_->default_directory);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setNameFilters(filters);
  if (dialog.exec() == 1)
  {
    const QStringList selected_files = dialog.selectedFiles();
    data_->default_directory = QFileInfo(selected_files[0]).absoluteDir().path();
    QString save_path = selected_files[0];
    QImageWriter writer(save_path);

    if (!writer.write(data_->image))
      QMessageBox::information(
          this, "ImageViewer", QString("Failed to save image %1: %2").arg(save_path, writer.errorString()));
  }
}

void ImageViewerWidget::onZoomIn() { setScaleFactor(data_->scale_factor * 1.25); }

void ImageViewerWidget::onZoomOut() { setScaleFactor(data_->scale_factor * 0.8); }

void ImageViewerWidget::onNormalSize()
{
  data_->scale_factor = 1.0;
  setTransform(QTransform());
  updateActionEnables();
}

void ImageViewerWidget::onFitToWindow()
{
  applyFitIfEnabled();
  updateActionEnables();
}

void ImageViewerWidget::createToolBar()
{
  data_->toolbar = new QToolBar;  // NOLINT
  data_->toolbar->setIconSize(QSize(18, 18));
  data_->toolbar->setMovable(false);
  data_->toolbar->setFloatable(false);
  data_->toolbar->setContentsMargins(0, 0, 0, 0);

  data_->open_action = data_->toolbar->addAction(icons::getImportIcon(), "Open Image", this, SLOT(onOpen()));
  data_->save_action = data_->toolbar->addAction(icons::getSaveIcon(), "Save Image", this, SLOT(onSave()));
  data_->toolbar->addSeparator();
  data_->zoom_in_action = data_->toolbar->addAction(icons::getZoomInIcon(), "Zoom In", this, SLOT(onZoomIn()));
  data_->zoom_out_action = data_->toolbar->addAction(icons::getZoomOutIcon(), "Zoom Out", this, SLOT(onZoomOut()));
  data_->normal_size_action =
      data_->toolbar->addAction(icons::getRatioIcon(), "Normal Size", this, SLOT(onNormalSize()));
  data_->fit_to_window_action =
      data_->toolbar->addAction(icons::getFullscreenIcon(), "Fit To Window", this, SLOT(onFitToWindow()));

  data_->fit_to_window_action->setCheckable(true);
  data_->fit_to_window_action->setChecked(true);
}

void ImageViewerWidget::updateActionEnables()
{
  const bool has_image = !data_->image.isNull();
  const bool fit = (data_->fit_to_window_action && data_->fit_to_window_action->isChecked());

  if (data_->save_action)
    data_->save_action->setEnabled(has_image);
  if (data_->zoom_in_action)
    data_->zoom_in_action->setEnabled(has_image && !fit);
  if (data_->zoom_out_action)
    data_->zoom_out_action->setEnabled(has_image && !fit);
  if (data_->normal_size_action)
    data_->normal_size_action->setEnabled(has_image && !fit);
}

void ImageViewerWidget::applyFitIfEnabled()
{
  if (!data_->pix)
    return;

  if (data_->fit_to_window_action && data_->fit_to_window_action->isChecked())
  {
    setTransform(QTransform());
    fitInView(data_->pix, Qt::KeepAspectRatio);
  }
  else
  {
    onNormalSize();
  }
}

void ImageViewerWidget::setScaleFactor(double s)
{
  if (!data_->pix)
    return;
  if (data_->fit_to_window_action && data_->fit_to_window_action->isChecked())
    return;  // disabled in fit mode

  data_->scale_factor = s;  // std::clamp(s, 0.333, 3.0);
  setTransform(QTransform());
  updateActionEnables();
}

void ImageViewerWidget::wheelEvent(QWheelEvent* e)
{
  if (data_->image.isNull())
    return;

  // Scroll by default; hold Ctrl to zoom
  if (!(e->modifiers() & Qt::ControlModifier) || !data_->pix)
  {
    QGraphicsView::wheelEvent(e);
    return;
  }

  // Smooth delta (support touchpads)
  qreal dy = 0;
  if (!e->pixelDelta().isNull())
    dy = e->pixelDelta().y();
  else if (!e->angleDelta().isNull())
    dy = e->angleDelta().y();

  const qreal base = 1.0015;
  const qreal factor = std::pow(base, dy);
  setScaleFactor(data_->scale_factor * factor);

  e->accept();
}

void ImageViewerWidget::resizeEvent(QResizeEvent* e)
{
  QGraphicsView::resizeEvent(e);

  // Keep toolbar pinned to the top-left with a margin
  if (data_->toolbar)
    data_->toolbar->move(8, 8);

  // Re-apply fit on resize if enabled
  if (data_->pix && data_->fit_to_window_action && data_->fit_to_window_action->isChecked())
  {
    setTransform(QTransform());
    fitInView(data_->pix, Qt::KeepAspectRatio);
  }
}
}  // namespace tesseract_gui
