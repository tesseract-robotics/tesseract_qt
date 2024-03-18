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
#include "ui_joint_trajectory_plot_dialog.h"
#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_plot_dialog.h>
#include <tesseract_qt/plot/plot_subplots.h>
#include <tesseract_qt/common/joint_trajectory_set.h>

const double SLIDER_RESOLUTION = 0.001;

namespace tesseract_gui
{
JointTrajectoryPlotDialog::JointTrajectoryPlotDialog(tesseract_common::JointTrajectoryInfo current_trajectory,
                                                     QWidget* parent)
  : QDialog(parent), ui_(std::make_unique<Ui::JointTrajectoryPlotDialog>())
{
  ui_->setupUi(this);

  ui_->jtpHorizontalSlider->setMinimum(current_trajectory.joint_trajectory.front().time / SLIDER_RESOLUTION);
  ui_->jtpHorizontalSlider->setMaximum(current_trajectory.joint_trajectory.back().time / SLIDER_RESOLUTION);
  ui_->jtpHorizontalSlider->setSliderPosition(0);

  plot_data_map_.clear();

  const std::vector<std::string> joint_names = current_trajectory.joint_trajectory[0].joint_names;

  for (std::size_t i = 0; i < joint_names.size(); ++i)
  {
    tesseract_gui::PlotData& position = plot_data_map_.getOrCreateNumeric(joint_names[i] + "::position");
    tesseract_gui::PlotData& velocity = plot_data_map_.getOrCreateNumeric(joint_names[i] + "::velocity");
    tesseract_gui::PlotData& acceleration = plot_data_map_.getOrCreateNumeric(joint_names[i] + "::acceleration");
    double ct{ 0 };
    for (const auto& state : current_trajectory.joint_trajectory)
    {
      position.pushBack(tesseract_gui::PlotDataXY::Point(state.time, state.position(i)));
      velocity.pushBack(tesseract_gui::PlotDataXY::Point(state.time, state.velocity(i)));
      acceleration.pushBack(tesseract_gui::PlotDataXY::Point(state.time, state.acceleration(i)));
    }
  }

  position_subplots_ = new tesseract_gui::PlotSubplots(plot_data_map_, joint_names.size(), 1);
  for (std::size_t i = 0; i < joint_names.size(); ++i)
  {
    tesseract_gui::PlotWidget* widget = position_subplots_->getSubplot(i, 0);
    widget->addCurve(joint_names[i] + "::position");
    if (i == joint_names.size() - 1)
      widget->setXAxisTitle("Time (seconds)");
    widget->setYAxisTitle("Units (rad)");
    widget->enableTracker(true);
    widget->zoomOut(true);
    widget->replot();

    QObject::connect(widget, &tesseract_gui::PlotWidget::trackerMoved, [this](QPointF pos) {
      onSliderMoved(pos.x() / SLIDER_RESOLUTION);
      this->ui_->jtpHorizontalSlider->blockSignals(true);
      this->ui_->jtpHorizontalSlider->setValue(pos.x() / SLIDER_RESOLUTION);
      this->ui_->jtpHorizontalSlider->blockSignals(false);
    });
  }

  ui_->jtpTabWidget->addTab(position_subplots_, tr("Position"));

  connect(ui_->jtpTabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));

  velocity_subplots_ = new tesseract_gui::PlotSubplots(plot_data_map_, joint_names.size(), 1);
  for (std::size_t i = 0; i < joint_names.size(); ++i)
  {
    tesseract_gui::PlotWidget* widget = velocity_subplots_->getSubplot(i, 0);
    widget->addCurve(joint_names[i] + "::velocity");
    if (i == joint_names.size() - 1)
      widget->setXAxisTitle("Time (seconds)");
    widget->setYAxisTitle("Units (rad/sec)");
    widget->enableTracker(true);
    widget->zoomOut(true);
    widget->replot();

    QObject::connect(widget, &tesseract_gui::PlotWidget::trackerMoved, [this](QPointF pos) {
      onSliderMoved(pos.x() / SLIDER_RESOLUTION);
      this->ui_->jtpHorizontalSlider->blockSignals(true);
      this->ui_->jtpHorizontalSlider->setValue(pos.x() / SLIDER_RESOLUTION);
      this->ui_->jtpHorizontalSlider->blockSignals(false);
    });
  }

  ui_->jtpTabWidget->addTab(velocity_subplots_, tr("Velocity"));
  connect(ui_->jtpTabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));

  acceleration_subplots_ = new tesseract_gui::PlotSubplots(plot_data_map_, joint_names.size(), 1);
  for (std::size_t i = 0; i < joint_names.size(); ++i)
  {
    tesseract_gui::PlotWidget* widget = acceleration_subplots_->getSubplot(i, 0);
    widget->addCurve(joint_names[i] + "::acceleration");
    if (i == joint_names.size() - 1)
      widget->setXAxisTitle("Time (seconds)");
    widget->setYAxisTitle("Units (rad/sec^2)");
    widget->enableTracker(true);
    widget->zoomOut(true);
    widget->replot();

    QObject::connect(widget, &tesseract_gui::PlotWidget::trackerMoved, [this](QPointF pos) {
      onSliderMoved(pos.x() / SLIDER_RESOLUTION);
      this->ui_->jtpHorizontalSlider->blockSignals(true);
      this->ui_->jtpHorizontalSlider->setValue(pos.x() / SLIDER_RESOLUTION);
      this->ui_->jtpHorizontalSlider->blockSignals(false);
    });
  }

  ui_->jtpTabWidget->addTab(acceleration_subplots_, tr("Acceleration"));
  connect(ui_->jtpTabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));

  connect(ui_->jtpHorizontalSlider, SIGNAL(sliderMoved(int)), this, SLOT(onSliderMoved(int)));
}

JointTrajectoryPlotDialog::~JointTrajectoryPlotDialog() = default;

void JointTrajectoryPlotDialog::onSliderMoved(int value)
{
  const double t = SLIDER_RESOLUTION * value;

  for (int r = 0; r < position_subplots_->rows(); ++r)
  {
    for (int c = 0; c < position_subplots_->cols(); ++c)
    {
      auto* w = position_subplots_->getSubplot(r, c);
      w->setTrackerPosition(t);
      w->replot();
    }
  }

  for (int r = 0; r < velocity_subplots_->rows(); ++r)
  {
    for (int c = 0; c < velocity_subplots_->cols(); ++c)
    {
      auto* w = velocity_subplots_->getSubplot(r, c);
      w->setTrackerPosition(t);
      w->replot();
    }
  }

  for (int r = 0; r < acceleration_subplots_->rows(); ++r)
  {
    for (int c = 0; c < acceleration_subplots_->cols(); ++c)
    {
      auto* w = acceleration_subplots_->getSubplot(r, c);
      w->setTrackerPosition(t);
      w->replot();
    }
  }
}

void JointTrajectoryPlotDialog::onTabIndexChanged(int value)
{
  if (value == 0)
    position_subplots_->replot();
  else if (value == 1)
    velocity_subplots_->replot();
  else if (value == 2)
    acceleration_subplots_->replot();
}
}  // namespace tesseract_gui
