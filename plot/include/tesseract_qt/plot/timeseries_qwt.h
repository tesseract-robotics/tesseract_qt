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
#ifndef TESSERACT_QT_PLOT_TIMESERIES_QWT_H
#define TESSERACT_QT_PLOT_TIMESERIES_QWT_H

#include <qwt/qwt_series_data.h>
#include <tesseract_qt/plot/plot_data.h>
#include <tesseract_qt/plot/transform_function.h>

namespace tesseract_gui
{
// wrapper to Timeseries including a time offset
class QwtSeriesWrapper : public QwtSeriesData<QPointF>
{
private:
  const PlotDataXY* _data;
  double _time_offset;

public:
  QwtSeriesWrapper(const PlotDataXY* data) : _data(data), _time_offset(0.0) {}

  QPointF sample(size_t i) const override;

  void setTimeOffset(double offset);

  virtual bool updateCache(bool reset_old_data) = 0;

  size_t size() const override;

  QRectF boundingRect() const override;

  const PlotDataXY* plotData() const;

  virtual RangeOpt getVisualizationRangeX();

  virtual RangeOpt getVisualizationRangeY(Range range_X) = 0;

  virtual std::optional<QPointF> sampleFromTime(double t) = 0;
};

class QwtTimeseries : public QwtSeriesWrapper
{
public:
  QwtTimeseries(const PlotData* data) : QwtSeriesWrapper(data), _ts_data(data) {}

  virtual RangeOpt getVisualizationRangeY(Range range_X) override;

  virtual std::optional<QPointF> sampleFromTime(double t) override;

protected:
  const PlotData* _ts_data;
};

//------------------------------------

class TransformedTimeseries : public QwtTimeseries
{
public:
  TransformedTimeseries(const PlotData* source_data);

  TransformFunction::Ptr transform();

  void setTransform(QString transform_ID);

  virtual bool updateCache(bool reset_old_data) override;

  QString transformName();

  QString alias() const;

  void setAlias(QString alias);

protected:
  QString _alias;
  PlotData _dst_data;
  const PlotData* _src_data;
  TransformFunction_SISO::Ptr _transform;
};

//---------------------------------------------------------
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_PLOT_TIMESERIES_QWT_H
