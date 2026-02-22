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
#include <tesseract_qt/plot/transforms/outlier_removal.h>
#include "ui_outlier_removal.h"

namespace tesseract::gui
{
OutlierRemovalFilter::OutlierRemovalFilter()
  : ui(std::make_unique<Ui::OutlierRemovalFilter>())
  , _widget(std::make_unique<QWidget>())
  , _buffer(4)
  , _ring_view(_buffer.begin(), _buffer.end())
{
  ui->setupUi(_widget.get());

  connect(ui->spinBoxFactor, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [=](int) {
    emit parametersChanged();
  });
}

OutlierRemovalFilter::~OutlierRemovalFilter() = default;

QWidget* OutlierRemovalFilter::optionsWidget() { return _widget.get(); }

std::optional<PlotData::Point> OutlierRemovalFilter::calculateNextPoint(size_t index)
{
  const auto& p = dataSource()->at(index);
  _ring_view.push_back(p.y);

  if (index < 3)
  {
    return p;
  }

  double d1 = (_ring_view[1] - _ring_view[2]);
  double d2 = (_ring_view[2] - _ring_view[3]);
  if (d1 * d2 < 0)  // spike
  {
    double d0 = (_ring_view[0] - _ring_view[1]);
    double thresh = ui->spinBoxFactor->value();

    double jump = std::max(std::abs(d1), std::abs(d2));
    if (jump / std::abs(d0) > thresh)
    {
      return {};
    }
  }
  return dataSource()->at(index - 1);
}
}  // namespace tesseract::gui
