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
#ifndef TESSERACT_GUI_COMMON_CARTESIAN_EDITOR_WIDGET_H
#define TESSERACT_GUI_COMMON_CARTESIAN_EDITOR_WIDGET_H

#include <QWidget>
#include <memory>
#include <Eigen/Geometry>

namespace Ui
{
class CartesianEditorWidget;
}

namespace tesseract_gui
{
class CartesianEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit CartesianEditorWidget(QWidget* parent = nullptr);
  ~CartesianEditorWidget();

Q_SIGNALS:
  void positionChanged(const Eigen::Vector3d& xyz);
  void orientationChanged(const Eigen::Quaterniond& xyzw);
  void transformChanged(const Eigen::Isometry3d& transform);

public Q_SLOTS:
  void setPosition(const Eigen::Vector3d& xyz);
  void setOrientation(const Eigen::Quaterniond& xyzw);
  void setTransform(const Eigen::Isometry3d& transform);

private Q_SLOTS:
  void onPositionChanged();
  void onRPYChanged();
  void onQuaternionChanged();

private:
  std::unique_ptr<Ui::CartesianEditorWidget> ui;

  void setPositionHelper(const Eigen::Vector3d& xyz);
  void setOrientationHelper(const Eigen::Quaterniond& xyzw);
};
}  // namespace tesseract_gui
#endif  // TESSERACT_GUI_COMMON_CARTESIAN_EDITOR_WIDGET_H
