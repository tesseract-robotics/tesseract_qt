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
#include "ui_workbench_widget.h"

#include <tesseract_qt/workbench/workbench_widget.h>
#include <tesseract_qt/scene_graph/widgets/scene_graph_tool_bar.h>
#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_tool_bar.h>
#include <tesseract_qt/manipulation/manipulation_tool_bar.h>
#include <tesseract_qt/planning/widgets/task_composer_tool_bar.h>
#include <tesseract_qt/common/component_info.h>

#include <QToolBar>
#include <QVBoxLayout>

namespace tesseract_gui
{
struct WorkbenchWidget::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;
  std::shared_ptr<const ComponentInfo> jt_component_info;

  SceneGraphToolBar* env_toolbar{ nullptr };
  SceneGraphToolBar* jt_env_toolbar{ nullptr };
  JointTrajectoryToolBar* jt_toolbar{ nullptr };
  ManipulationToolBar* manip_toolbar{ nullptr };
  TaskComposerToolBar* task_composer_toolbar{ nullptr };
};

WorkbenchWidget::WorkbenchWidget(QWidget* parent) : WorkbenchWidget(nullptr, parent) {}

WorkbenchWidget::WorkbenchWidget(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::WorkbenchWidget>()), data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);
  ui->tabWidget->setCurrentIndex(0);

  data_->env_toolbar = new SceneGraphToolBar();  // NOLINT
  static_cast<QVBoxLayout*>(ui->environment_widget->layout())->insertWidget(0, data_->env_toolbar);

  data_->jt_env_toolbar = new SceneGraphToolBar();  // NOLINT
  static_cast<QVBoxLayout*>(ui->jt_environment_widget->layout())->insertWidget(0, data_->jt_env_toolbar);

  data_->jt_toolbar = new JointTrajectoryToolBar();  // NOLINT
  static_cast<QVBoxLayout*>(ui->joint_trajectory_widget->layout())->insertWidget(0, data_->jt_toolbar);

  data_->manip_toolbar = new ManipulationToolBar();  // NOLINT
  static_cast<QVBoxLayout*>(ui->manipulation_widget->layout())->insertWidget(0, data_->manip_toolbar);

  data_->task_composer_toolbar = new TaskComposerToolBar();  // NOLINT
  static_cast<QVBoxLayout*>(ui->task_composer_widget->layout())->insertWidget(0, data_->task_composer_toolbar);

  if (component_info != nullptr)
    setComponentInfo(std::move(component_info));
}

WorkbenchWidget::~WorkbenchWidget() = default;

void WorkbenchWidget::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  data_->component_info = std::move(component_info);
  auto jt_component_info = data_->component_info->createChild();
  jt_component_info->setDescription("Workbench Joint Trajectory");
  data_->jt_component_info = jt_component_info;

  ui->environment_widget->setComponentInfo(data_->component_info);
  data_->env_toolbar->setComponentInfo(data_->component_info);
  data_->manip_toolbar->setComponentInfo(data_->component_info);
  ui->manipulation_widget->setComponentInfo(data_->component_info);
  ui->joint_trajectory_widget->setComponentInfo(data_->jt_component_info);
  data_->jt_toolbar->setComponentInfo(data_->jt_component_info);
  ui->jt_environment_widget->setComponentInfo(data_->jt_component_info);
  data_->jt_env_toolbar->setComponentInfo(data_->jt_component_info);
  ui->task_composer_widget->setComponentInfo(data_->component_info);
  data_->task_composer_toolbar->setComponentInfo(data_->component_info);
}

std::shared_ptr<const ComponentInfo> WorkbenchWidget::getComponentInfo() const { return data_->component_info; }

EnvironmentWidget& WorkbenchWidget::getEnvironmentWidget() { return *ui->environment_widget; }
const EnvironmentWidget& WorkbenchWidget::getEnvironmentWidget() const { return *ui->environment_widget; }

JointTrajectoryWidget& WorkbenchWidget::getJointTrajectoryWidget() { return *ui->joint_trajectory_widget; }
const JointTrajectoryWidget& WorkbenchWidget::getJointTrajectoryWidget() const { return *ui->joint_trajectory_widget; }

ManipulationWidget& WorkbenchWidget::getManipulationWidget() { return *ui->manipulation_widget; }
const ManipulationWidget& WorkbenchWidget::getManipulationWidget() const { return *ui->manipulation_widget; }

TaskComposerWidget& WorkbenchWidget::getTaskComposerWidget() { return *ui->task_composer_widget; }
const TaskComposerWidget& WorkbenchWidget::getTaskComposerWidget() const { return *ui->task_composer_widget; }

}  // namespace tesseract_gui
