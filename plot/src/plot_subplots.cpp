/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
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
#include <tesseract_qt/plot/plot_subplots.h>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QFrame>

namespace tesseract_gui
{
PlotSubplots::PlotSubplots(PlotDataMapRef& datamap, int rows, int cols, QWidget* parent)
  : QWidget(parent), rows_(rows), cols_(cols)
{
  if (rows < 1)
    throw std::runtime_error("PlotSubplots, rows must be greater than zero!");

  if (cols < 1)
    throw std::runtime_error("PlotSubplots, cols must be greater than zero!");

  QGridLayout* grid_layout = new QGridLayout(this);
  grid_layout->setHorizontalSpacing(cols);
  grid_layout->setVerticalSpacing(rows);
  grid_layout->setSpacing(0);

  subplots_.resize(rows);
  for (std::size_t r = 0; r < rows; ++r)
  {
    std::vector<std::unique_ptr<PlotWidget>>& row = subplots_[r];
    row.resize(cols);
    for (std::size_t c = 0; c < cols; ++c)
    {
      QFrame* frame = new QFrame(this);
      frame->setFrameStyle(QFrame::Box);
      QGridLayout* layout = new QGridLayout(frame);

      auto plot_widget = std::make_unique<PlotWidget>(datamap);
      layout->addWidget(plot_widget->widget());
      grid_layout->addWidget(frame);
      row[c] = std::move(plot_widget);
    }
  }
}

PlotWidget* PlotSubplots::getSubplot(int row, int col) { return subplots_.at(row).at(col).get(); }

int PlotSubplots::rows() const { return rows_; }
int PlotSubplots::cols() const { return cols_; }

void PlotSubplots::replot()
{
  for (std::size_t r = 0; r < rows_; ++r)
  {
    for (std::size_t c = 0; c < cols_; ++c)
    {
      subplots_.at(r).at(c)->replot();
    }
  }
}
}  // namespace tesseract_gui
