/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
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
#include <tesseract_qt/joint_state_slider/joint_state_slider_widget.h>
#include "ui_joint_state_slider_widget.h"

#include <QLabel>
#include <QSlider>
#include <QGridLayout>
#include <QFrame>
#include <QDoubleSpinBox>

const double SLIDER_RESOLUTION = 0.001;

namespace tesseract_gui
{
struct JointStateSliderWidgetPrivate
{
  QGridLayout* layout;
  std::unordered_map<std::string, double> state;
  std::unordered_map<std::string, QSlider*> sliders;
  std::unordered_map<std::string, QDoubleSpinBox*> spinboxes;
};

JointStateSliderWidget::JointStateSliderWidget(QWidget* parent)
  : QWidget(parent)
  , ui_(std::make_unique<Ui::JointStateSliderWidget>())
  , data_(std::make_unique<JointStateSliderWidgetPrivate>())
{
  ui_->setupUi(this);

  data_->layout = new QGridLayout();
  setLayout(data_->layout);
}

JointStateSliderWidget::~JointStateSliderWidget() = default;

void JointStateSliderWidget::setJoints(const std::vector<tesseract_scene_graph::Joint::ConstPtr>& joints)
{
  data_->state.clear();
  data_->sliders.clear();
  data_->spinboxes.clear();
  while (data_->layout->count() > 0)
  {
    QLayoutItem* item = data_->layout->takeAt(0);
    delete item->widget();
    delete item;
  }

  int row{ 0 };
  for (const auto& joint : joints)
  {
    std::string name = joint->getName();
    double range = (joint->limits->upper - joint->limits->lower);
    double value = joint->limits->lower + (range / 2.0);
    data_->state[name] = value;

    auto* name_lable = new QLabel();
    name_lable->setText(QString("%1:").arg(QString::fromStdString(joint->getName())));
    data_->layout->addWidget(name_lable, row, 0);

    auto* min_lable = new QLabel();
    min_lable->setFrameShape(QFrame::StyledPanel);
    min_lable->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed));
    min_lable->setText(QString("").setNum(joint->limits->lower, 'g', 3));
    min_lable->setAlignment(Qt::AlignmentFlag::AlignRight);
    data_->layout->addWidget(min_lable, row, 1);

    auto* slider = new QSlider();
    slider->setOrientation(Qt::Orientation::Horizontal);
    slider->setMinimum(joint->limits->lower / SLIDER_RESOLUTION);
    slider->setMaximum(joint->limits->upper / SLIDER_RESOLUTION);
    slider->setValue(value / SLIDER_RESOLUTION);
    data_->layout->addWidget(slider, row, 2);
    data_->sliders[joint->getName()] = slider;

    auto* max_lable = new QLabel();
    max_lable->setFrameShape(QFrame::StyledPanel);
    max_lable->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed));
    max_lable->setText(QString("").setNum(joint->limits->upper, 'g', 3));
    max_lable->setAlignment(Qt::AlignmentFlag::AlignRight);
    data_->layout->addWidget(max_lable, row, 3);

    auto* spin = new QDoubleSpinBox();
    spin->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed));
    spin->setMinimum(joint->limits->lower);
    spin->setMaximum(joint->limits->upper);
    spin->setValue(value);
    spin->setDecimals(6);
    spin->setSingleStep(SLIDER_RESOLUTION);
    spin->setFixedWidth(100);
    data_->layout->addWidget(spin, row, 4);
    data_->spinboxes[joint->getName()] = spin;

    connect(slider, &QSlider::sliderMoved, spin, [this, name, spin](int value) {
      double v = value * SLIDER_RESOLUTION;
      spin->blockSignals(true);
      spin->setValue(v);
      spin->blockSignals(false);
      data_->state[name] = v;
      Q_EMIT jointValueChanged(QString::fromStdString(name), v);
      Q_EMIT jointStateChanged(data_->state);
    });

    connect(spin, qOverload<double>(&QDoubleSpinBox::valueChanged), slider, [this, name, slider](double value) {
      slider->blockSignals(true);
      slider->setValue(value / SLIDER_RESOLUTION);
      slider->blockSignals(false);
      data_->state[name] = value;
      Q_EMIT jointValueChanged(QString::fromStdString(name), value);
      Q_EMIT jointStateChanged(data_->state);
    });

    ++row;
  }
  data_->layout->setColumnStretch(2, 1);
  data_->layout->setColumnStretch(0, 0);
  Q_EMIT jointStateChanged(data_->state);
}

std::unordered_map<std::string, double> JointStateSliderWidget::getJointState() const { return data_->state; }

void JointStateSliderWidget::setJointState(const std::unordered_map<std::string, double>& state)
{
  for (const auto& v : state)
  {
    auto it_slider = data_->sliders.find(v.first);
    auto it_spinbox = data_->spinboxes.find(v.first);
    if (it_slider != data_->sliders.end() && it_spinbox != data_->spinboxes.end())
    {
      it_slider->second->setValue(v.second / SLIDER_RESOLUTION);
      it_spinbox->second->setValue(v.second);
    }
  }
}

}  // namespace tesseract_gui
