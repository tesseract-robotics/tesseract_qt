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
#include <tesseract_qt/plot/transforms/moving_rms.h>
#include "ui_moving_rms.h"

namespace tesseract_gui
{
MovingRMS::MovingRMS()
  : ui(std::make_unique<Ui::MovingRMS>())
  , _widget(std::make_unique<QWidget>())
  , _buffer(1)
  , _ring_view(_buffer.begin(), _buffer.end())
{
  ui->setupUi(_widget.get());

  connect(ui->spinBoxSamples, qOverload<int>(&QSpinBox::valueChanged), this, [=](int) { emit parametersChanged(); });
}

MovingRMS::~MovingRMS() = default;

void MovingRMS::reset()
{
  _buffer.clear();
  TransformFunction_SISO::reset();
}

QWidget* MovingRMS::optionsWidget() { return _widget.get(); }

std::optional<PlotData::Point> MovingRMS::calculateNextPoint(size_t index)
{
  size_t buffer_size = std::min(size_t(ui->spinBoxSamples->value()), size_t(dataSource()->size()));
  if (buffer_size != _buffer.size())
  {
    _buffer.resize(buffer_size);
    _ring_view = nonstd::ring_span<PlotData::Point>(_buffer.begin(), _buffer.end());
  }

  const auto& p = dataSource()->at(index);
  _ring_view.push_back(p);

  while (_ring_view.size() < buffer_size)
  {
    _ring_view.push_back(p);
  }

  double total_sqr = 0;

  for (size_t i = 0; i < buffer_size; i++)
  {
    double val = _ring_view[i].y;
    total_sqr += val * val;
  }

  double time = p.x;

  PlotData::Point out = { time, sqrt(total_sqr / _ring_view.size()) };
  return out;
}
}  // namespace tesseract_gui
