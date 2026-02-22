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
#include <tesseract_qt/plot/timeseries_qwt.h>
#include <limits>
#include <stdexcept>
#include <QMessageBox>
#include <QPushButton>
#include <QString>

namespace tesseract::gui
{
RangeOpt QwtTimeseries::getVisualizationRangeY(Range range_X)
{
  int first_index = _ts_data->getIndexFromX(range_X.min);
  int last_index = _ts_data->getIndexFromX(range_X.max);

  if (first_index > last_index || first_index < 0 || last_index < 0)
  {
    return {};
  }

  if (first_index == 0 && last_index == plotData()->size() - 1)
  {
    return _ts_data->rangeY();
  }

  double min_y = (std::numeric_limits<double>::max());
  double max_y = (-std::numeric_limits<double>::max());

  for (size_t i = first_index; i < last_index; i++)
  {
    const double Y = sample(i).y();
    min_y = std::min(min_y, Y);
    max_y = std::max(max_y, Y);
  }
  return Range{ min_y, max_y };
}

std::optional<QPointF> QwtTimeseries::sampleFromTime(double t)
{
  int index = _ts_data->getIndexFromX(t);
  if (index < 0)
  {
    return {};
  }
  const auto& p = plotData()->at(size_t(index));
  return QPointF(p.x, p.y);
}

TransformedTimeseries::TransformedTimeseries(const PlotData* source_data)
  : QwtTimeseries(&_dst_data), _dst_data(source_data->plotName(), {}), _src_data(source_data)
{
}

TransformFunction::Ptr TransformedTimeseries::transform() { return _transform; }

void TransformedTimeseries::setTransform(QString transform_ID)
{
  if (transformName() == transform_ID)
  {
    return;
  }
  if (transform_ID.isEmpty())
  {
    _transform.reset();
  }
  else
  {
    _dst_data.clear();
    _transform = TransformFactory::create(transform_ID.toStdString());
    std::vector<PlotData*> dest = { &_dst_data };
    _transform->setData(nullptr, { _src_data }, dest);
  }
}

bool TransformedTimeseries::updateCache(bool reset_old_data)
{
  if (_transform)
  {
    if (reset_old_data)
    {
      _dst_data.clear();
      _transform->reset();
    }
    std::vector<PlotData*> dest = { &_dst_data };
    _transform->calculate();
  }
  else
  {
    // TODO: optimize ??
    _dst_data.clear();
    for (size_t i = 0; i < _src_data->size(); i++)
    {
      _dst_data.pushBack(_src_data->at(i));
    }
  }
  return true;
}

QString TransformedTimeseries::transformName() { return (!_transform) ? QString() : _transform->name(); }

QString TransformedTimeseries::alias() const { return _alias; }

void TransformedTimeseries::setAlias(QString alias) { _alias = alias; }

QRectF QwtSeriesWrapper::boundingRect() const
{
  if (size() == 0)
  {
    return {};
  }
  auto range_x = plotData()->rangeX().value();
  auto range_y = plotData()->rangeY().value();

  QRectF box;
  box.setLeft(range_x.min - _time_offset);
  box.setRight(range_x.max - _time_offset);
  box.setTop(range_y.max);
  box.setBottom(range_y.min);
  return box;
}

QPointF QwtSeriesWrapper::sample(size_t i) const
{
  const auto& p = _data->at(i);
  return QPointF(p.x - _time_offset, p.y);
}

size_t QwtSeriesWrapper::size() const { return _data->size(); }

void QwtSeriesWrapper::setTimeOffset(double offset) { _time_offset = offset; }

RangeOpt QwtSeriesWrapper::getVisualizationRangeX()
{
  if (this->size() < 2)
  {
    return {};
  }
  else
  {
    auto range = _data->rangeX().value();
    return RangeOpt({ range.min - _time_offset, range.max - _time_offset });
  }
}

const PlotDataBase<double, double>* QwtSeriesWrapper::plotData() const { return _data; }
}  // namespace tesseract::gui
