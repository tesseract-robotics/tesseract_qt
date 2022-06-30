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
#ifndef TESSERACT_QT_PLOT_TRANSFORMS_FIRST_DERIVATIVE_H
#define TESSERACT_QT_PLOT_TRANSFORMS_FIRST_DERIVATIVE_H

#include <QLineEdit>
#include <tesseract_qt/plot/transform_function.h>

namespace Ui
{
class FirstDerivariveForm;
}

namespace tesseract_gui
{
class FirstDerivative : public TransformFunction_SISO
{
public:
  FirstDerivative();

  ~FirstDerivative() override;

  const char* name() const override { return "Derivative"; }

  QWidget* optionsWidget() override;

  void on_buttonCompute_clicked();

private:
  std::optional<PlotData::Point> calculateNextPoint(size_t index) override;

  std::unique_ptr<QWidget> _widget;
  std::unique_ptr<Ui::FirstDerivariveForm> ui;
  double _dT;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_PLOT_TRANSFORMS_FIRST_DERIVATIVE_H
