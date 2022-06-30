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
#ifndef TESSERACT_QT_PLOT_TIMESERIES_H
#define TESSERACT_QT_PLOT_TIMESERIES_H

#include <tesseract_qt/plot/plot_database.h>
#include <algorithm>

namespace tesseract_gui
{
template <typename Value>
class TimeseriesBase : public PlotDataBase<double, Value>
{
protected:
  double _max_range_x;
  using PlotDataBase<double, Value>::_points;

public:
  using Point = typename PlotDataBase<double, Value>::Point;

  TimeseriesBase(const std::string& name, PlotGroup::Ptr group)
    : PlotDataBase<double, Value>(name, group), _max_range_x(std::numeric_limits<double>::max())
  {
  }

  TimeseriesBase(const TimeseriesBase& other) = delete;
  TimeseriesBase(TimeseriesBase&& other) = default;

  TimeseriesBase& operator=(const TimeseriesBase& other) = delete;
  TimeseriesBase& operator=(TimeseriesBase&& other) = default;

  void clone(const TimeseriesBase& other)
  {
    _max_range_x = other._max_range_x;
    _points = other._points;
  }

  void setMaximumRangeX(double max_range)
  {
    _max_range_x = max_range;
    trimRange();
  }

  double maximumRangeX() const { return _max_range_x; }

  int getIndexFromX(double x) const;

  std::optional<Value> getYfromX(double x) const
  {
    int index = getIndexFromX(x);
    return (index < 0) ? std::nullopt : std::optional(_points[index].y);
  }

  void pushBack(const Point& p) override
  {
    auto temp = p;
    pushBack(std::move(temp));
  }

  void pushBack(Point&& p) override
  {
    bool need_sorting = (!_points.empty() && p.x < this->back().x);

    if (need_sorting)
    {
      auto it = std::upper_bound(_points.begin(), _points.end(), p, TimeCompare);
      PlotDataBase<double, Value>::insert(it, std::move(p));
    }
    else
    {
      PlotDataBase<double, Value>::pushBack(std::move(p));
    }
    trimRange();
  }

private:
  void trimRange()
  {
    while (_points.size() > 2 && (_points.back().x - _points.front().x) > _max_range_x)
    {
      this->popFront();
    }
  }

  static bool TimeCompare(const Point& a, const Point& b) { return a.x < b.x; }
};

//--------------------

template <typename Value>
inline int TimeseriesBase<Value>::getIndexFromX(double x) const
{
  if (_points.size() == 0)
  {
    return -1;
  }
  auto lower = std::lower_bound(_points.begin(), _points.end(), Point(x, {}), TimeCompare);
  auto index = std::distance(_points.begin(), lower);

  if (index >= _points.size())
  {
    return _points.size() - 1;
  }
  if (index < 0)
  {
    return 0;
  }

  if (index > 0 && (abs(_points[index - 1].x - x) < abs(_points[index].x - x)))
  {
    index = index - 1;
  }
  return index;
}

}  // namespace tesseract_gui

#endif
