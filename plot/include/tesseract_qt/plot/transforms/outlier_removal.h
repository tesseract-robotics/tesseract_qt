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
#ifndef TESSERACT_QT_PLOT_TRANSFORMS_OUTLIER_REMOVAL_H
#define TESSERACT_QT_PLOT_TRANSFORMS_OUTLIER_REMOVAL_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <tesseract_qt/plot/transform_function.h>
#include <tesseract_qt/plot/ring_span.h>

namespace Ui
{
class OutlierRemovalFilter;
}

namespace tesseract_gui
{
class OutlierRemovalFilter : public TransformFunction_SISO
{
public:
  explicit OutlierRemovalFilter();

  ~OutlierRemovalFilter() override;

  const char* name() const override { return "Outlier Removal"; }

  QWidget* optionsWidget() override;

private:
  std::unique_ptr<Ui::OutlierRemovalFilter> ui;
  std::unique_ptr<QWidget> _widget;
  std::vector<double> _buffer;
  nonstd::ring_span_lite::ring_span<double> _ring_view;

  std::optional<PlotData::Point> calculateNextPoint(size_t index) override;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_PLOT_TRANSFORMS_OUTLIER_REMOVAL_H
