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
#ifndef TESSERACT_QT_PLOT_PLOT_SUBPLOTS_H
#define TESSERACT_QT_PLOT_PLOT_SUBPLOTS_H

#include <QWidget>
#include <memory>
#include <tesseract_qt/plot/plot_database.h>
#include <tesseract_qt/plot/plot_widget.h>

namespace tesseract_gui
{
class PlotSubplots : public QWidget
{
  Q_OBJECT

public:
  PlotSubplots(PlotDataMapRef& datamap, int rows, int cols, QWidget* parent = nullptr);

  PlotWidget* getSubplot(int row, int col);
  int rows() const;
  int cols() const;

public Q_SLOTS:
  void replot();

private:
  int rows_{ 0 };
  int cols_{ 0 };
  std::vector<std::vector<std::unique_ptr<PlotWidget>>> subplots_;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_PLOT_PLOT_SUBPLOTS_H
