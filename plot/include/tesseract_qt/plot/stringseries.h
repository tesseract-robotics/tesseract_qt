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
#ifndef TESSERACT_QT_PLOT_STRINGSERIES_H
#define TESSERACT_QT_PLOT_STRINGSERIES_H

#include <tesseract_qt/plot/timeseries.h>
#include <tesseract_qt/plot/string_ref_sso.h>
#include <algorithm>
#include <unordered_set>

namespace tesseract_gui
{
class StringSeries : public TimeseriesBase<StringRef>
{
public:
  using TimeseriesBase<StringRef>::_points;

  StringSeries(const std::string& name, PlotGroup::Ptr group) : TimeseriesBase<StringRef>(name, group) {}

  StringSeries(const StringSeries& other) = delete;
  StringSeries(StringSeries&& other) = default;

  StringSeries& operator=(const StringSeries& other) = delete;
  StringSeries& operator=(StringSeries&& other) = default;

  virtual void clear() override
  {
    _storage.clear();
    TimeseriesBase<StringRef>::clear();
  }

  void pushBack(const Point& p) override
  {
    auto temp = p;
    pushBack(std::move(temp));
  }

  virtual void pushBack(Point&& p) override
  {
    const auto& str = p.y;
    // do not add empty strings
    if (str.data() == nullptr || str.size() == 0)
    {
      return;
    }
    if (str.isSSO())
    {
      // the object stroed the string already, just push it
      TimeseriesBase<StringRef>::pushBack(std::move(p));
    }
    else
    {
      // save a copy of the string in the flywheel structure _storage
      // create a reference to that cached value.
      _tmp_str.assign(str.data(), str.size());

      auto it = _storage.find(_tmp_str);
      if (it == _storage.end())
      {
        it = _storage.insert(_tmp_str).first;
      }
      TimeseriesBase<StringRef>::pushBack({ p.x, StringRef(*it) });
    }
  }

private:
  std::string _tmp_str;
  std::unordered_set<std::string> _storage;
};

}  // namespace tesseract_gui

#endif
