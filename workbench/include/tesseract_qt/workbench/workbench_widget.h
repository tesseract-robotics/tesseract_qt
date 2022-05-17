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
#ifndef TESSERACT_QT_WORKBENCH_WORKBENCH_WIDGET_H
#define TESSERACT_QT_WORKBENCH_WORKBENCH_WIDGET_H

#include <QWidget>
#include <memory>

namespace Ui
{
class WorkbenchWidget;
}

namespace tesseract_environment
{
class Environment;
}

namespace tesseract_common
{
class JointTrajectorySet;
class JointState;
}  // namespace tesseract_common

namespace tesseract_gui
{
class EnvironmentWidget;
class JointTrajectoryWidget;
class ManipulationWidget;

struct WorkbenchWidgetImpl;

class WorkbenchWidget : public QWidget
{
  Q_OBJECT

public:
  explicit WorkbenchWidget(EnvironmentWidget* environment_widget,
                           JointTrajectoryWidget* joint_trajectory_widget,
                           ManipulationWidget* manipulation_widget,
                           QWidget* parent = nullptr);
  ~WorkbenchWidget();

  EnvironmentWidget& getEnvironmentWidget();
  const EnvironmentWidget& getEnvironmentWidget() const;

  JointTrajectoryWidget& getJointTrajectoryWidget();
  const JointTrajectoryWidget& getJointTrajectoryWidget() const;

public Q_SLOTS:
  virtual void onRender();
  virtual void onEnable();

private Q_SLOTS:
  void onEnvironmentSet(const std::shared_ptr<tesseract_environment::Environment>& env);
  void onConfigureJointTrajectorySet(const QString& uuid,
                                     const tesseract_common::JointTrajectorySet& joint_trajectory_set);
  void onJointTrajectorySetRemoved(const QString& uuid);
  void onJointTrajectorySetState(const tesseract_common::JointState& state);

private:
  std::unique_ptr<Ui::WorkbenchWidget> ui;
  std::unique_ptr<WorkbenchWidgetImpl> data_;
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_WORKBENCH_WORKBENCH_WIDGET_H
