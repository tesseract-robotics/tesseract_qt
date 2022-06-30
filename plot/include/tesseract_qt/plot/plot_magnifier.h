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
#ifndef TESSERACT_QT_PLOT_PLOT_MAGNIFIER_H
#define TESSERACT_QT_PLOT_PLOT_MAGNIFIER_H

#include <QTimer>
#include <QEvent>
#include <qwt/qwt_plot_magnifier.h>
#include <qwt/qwt_plot.h>

namespace tesseract_gui
{
class PlotMagnifier : public QwtPlotMagnifier
{
  Q_OBJECT

public:
  explicit PlotMagnifier(QWidget* canvas);
  virtual ~PlotMagnifier() override;

  void setAxisLimits(int axis, double lower, double upper);
  virtual void widgetWheelEvent(QWheelEvent* event) override;

  enum AxisMode
  {
    X_AXIS,
    Y_AXIS,
    BOTH_AXES
  };

  virtual void rescale(double factor) override { rescale(factor, _default_mode); }

  void setDefaultMode(AxisMode mode) { _default_mode = mode; }

  void rescale(double factor, AxisMode axis);

protected:
  virtual void widgetMousePressEvent(QMouseEvent* event) override;

  double _lower_bounds[QwtPlot::axisCnt];
  double _upper_bounds[QwtPlot::axisCnt];

  QPointF _mouse_position;

Q_SIGNALS:
  void rescaled(QRectF new_size);

private:
  QPointF invTransform(QPoint pos);
  QTimer _future_emit;
  AxisMode _default_mode;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_PLOT_PLOT_MAGNIFIER_H
