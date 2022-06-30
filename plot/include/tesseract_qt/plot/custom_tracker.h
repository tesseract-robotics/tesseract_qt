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
#ifndef TESSERACT_QT_PLOT_CUSTOM_TRACKER_H
#define TESSERACT_QT_PLOT_CUSTOM_TRACKER_H

#include <QEvent>
#include <QPointF>
#include <qwt/qwt_plot_picker.h>
#include <qwt/qwt_picker_machine.h>
#include <qwt/qwt_plot_marker.h>

class QwtPlotCurve;

namespace tesseract_gui
{
class CurveTracker : public QObject
{
  Q_OBJECT
public:
  explicit CurveTracker(QwtPlot*);

  ~CurveTracker();

  QPointF actualPosition() const;

  typedef enum
  {
    LINE_ONLY,
    VALUE,
    VALUE_NAME
  } Parameter;

public Q_SLOTS:

  void setPosition(const QPointF& pos);

  void setParameter(Parameter par);

  void setEnabled(bool enable);

  bool isEnabled() const;

  void redraw() { setPosition(_prev_trackerpoint); }

private:
  QLineF curveLineAt(const QwtPlotCurve*, double x) const;

  QPointF transform(QPoint);

  QPoint invTransform(QPointF);

  QPointF _prev_trackerpoint;
  std::vector<QwtPlotMarker*> _marker;
  QwtPlotMarker* _line_marker;
  QwtPlotMarker* _text_marker;
  QwtPlot* _plot;
  Parameter _param;
  bool _visible;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_PLOT_CUSTOM_TRACKER_H
