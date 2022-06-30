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
#include <tesseract_qt/plot/plot_zoomer.h>
#include <QMouseEvent>
#include <QApplication>
#include <QSettings>
#include <QPen>

#include <qwt/qwt_plot_zoomer.h>
#include <qwt/qwt_scale_map.h>
#include <qwt/qwt_plot.h>

namespace tesseract_gui
{
PlotZoomer::PlotZoomer(QWidget* canvas)
  : QwtPlotZoomer(canvas, false), _mouse_pressed(false), _zoom_enabled(false), _keep_aspect_ratio(false)
{
  this->setTrackerMode(AlwaysOff);
}

void PlotZoomer::widgetMousePressEvent(QMouseEvent* me)
{
  _mouse_pressed = false;
  auto patterns = this->mousePattern();
  for (QwtEventPattern::MousePattern& pattern : patterns)
  {
    if (this->mouseMatch(pattern, me))
    {
      _mouse_pressed = true;
      // this->setTrackerMode(AlwaysOn);
      _initial_pos = me->pos();
    }
    break;
  }
  QwtPlotPicker::widgetMousePressEvent(me);
}

void PlotZoomer::widgetMouseMoveEvent(QMouseEvent* me)
{
  if (_mouse_pressed)
  {
    auto patterns = this->mousePattern();
    for (QwtEventPattern::MousePattern& pattern : patterns)
    {
      QRect rect(me->pos(), _initial_pos);
      QRectF zoomRect = invTransform(rect.normalized());

      if (zoomRect.width() > minZoomSize().width() && zoomRect.height() > minZoomSize().height())
      {
        if (!_zoom_enabled)
        {
          QSettings settings;
          QString theme = settings.value("Preferences::theme", "light").toString();
          QPixmap pixmap(tr(":/style_%1/zoom_in.png").arg(theme));
          QCursor zoom_cursor(pixmap.scaled(24, 24));

          _zoom_enabled = true;
          this->setRubberBand(RectRubberBand);
          this->setTrackerMode(AlwaysOff);
          QPen pen(parentWidget()->palette().foreground().color(), 1, Qt::DashLine);
          this->setRubberBandPen(pen);
          QApplication::setOverrideCursor(zoom_cursor);
        }
      }
      else if (_zoom_enabled)
      {
        _zoom_enabled = false;
        this->setRubberBand(NoRubberBand);
        QApplication::restoreOverrideCursor();
      }
      break;
    }
  }
  QwtPlotPicker::widgetMouseMoveEvent(me);
}

void PlotZoomer::widgetMouseReleaseEvent(QMouseEvent* me)
{
  _mouse_pressed = false;
  _zoom_enabled = false;
  QwtPlotPicker::widgetMouseReleaseEvent(me);
  this->setTrackerMode(AlwaysOff);
}

bool PlotZoomer::accept(QPolygon& pa) const
{
  QApplication::restoreOverrideCursor();

  if (pa.count() < 2)
    return false;

  QRect rect = QRect(pa[0], pa[int(pa.count()) - 1]);
  QRectF zoomRect = invTransform(rect.normalized());

  if (zoomRect.width() < minZoomSize().width() && zoomRect.height() < minZoomSize().height())
  {
    return false;
  }
  return QwtPlotZoomer::accept(pa);
}

void PlotZoomer::zoom(const QRectF& zoomRect)
{
  QRectF rect = zoomRect;

  if (_keep_aspect_ratio)
  {
    const QRectF cr = canvas()->contentsRect();
    const double canvas_ratio = cr.width() / cr.height();
    const double zoom_ratio = zoomRect.width() / zoomRect.height();

    if (zoom_ratio < canvas_ratio)
    {
      double new_width = zoomRect.height() * canvas_ratio;
      double increment = new_width - zoomRect.width();
      rect.setWidth(new_width);
      rect.moveLeft(rect.left() - 0.5 * increment);
    }
    else
    {
      double new_height = zoomRect.width() / canvas_ratio;
      double increment = new_height - zoomRect.height();
      rect.setHeight(new_height);
      rect.moveTop(rect.top() - 0.5 * increment);
    }
  }
  QwtPlotZoomer::zoom(rect);
}

QSizeF PlotZoomer::minZoomSize() const { return QSizeF(scaleRect().width() * 0.02, scaleRect().height() * 0.02); }
}  // namespace tesseract_gui
