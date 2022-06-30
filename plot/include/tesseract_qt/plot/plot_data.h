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
#ifndef TESSERACT_QT_PLOT_PLOT_DATA_H
#define TESSERACT_QT_PLOT_PLOT_DATA_H

#include <tesseract_qt/plot/plot_database.h>
#include <tesseract_qt/plot/timeseries.h>
#include <tesseract_qt/plot/stringseries.h>

namespace tesseract_gui
{
using PlotDataXY = PlotDataBase<double, double>;
using PlotData = TimeseriesBase<double>;
using PlotDataAny = TimeseriesBase<std::any>;

/**
 * @brief The PlotDataMapRef is the main data structure used to store all the
 * timeseries in a single place.
 */

using PlotDataMap = std::unordered_map<std::string, PlotData>;
using AnySeriesMap = std::unordered_map<std::string, PlotDataAny>;
using StringSeriesMap = std::unordered_map<std::string, StringSeries>;

struct PlotDataMapRef
{
  /// Numerical timeseries
  PlotDataMap numeric;

  /// Timeseries that can contain any data structure.
  /// PlotJuggler can not display it natively, only plugins can manipulate them.
  AnySeriesMap user_defined;

  /// Series of strings
  StringSeriesMap strings;

  /**
   * @brief Each series can have (optionally) a group.
   * Groups can have their own properties.
   */
  std::unordered_map<std::string, PlotGroup::Ptr> groups;

  PlotDataMap::iterator addNumeric(const std::string& name, PlotGroup::Ptr group = {});

  AnySeriesMap::iterator addUserDefined(const std::string& name, PlotGroup::Ptr group = {});

  StringSeriesMap::iterator addStringSeries(const std::string& name, PlotGroup::Ptr group = {});

  PlotData& getOrCreateNumeric(const std::string& name, PlotGroup::Ptr group = {});

  StringSeries& getOrCreateStringSeries(const std::string& name, PlotGroup::Ptr group = {});

  PlotDataAny& getOrCreateUserDefined(const std::string& name, PlotGroup::Ptr group = {});

  PlotGroup::Ptr getOrCreateGroup(const std::string& name);

  std::unordered_set<std::string> getAllNames() const;

  void clear();

  void setMaximumRangeX(double range);

  bool erase(const std::string& name);
};

template <typename Value>
inline void AddPrefixToPlotData(const std::string& prefix, std::unordered_map<std::string, Value>& data)
{
  if (prefix.empty())
  {
    return;
  }

  std::vector<std::string> temp_key;
  temp_key.reserve(data.size());
  std::vector<Value> temp_value;
  temp_value.reserve(data.size());

  for (auto& it : data)
  {
    std::string key;
    key.reserve(prefix.size() + 2 + it.first.size());
    key = (it.first.front() == '/') ? (prefix + it.first) : (prefix + "/" + it.first);

    temp_key.emplace_back(key);
    temp_value.emplace_back(std::move(it.second));
  }

  data.clear();

  for (size_t i = 0; i < temp_key.size(); i++)
  {
    const std::string& key = temp_key[i];

    auto it =
        data.emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(key, PlotGroup::Ptr()))
            .first;

    it->second = std::move(temp_value[i]);
  }
}

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_PLOT_PLOT_DATA_H
