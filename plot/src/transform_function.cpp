/**
 * @author Davide Faconti <davide.faconti@gmail.com>
 *
 * @copyright Copyright (C) 2015-2018 Davide Faconti <davide.faconti@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include <tesseract_qt/plot/transform_function.h>

namespace tesseract_gui
{
TransformFunction::TransformFunction() : _data(nullptr)
{
  static unsigned order = 0;
  _order = order++;
}

std::vector<const PlotData*>& TransformFunction::dataSources() { return _src_vector; }

void TransformFunction::setData(PlotDataMapRef* data,
                                const std::vector<const PlotData*>& src_vect,
                                std::vector<PlotData*>& dst_vect)
{
  if (numInputs() >= 0 && src_vect.size() != numInputs())
  {
    throw std::runtime_error("Wrong number of input data sources "
                             "in setDataSource");
  }
  if (dst_vect.size() != numOutputs())
  {
    throw std::runtime_error("Wrong number of output data destinations");
  }
  _data = data;
  _src_vector = src_vect;
  _dst_vector = dst_vect;
}

void TransformFunction_SISO::reset() { _last_timestamp = -std::numeric_limits<double>::max(); }

void TransformFunction_SISO::calculate()
{
  const PlotData* src_data = _src_vector.front();
  PlotData* dst_data = _dst_vector.front();
  if (src_data->size() == 0)
  {
    return;
  }
  dst_data->setMaximumRangeX(src_data->maximumRangeX());
  if (dst_data->size() != 0)
  {
    _last_timestamp = dst_data->back().x;
  }

  int pos = src_data->getIndexFromX(_last_timestamp);
  size_t index = pos < 0 ? 0 : static_cast<size_t>(pos);

  while (index < src_data->size())
  {
    const auto& in_point = src_data->at(index);

    if (in_point.x >= _last_timestamp)
    {
      auto out_point = calculateNextPoint(index);
      if (out_point)
      {
        dst_data->pushBack(std::move(out_point.value()));
      }
      _last_timestamp = in_point.x;
    }
    index++;
  }
}

const PlotData* TransformFunction_SISO::dataSource() const
{
  if (_src_vector.empty())
  {
    return nullptr;
  }
  return _src_vector.front();
}

TransformFunction::Ptr TransformFactory::create(const std::string& name)
{
  auto it = instance()->creators_.find(name);
  if (it == instance()->creators_.end())
  {
    return {};
  }
  return it->second();
}

TransformFactory* TransformFactory::instance()
{
  static TransformFactory* _ptr(nullptr);
  if (!qApp->property("TransformFactory").isValid() && !_ptr)
  {
    _ptr = transform_factory_ptr_from_macro_;
    qApp->setProperty("TransformFactory", QVariant::fromValue(_ptr));
  }
  else if (!_ptr)
  {
    _ptr = qvariant_cast<TransformFactory*>(qApp->property("TransformFactory"));
  }
  else if (!qApp->property("TransformFactory").isValid())
  {
    qApp->setProperty("TransformFactory", QVariant::fromValue(_ptr));
  }
  return _ptr;
}

const std::set<std::string>& TransformFactory::registeredTransforms() { return instance()->names_; }

}  // namespace tesseract_gui
