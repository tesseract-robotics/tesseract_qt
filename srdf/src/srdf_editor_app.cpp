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
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <QApplication>
#include <QStandardItemModel>
#include <QDebug>
#include <QMainWindow>
#include <QStatusBar>
#include <QTabWidget>
#include <QHBoxLayout>
#include <memory>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/srdf/srdf_editor_widget.h>
#include <tesseract_qt/rendering/render_widget.h>
#include <tesseract_qt/rendering/ign_scene_graph_render_manager.h>

#include <tesseract_qt/environment/widgets/environment_widget.h>

#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/theme_utils.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_common/resource_locator.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  // setup stylesheet
  app.setStyleSheet(tesseract_gui::themes::getDarkTheme());
  app.setApplicationName("Tesseract SRDF Editor");

  auto locator = std::make_shared<tesseract_common::GeneralResourceLocator>();
  auto component_info = tesseract_gui::ComponentInfoManager::create("srdf_scene");
  auto entity_manager = std::make_shared<tesseract_gui::EntityManager>();
  auto* srdf_widget = new tesseract_gui::SRDFEditorWidget(component_info, locator);
  auto* env_widget = new tesseract_gui::EnvironmentWidget(component_info);
  auto* render_widget = new tesseract_gui::RenderWidget(component_info->getSceneName());

  tesseract_gui::IgnSceneGraphRenderManager scene_graph_manager{ component_info, entity_manager };

  auto tab_widget = new QTabWidget();
  tab_widget->addTab(srdf_widget, "SRDF");
  tab_widget->addTab(env_widget, "Environment");

  auto central_widget = new QWidget();
  auto layout = new QHBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addWidget(tab_widget);
  layout->addWidget(render_widget, 1);
  central_widget->setLayout(layout);

  QMainWindow window;
  window.setWindowTitle("Tesseract SRDF Editor");
  window.setCentralWidget(central_widget);
  window.statusBar()->showMessage("Ready!");
  window.resize(1200, 800);

  //  QApplication::connect(srdf_widget,
  //                        &tesseract_gui::SRDFEditorWidget::environmentSet,
  //                        [env_config](const std::shared_ptr<tesseract_environment::Environment>& env) {
  //                          env_config->setEnvironment(env);
  //                        });

  //  QApplication::connect(
  //      srdf_widget, &tesseract_gui::SRDFEditorWidget::showStatusMessage, [&window](const QString& message, int
  //      timeout) {
  //        window.statusBar()->showMessage(message, timeout);
  //        window.repaint();
  //      });

  //  QApplication::connect(window.statusBar(), &QStatusBar::messageChanged, [&window](const QString& message) {
  //    if (message.isEmpty())
  //      window.statusBar()->showMessage("Ready!");

  //    window.repaint();
  //  });

  window.show();

  return QApplication::exec();
}
