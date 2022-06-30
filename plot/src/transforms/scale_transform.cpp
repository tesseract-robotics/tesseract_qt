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
#include <tesseract_qt/plot/transforms/scale_transform.h>
#include "ui_scale_transform.h"

namespace tesseract_gui
{
ScaleTransform::ScaleTransform() : _widget(std::make_unique<QWidget>()), ui(std::make_unique<Ui::ScaleTransform>())
{
  ui->setupUi(_widget.get());

  //  ui->lineEditValue->setValidator( new QDoubleValidator() );

  connect(ui->buttonDegRad, &QPushButton::clicked, this, [=]() {
    const double deg_rad = 3.14159265359 / 180;
    ui->lineEditValueScale->setText(QString::number(deg_rad, 'g', 5));
    emit parametersChanged();
  });

  connect(ui->buttonRadDeg, &QPushButton::clicked, this, [=]() {
    const double rad_deg = 180.0 / 3.14159265359;
    ui->lineEditValueScale->setText(QString::number(rad_deg, 'g', 5));
    emit parametersChanged();
  });

  connect(ui->lineEditTimeOffset, &QLineEdit::editingFinished, this, [=]() { emit parametersChanged(); });
  connect(ui->lineEditValueOffset, &QLineEdit::editingFinished, this, [=]() { emit parametersChanged(); });
  connect(ui->lineEditValueScale, &QLineEdit::editingFinished, this, [=]() { emit parametersChanged(); });
}

ScaleTransform::~ScaleTransform() = default;

const char* ScaleTransform::name() const { return "Scale/Offset"; }

QWidget* ScaleTransform::optionsWidget() { return _widget.get(); }

std::optional<PlotData::Point> ScaleTransform::calculateNextPoint(size_t index)
{
  double off_x = ui->lineEditTimeOffset->text().toDouble();
  double off_y = ui->lineEditValueOffset->text().toDouble();
  double scale = ui->lineEditValueScale->text().toDouble();

  const auto& p = dataSource()->at(index);
  PlotData::Point out = { p.x + off_x, scale * p.y + off_y };
  return out;
}
}  // namespace tesseract_gui
