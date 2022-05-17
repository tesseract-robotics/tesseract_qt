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
#include "ui_workbench_widget.h"

#include <tesseract_qt/workbench/workbench_widget.h>
#include <tesseract_qt/environment/environment_widget.h>
#include <tesseract_qt/environment/environment_widget_config.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_widget.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_model.h>
#include <tesseract_qt/manipulation/manipulation_widget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>

namespace tesseract_gui
{
struct WorkbenchWidgetImpl
{
  EnvironmentWidget* environment_widget;
  EnvironmentWidget* joint_trajectory_environment_widget;
  JointTrajectoryWidget* joint_trajectory_widget;
  JointTrajectoryModel* joint_trajectory_model;
  ManipulationWidget* manipulation_widget;

  QString current_uuid;
  std::unordered_map<std::string, tesseract_gui::EnvironmentWidgetConfig::Ptr> joint_trajectory_environment_configs;
};

WorkbenchWidget::WorkbenchWidget(EnvironmentWidget* environment_widget,
                                 JointTrajectoryWidget* joint_trajectory_widget,
                                 tesseract_gui::ManipulationWidget* manipulation_widget,
                                 QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::WorkbenchWidget>()), data_(std::make_unique<WorkbenchWidgetImpl>())
{
  ui->setupUi(this);
  ui->tabWidget->setCurrentIndex(0);
  data_->environment_widget = environment_widget;
  data_->joint_trajectory_environment_widget = environment_widget->clone();
  data_->joint_trajectory_widget = joint_trajectory_widget;
  data_->joint_trajectory_model = new JointTrajectoryModel();  // NOLINT
  data_->joint_trajectory_widget->setModel(data_->joint_trajectory_model);
  data_->manipulation_widget = manipulation_widget;

  connect(data_->environment_widget,
          SIGNAL(environmentSet(std::shared_ptr<tesseract_environment::Environment>)),
          this,
          SLOT(onEnvironmentSet(std::shared_ptr<tesseract_environment::Environment>)));
  connect(data_->joint_trajectory_widget,
          SIGNAL(configureJointTrajectorySet(QString, tesseract_common::JointTrajectorySet)),
          this,
          SLOT(onConfigureJointTrajectorySet(QString, tesseract_common::JointTrajectorySet)));
  connect(data_->joint_trajectory_widget,
          SIGNAL(jointTrajectorySetRemoved(QString)),
          this,
          SLOT(onJointTrajectorySetRemoved(QString)));
  connect(data_->joint_trajectory_widget,
          SIGNAL(showJointState(tesseract_common::JointState)),
          this,
          SLOT(onJointTrajectorySetState(tesseract_common::JointState)));

  {                                    // Add environment widget
    auto* layout = new QVBoxLayout();  // NOLINT
    layout->setMargin(3);
    layout->addWidget(environment_widget);
    ui->environment_tab->setLayout(layout);
  }

  {                                    // Add joint trajectory widget
    auto* layout = new QHBoxLayout();  // NOLINT
    layout->setMargin(3);
    layout->addWidget(data_->joint_trajectory_environment_widget);
    layout->addWidget(data_->joint_trajectory_widget);
    ui->trajectory_tab->setLayout(layout);
  }

  {                                    // Add manipulation widget
    auto* layout = new QHBoxLayout();  // NOLINT
    layout->setMargin(3);
    layout->addWidget(data_->manipulation_widget);
    ui->manipulation_tab->setLayout(layout);
  }
}

WorkbenchWidget::~WorkbenchWidget() = default;

EnvironmentWidget& WorkbenchWidget::getEnvironmentWidget() { return *data_->environment_widget; }
const EnvironmentWidget& WorkbenchWidget::getEnvironmentWidget() const { return *data_->environment_widget; }

JointTrajectoryWidget& WorkbenchWidget::getJointTrajectoryWidget() { return *data_->joint_trajectory_widget; }
const JointTrajectoryWidget& WorkbenchWidget::getJointTrajectoryWidget() const
{
  return *data_->joint_trajectory_widget;
}

void WorkbenchWidget::onRender()
{
  data_->environment_widget->onRender();
  data_->joint_trajectory_environment_widget->onRender();
  data_->manipulation_widget->onRender();
}

void WorkbenchWidget::onEnable()
{
  data_->environment_widget->onEnable();
  data_->joint_trajectory_environment_widget->onEnable();
  data_->joint_trajectory_widget->onEnable();
  data_->manipulation_widget->onEnable();
}

void WorkbenchWidget::onEnvironmentSet(const std::shared_ptr<tesseract_environment::Environment>& env)
{
  data_->joint_trajectory_widget->setDefaultEnvironment(env);
  data_->manipulation_widget->setEnvironment(env);
}

void WorkbenchWidget::onConfigureJointTrajectorySet(const QString& uuid,
                                                    const tesseract_common::JointTrajectorySet& joint_trajectory_set)
{
  auto it = data_->joint_trajectory_environment_configs.find(uuid.toStdString());
  if (it != data_->joint_trajectory_environment_configs.end())
  {
    data_->joint_trajectory_environment_widget->setConfiguration(it->second);
  }
  else
  {
    auto config = std::make_shared<tesseract_gui::EnvironmentWidgetConfig>();
    config->setEnvironment(joint_trajectory_set.getEnvironment());
    data_->joint_trajectory_environment_widget->setConfiguration(config);
    data_->joint_trajectory_environment_configs[uuid.toStdString()] = config;
    data_->current_uuid = uuid;
  }

  auto initial_state = joint_trajectory_set.getInitialState();
  data_->joint_trajectory_environment_widget->environment().setState(initial_state.joint_names, initial_state.position);
}

void WorkbenchWidget::onJointTrajectorySetState(const tesseract_common::JointState& state)
{
  data_->joint_trajectory_environment_widget->environment().setState(state.joint_names, state.position);
}

void WorkbenchWidget::onJointTrajectorySetRemoved(const QString& uuid)
{
  data_->joint_trajectory_environment_configs.erase(uuid.toStdString());
  if (uuid == data_->current_uuid)
  {
    data_->joint_trajectory_environment_widget->setConfiguration(
        std::make_shared<tesseract_gui::EnvironmentWidgetConfig>());
    data_->current_uuid.clear();
  }
}

}  // namespace tesseract_gui
