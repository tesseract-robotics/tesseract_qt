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
#ifndef TESSERACT_QT_PLOT_POINT_SERIES_H
#define TESSERACT_QT_PLOT_POINT_SERIES_H

#include <tesseract_qt/plot/timeseries_qwt.h>

namespace tesseract_gui
{
class PointSeriesXY : public QwtSeriesWrapper
{
public:
  PointSeriesXY(const PlotData* x_axis, const PlotData* y_axis);

  virtual QPointF sample(size_t i) const override
  {
    const auto& p = _cached_curve.at(i);
    return QPointF(p.x, p.y);
  }

  size_t size() const override;

  std::optional<QPointF> sampleFromTime(double t) override;

  RangeOpt getVisualizationRangeY(Range range_X) override;

  bool updateCache(bool reset_old_data) override;

  RangeOpt getVisualizationRangeX() override;

  const PlotData* dataX() const { return _x_axis; }
  const PlotData* dataY() const { return _y_axis; }

protected:
  const PlotData* _x_axis;
  const PlotData* _y_axis;
  PlotDataXY _cached_curve;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_PLOT_POINT_SERIES_H
