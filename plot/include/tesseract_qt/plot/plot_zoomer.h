/**
 * @author Davide Faconti <davide.faconti@gmail.com>
 *
 * @copyright Copyright (C) 2015-2018 Davide Faconti <davide.faconti@gmail.com>
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
#ifndef TESSERACT_QT_PLOT_PLOT_ZOOMER_H
#define TESSERACT_QT_PLOT_PLOT_ZOOMER_H

#include <QObject>
#include <QPoint>
#include <QDebug>
#include <qwt/qwt_plot_zoomer.h>

namespace tesseract_gui
{
class PlotZoomer : public QwtPlotZoomer
{
public:
  PlotZoomer();

  explicit PlotZoomer(QWidget*);

  virtual ~PlotZoomer() override = default;

  void keepAspectRatio(bool doKeep) { _keep_aspect_ratio = doKeep; }

protected:
  virtual void widgetMousePressEvent(QMouseEvent* event) override;
  virtual void widgetMouseReleaseEvent(QMouseEvent* event) override;
  virtual void widgetMouseMoveEvent(QMouseEvent* event) override;
  virtual bool accept(QPolygon&) const override;

  virtual void zoom(const QRectF& rect) override;

  virtual QSizeF minZoomSize() const override;

private:
  bool _mouse_pressed;
  bool _zoom_enabled;
  bool _keep_aspect_ratio;
  QPoint _initial_pos;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_PLOT_PLOT_ZOOMER_H
