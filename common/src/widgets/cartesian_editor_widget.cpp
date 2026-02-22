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
#include <tesseract_qt/common/widgets/cartesian_editor_widget.h>
#include "ui_cartesian_editor_widget.h"

namespace tesseract::gui
{
CartesianEditorWidget::CartesianEditorWidget(QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::CartesianEditorWidget>())
{
  ui->setupUi(this);

  connect(ui->x_value, SIGNAL(valueChanged(double)), this, SLOT(onPositionChanged()));
  connect(ui->y_value, SIGNAL(valueChanged(double)), this, SLOT(onPositionChanged()));
  connect(ui->z_value, SIGNAL(valueChanged(double)), this, SLOT(onPositionChanged()));

  connect(ui->roll_value, SIGNAL(valueChanged(double)), this, SLOT(onRPYChanged()));
  connect(ui->pitch_value, SIGNAL(valueChanged(double)), this, SLOT(onRPYChanged()));
  connect(ui->yaw_value, SIGNAL(valueChanged(double)), this, SLOT(onRPYChanged()));

  connect(ui->qx_value, SIGNAL(valueChanged(double)), this, SLOT(onQuaternionChanged()));
  connect(ui->qy_value, SIGNAL(valueChanged(double)), this, SLOT(onQuaternionChanged()));
  connect(ui->qz_value, SIGNAL(valueChanged(double)), this, SLOT(onQuaternionChanged()));
  connect(ui->qw_value, SIGNAL(valueChanged(double)), this, SLOT(onQuaternionChanged()));
}

CartesianEditorWidget::~CartesianEditorWidget() = default;

void CartesianEditorWidget::onPositionChanged()
{
  Eigen::Vector3d xyz(ui->x_value->value(), ui->y_value->value(), ui->z_value->value());
  Q_EMIT positionChanged(xyz);

  Eigen::Isometry3d tf{ Eigen::Isometry3d::Identity() };
  tf.translation() = xyz;

  Eigen::Quaterniond q(ui->qw_value->value(), ui->qx_value->value(), ui->qy_value->value(), ui->qz_value->value());
  q.normalize();
  tf.linear() = q.toRotationMatrix();
  Q_EMIT transformChanged(tf);
}

void CartesianEditorWidget::onRPYChanged()
{
  Eigen::AngleAxisd rollAngle(ui->roll_value->value(), Eigen::Vector3d::UnitX());
  Eigen::AngleAxisd pitchAngle(ui->pitch_value->value(), Eigen::Vector3d::UnitY());
  Eigen::AngleAxisd yawAngle(ui->yaw_value->value(), Eigen::Vector3d::UnitZ());

  Eigen::Quaterniond q = yawAngle * pitchAngle * rollAngle;

  ui->qx_value->blockSignals(true);
  ui->qy_value->blockSignals(true);
  ui->qz_value->blockSignals(true);
  ui->qw_value->blockSignals(true);
  ui->qx_value->setValue(q.x());
  ui->qy_value->setValue(q.y());
  ui->qz_value->setValue(q.z());
  ui->qw_value->setValue(q.w());
  ui->qx_value->blockSignals(false);
  ui->qy_value->blockSignals(false);
  ui->qz_value->blockSignals(false);
  ui->qw_value->blockSignals(false);

  Q_EMIT orientationChanged(q);

  Eigen::Isometry3d tf{ Eigen::Isometry3d::Identity() };
  tf.translation() = Eigen::Vector3d(ui->x_value->value(), ui->y_value->value(), ui->z_value->value());
  tf.linear() = q.toRotationMatrix();
  Q_EMIT transformChanged(tf);
}

void CartesianEditorWidget::onQuaternionChanged()
{
  Eigen::Quaterniond q(ui->qw_value->value(), ui->qx_value->value(), ui->qy_value->value(), ui->qz_value->value());
  q.normalize();

  Eigen::Vector3d ea = q.toRotationMatrix().eulerAngles(0, 1, 2);
  ui->roll_value->blockSignals(true);
  ui->pitch_value->blockSignals(true);
  ui->yaw_value->blockSignals(true);
  ui->roll_value->setValue(ea.x());
  ui->pitch_value->setValue(ea.y());
  ui->yaw_value->setValue(ea.z());
  ui->roll_value->blockSignals(false);
  ui->pitch_value->blockSignals(false);
  ui->yaw_value->blockSignals(false);

  Q_EMIT orientationChanged(q);

  Eigen::Isometry3d tf{ Eigen::Isometry3d::Identity() };
  tf.translation() = Eigen::Vector3d(ui->x_value->value(), ui->y_value->value(), ui->z_value->value());
  tf.linear() = q.toRotationMatrix();
  Q_EMIT transformChanged(tf);
}

void CartesianEditorWidget::setPosition(const Eigen::Vector3d& xyz)
{
  setPositionHelper(xyz);

  Eigen::Isometry3d tf{ Eigen::Isometry3d::Identity() };
  tf.translation() = xyz;

  Eigen::Quaterniond q(ui->qw_value->value(), ui->qx_value->value(), ui->qy_value->value(), ui->qz_value->value());
  q.normalize();
  tf.linear() = q.toRotationMatrix();
  Q_EMIT transformChanged(tf);
}

void CartesianEditorWidget::setOrientation(const Eigen::Quaterniond& xyzw)
{
  Eigen::Quaterniond q(xyzw);
  q.normalize();

  setOrientation(q);

  Eigen::Isometry3d tf{ Eigen::Isometry3d::Identity() };
  tf.translation() = Eigen::Vector3d(ui->x_value->value(), ui->y_value->value(), ui->z_value->value());
  tf.linear() = q.toRotationMatrix();
  Q_EMIT transformChanged(tf);
}

void CartesianEditorWidget::setTransform(const Eigen::Isometry3d& transform)
{
  setPositionHelper(transform.translation());

  Eigen::Quaterniond q(transform.rotation());
  setOrientationHelper(q);

  Q_EMIT transformChanged(transform);
}

void CartesianEditorWidget::setPositionHelper(const Eigen::Vector3d& xyz)
{
  ui->x_value->blockSignals(true);
  ui->y_value->blockSignals(true);
  ui->z_value->blockSignals(true);
  ui->x_value->setValue(xyz.x());
  ui->y_value->setValue(xyz.y());
  ui->z_value->setValue(xyz.z());
  ui->x_value->blockSignals(false);
  ui->y_value->blockSignals(false);
  ui->z_value->blockSignals(false);

  Q_EMIT positionChanged(xyz);
}

void CartesianEditorWidget::setOrientationHelper(const Eigen::Quaterniond& xyzw)
{
  Eigen::Vector3d ea = xyzw.toRotationMatrix().eulerAngles(0, 1, 2);
  ui->roll_value->blockSignals(true);
  ui->pitch_value->blockSignals(true);
  ui->yaw_value->blockSignals(true);
  ui->roll_value->setValue(ea.x());
  ui->pitch_value->setValue(ea.y());
  ui->yaw_value->setValue(ea.z());
  ui->roll_value->blockSignals(false);
  ui->pitch_value->blockSignals(false);
  ui->yaw_value->blockSignals(false);

  ui->qx_value->blockSignals(true);
  ui->qy_value->blockSignals(true);
  ui->qz_value->blockSignals(true);
  ui->qw_value->blockSignals(true);
  ui->qx_value->setValue(xyzw.x());
  ui->qy_value->setValue(xyzw.y());
  ui->qz_value->setValue(xyzw.z());
  ui->qw_value->setValue(xyzw.w());
  ui->qx_value->blockSignals(false);
  ui->qy_value->blockSignals(false);
  ui->qz_value->blockSignals(false);
  ui->qw_value->blockSignals(false);

  Q_EMIT orientationChanged(xyzw);
}

}  // namespace tesseract::gui
