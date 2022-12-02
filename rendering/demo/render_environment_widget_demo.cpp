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
#include <QDebug>
#include <QHBoxLayout>
#include <memory>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/rendering/render_widget.h>
#include <tesseract_qt/rendering/render_environment_widget.h>
#include <tesseract_qt/rendering/tool_path_render_manager.h>
#include <tesseract_qt/environment/environment_widget_config.h>
#include <tesseract_qt/tool_path/tool_path_events.h>
#include <tesseract_qt/common/tool_path.h>

#include <tesseract_support/tesseract_support_resource_locator.h>
#include <tesseract_environment/environment.h>

tesseract_gui::ToolPath getToolPath()
{
  tesseract_gui::ToolPath tool_path("Demo Tool Path");
  for (int i = 0; i < 5; ++i)
  {
    tesseract_gui::ToolPathSegment segment("Segment [" + std::to_string(i) + "]");

    Eigen::Isometry3d pose{ Eigen::Isometry3d::Identity() };
    for (Eigen::Index k = 0; k < 10; ++k)
    {
      pose.translation() = Eigen::Vector3d(-0.5 + k * 0.1, 0.5 - i * 0.05, 0.5);
      segment.push_back(pose);
    }

    tool_path.push_back(segment);
  }

  return tool_path;
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  auto locator = std::make_shared<tesseract_common::TesseractSupportResourceLocator>();
  tesseract_common::fs::path urdf_path = std::string(TESSERACT_SUPPORT_DIR) + "/urdf/lbr_iiwa_14_r820.urdf";
  tesseract_common::fs::path srdf_path = std::string(TESSERACT_SUPPORT_DIR) + "/urdf/lbr_iiwa_14_r820.srdf";

  auto env = std::make_unique<tesseract_environment::Environment>();
  env->init(urdf_path, srdf_path, locator);

  std::string scene_name = env->getName();
  auto render_widget = new tesseract_gui::RenderWidget(scene_name);
  render_widget->setSkyEnabled(true);

  auto config = std::make_shared<tesseract_gui::EnvironmentWidgetConfig>();
  config->setEnvironment(std::move(env));

  auto entity_manager = std::make_shared<tesseract_gui::EntityManager>();
  auto* env_widget = new tesseract_gui::RenderEnvironmentWidget(scene_name, *entity_manager);
  env_widget->setConfiguration(std::move(config));

  tesseract_gui::ToolPathRenderManager tool_path_manager(scene_name, entity_manager);

  QWidget widget;
  auto layout = new QHBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addWidget(env_widget);
  layout->addWidget(render_widget, 1);
  widget.setLayout(layout);

  //  QObject::connect(env_widget, SIGNAL(triggerRender()), render_widget, SLOT(update()));

  tesseract_gui::ToolPath tool_path = getToolPath();
  QApplication::sendEvent(qApp, new tesseract_gui::events::ToolPathAdd(scene_name, tool_path));

  widget.resize(1200, 800);
  widget.show();

  return app.exec();
}
