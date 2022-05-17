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
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <QApplication>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/workbench/workbench_widget.h>
#include <tesseract_qt/environment/environment_widget.h>
#include <tesseract_qt/environment/environment_widget_config.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_widget.h>
#include <tesseract_qt/manipulation/manipulation_widget.h>
#include <tesseract_support/tesseract_support_resource_locator.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  auto locator = std::make_shared<tesseract_common::TesseractSupportResourceLocator>();
  tesseract_common::fs::path urdf_path = std::string(TESSERACT_SUPPORT_DIR) + "/urdf/lbr_iiwa_14_r820.urdf";
  tesseract_common::fs::path srdf_path = std::string(TESSERACT_SUPPORT_DIR) + "/urdf/lbr_iiwa_14_r820.srdf";

  auto env = std::make_unique<tesseract_environment::Environment>();
  env->init(urdf_path, srdf_path, locator);

  auto config = std::make_shared<tesseract_gui::EnvironmentWidgetConfig>();
  config->setEnvironment(std::move(env));

  auto* env_widget = new tesseract_gui::EnvironmentWidget();  // NOLINT

  auto* jt_widget = new tesseract_gui::JointTrajectoryWidget();  // NOLINT

  auto manipulation_widget = new tesseract_gui::ManipulationWidget();  // NOLINT

  tesseract_gui::WorkbenchWidget widget(env_widget, jt_widget, manipulation_widget);
  env_widget->setConfiguration(std::move(config));
  widget.show();

  return QApplication::exec();
}
