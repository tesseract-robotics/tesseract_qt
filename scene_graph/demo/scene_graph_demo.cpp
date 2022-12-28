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
#include <QVBoxLayout>
#include <QDebug>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/scene_graph/scene_graph_model.h>
#include <tesseract_qt/scene_graph/scene_graph_standard_item.h>
#include <tesseract_qt/scene_graph/scene_graph_tool_bar.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/tree_view.h>
#include <tesseract_urdf/urdf_parser.h>
#include <tesseract_support/tesseract_support_resource_locator.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  std::string path = std::string(TESSERACT_SUPPORT_DIR) + "/urdf/lbr_iiwa_14_r820.urdf";

  tesseract_common::TesseractSupportResourceLocator locator;
  auto scene_graph = tesseract_urdf::parseURDFFile(path, locator);

  tesseract_gui::ComponentInfo component_info{ "scene_name" };
  auto* model = new tesseract_gui::SceneGraphModel(component_info);

  auto* scene_graph_widget = new tesseract_gui::TreeView();
  scene_graph_widget->setModel(model);

  QWidget widget;
  auto layout = new QVBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addWidget(new tesseract_gui::SceneGraphToolBar(component_info));
  layout->addWidget(scene_graph_widget, 1);
  widget.setLayout(layout);
  widget.show();

  QApplication::sendEvent(qApp, new tesseract_gui::events::SceneGraphSet(component_info, std::move(scene_graph)));

  return app.exec();
}
