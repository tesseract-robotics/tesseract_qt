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
#include <tesseract_qt/plot/plot_data.h>

namespace tesseract_gui
{
template <typename T>
typename std::unordered_map<std::string, T>::iterator addImpl(std::unordered_map<std::string, T>& series,
                                                              const std::string& name,
                                                              PlotGroup::Ptr group)
{
  std::string ID;
  if (group)
  {
    ID = group->name();
    if (ID.back() != '/')
    {
      ID.push_back('/');
    }
  }
  ID += name;

  return series.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(name, group))
      .first;
}

template <typename T>
T& getOrCreateImpl(std::unordered_map<std::string, T>& series, const std::string& name, const PlotGroup::Ptr& group)
{
  auto it = series.find(name);
  if (it == series.end())
  {
    it = addImpl(series, name, group);
  }
  return it->second;
}

PlotDataMap::iterator PlotDataMapRef::addNumeric(const std::string& name, PlotGroup::Ptr group)
{
  return addImpl(numeric, name, group);
}

AnySeriesMap::iterator PlotDataMapRef::addUserDefined(const std::string& name, PlotGroup::Ptr group)
{
  return addImpl(user_defined, name, group);
}

StringSeriesMap::iterator PlotDataMapRef::addStringSeries(const std::string& name, PlotGroup::Ptr group)
{
  return addImpl(strings, name, group);
}

PlotData& PlotDataMapRef::getOrCreateNumeric(const std::string& name, PlotGroup::Ptr group)
{
  return getOrCreateImpl(numeric, name, group);
}

StringSeries& PlotDataMapRef::getOrCreateStringSeries(const std::string& name, PlotGroup::Ptr group)
{
  return getOrCreateImpl(strings, name, group);
}

PlotDataAny& PlotDataMapRef::getOrCreateUserDefined(const std::string& name, PlotGroup::Ptr group)
{
  return getOrCreateImpl(user_defined, name, group);
}

PlotGroup::Ptr PlotDataMapRef::getOrCreateGroup(const std::string& name)
{
  if (name.empty())
  {
    throw std::runtime_error("Group name can not be empty");
  }
  auto& group = groups[name];
  if (!group)
  {
    group = std::make_shared<PlotGroup>(name);
  }
  return group;
}

std::unordered_set<std::string> PlotDataMapRef::getAllNames() const
{
  std::unordered_set<std::string> out;
  for (auto& it : numeric)
  {
    out.insert(it.first);
  }
  for (auto& it : strings)
  {
    out.insert(it.first);
  }
  for (auto& it : user_defined)
  {
    out.insert(it.first);
  }
  return out;
}

void PlotDataMapRef::clear()
{
  numeric.clear();
  strings.clear();
  user_defined.clear();
}

void PlotDataMapRef::setMaximumRangeX(double range)
{
  for (auto& it : numeric)
  {
    it.second.setMaximumRangeX(range);
  }
  for (auto& it : strings)
  {
    it.second.setMaximumRangeX(range);
  }
  for (auto& it : user_defined)
  {
    it.second.setMaximumRangeX(range);
  }
}

bool PlotDataMapRef::erase(const std::string& name)
{
  bool erased = false;
  auto num_it = numeric.find(name);
  if (num_it != numeric.end())
  {
    numeric.erase(num_it);
    erased = true;
  }

  auto str_it = strings.find(name);
  if (str_it != strings.end())
  {
    strings.erase(str_it);
    erased = true;
  }

  auto any_it = user_defined.find(name);
  if (any_it != user_defined.end())
  {
    user_defined.erase(any_it);
    erased = true;
  }
  return erased;
}

}  // namespace tesseract_gui
