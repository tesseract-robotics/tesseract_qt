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
#include <tesseract_qt/plot/transforms/integral_transform.h>
#include <QFormLayout>
#include <QDoubleValidator>
#include "ui_integral_transform.h"

namespace tesseract_gui
{
IntegralTransform::IntegralTransform()
  : _widget(std::make_unique<QWidget>()), ui(std::make_unique<Ui::IntegralTransform>()), _dT(0.0)
{
  ui->setupUi(_widget.get());
  ui->lineEditCustom->setValidator(new QDoubleValidator(0.0001, 1000, 4, ui->lineEditCustom));

  connect(ui->buttonCompute, &QPushButton::clicked, this, &IntegralTransform::on_buttonCompute_clicked);

  connect(ui->lineEditCustom, &QLineEdit::editingFinished, this, [=]() {
    _dT = ui->lineEditCustom->text().toDouble();
    emit parametersChanged();
  });

  connect(ui->radioActual, &QRadioButton::toggled, this, [=](bool toggled) {
    if (toggled)
    {
      _dT = 0.0;
      emit parametersChanged();
    }
  });

  connect(ui->radioCustom, &QRadioButton::toggled, this, [=](bool toggled) {
    if (toggled)
    {
      _dT = ui->lineEditCustom->text().toDouble();
      emit parametersChanged();
    }
  });
}

IntegralTransform::~IntegralTransform() = default;

std::optional<PlotData::Point> IntegralTransform::calculateNextPoint(size_t index)
{
  if (index == 0)
  {
    return {};
  }

  const auto& prev = dataSource()->at(index - 1);
  const auto& p = dataSource()->at(index);

  double dt = (_dT == 0.0) ? (p.x - prev.x) : _dT;

  if (dt <= 0)
  {
    return {};
  }

  double val = (p.y + prev.y) * dt / (2.0);
  _accumulated_value += val;
  PlotData::Point out = { p.x, _accumulated_value };
  return out;
}

QWidget* IntegralTransform::optionsWidget()
{
  const size_t data_size = dataSource()->size();

  if (!dataSource() || data_size < 2)
  {
    _widget->setEnabled(false);
  }
  return _widget.get();
}

void IntegralTransform::reset()
{
  _accumulated_value = 0.0;
  TransformFunction_SISO::reset();
}

void IntegralTransform::on_buttonCompute_clicked()
{
  if (!dataSource() || dataSource()->size() < 2)
  {
    return;
  }

  const size_t data_size = dataSource()->size();

  // calculate automatic diff
  std::vector<double> diff;
  diff.reserve(data_size - 1);
  double prev_t = dataSource()->at(0).x;
  for (size_t i = 1; i < data_size; i++)
  {
    double t = dataSource()->at(i).x;
    double delta = t - prev_t;
    prev_t = t;
    diff.push_back(delta);
  }

  size_t first = 0;
  size_t last = diff.size();
  if (data_size > 10)
  {
    std::sort(diff.begin(), diff.end());
    first = last / 5;
    last = (last * 4) / 5;
  }
  double total = 0;
  for (size_t i = first; i < last; i++)
  {
    total += diff[i];
  }
  double estimated_dt = total / static_cast<double>(last - first);
  ui->lineEditCustom->setText(QString::number(estimated_dt, 'g', 4));

  if (ui->radioCustom->isChecked())
  {
    _dT = estimated_dt;
    emit parametersChanged();
  }
}
}  // namespace tesseract_gui
