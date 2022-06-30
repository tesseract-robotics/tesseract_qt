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
#ifndef TESSERACT_QT_PLOT_PLOT_LEGEND_H
#define TESSERACT_QT_PLOT_PLOT_LEGEND_H

#include <QObject>
#include <qwt/qwt_plot_legenditem.h>
#include <qwt/qwt_plot.h>

namespace tesseract_gui
{
class PlotLegend : public QObject, public QwtPlotLegendItem
{
  Q_OBJECT
public:
  PlotLegend(QwtPlot* parent);

  QRectF hideButtonRect() const;

  const QwtPlotItem* processMousePressEvent(QMouseEvent* mouse_event);

private:
  virtual void draw(QPainter* p, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& rect) const override;

  virtual void
  drawLegendData(QPainter* painter, const QwtPlotItem*, const QwtLegendData&, const QRectF&) const override;

  virtual void drawBackground(QPainter* painter, const QRectF& rect) const override;

  QwtPlot* _parent_plot;
  bool _collapsed;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_PLOT_PLOT_LEGEND_H
