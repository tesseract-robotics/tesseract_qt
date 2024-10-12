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

#include <QApplication>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QDebug>

#include <tesseract_qt/scene_graph/widgets/scene_graph_widget.h>
#include <tesseract_qt/scene_graph/widgets/scene_graph_tool_bar.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/widgets/tree_view.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>

#include <tesseract_urdf/urdf_parser.h>
#include <tesseract_scene_graph/graph.h>
#include <tesseract_common/resource_locator.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  tesseract_common::GeneralResourceLocator locator;
  std::string path = locator.locateResource("package://tesseract_support/urdf/lbr_iiwa_14_r820.urdf")->getFilePath();

  auto scene_graph = tesseract_urdf::parseURDFFile(path, locator);

  std::shared_ptr<const tesseract_gui::ComponentInfo> component_info = tesseract_gui::ComponentInfoManager::create("sce"
                                                                                                                   "ne_"
                                                                                                                   "nam"
                                                                                                                   "e");

  QWidget widget;
  auto layout = new QVBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addWidget(new tesseract_gui::SceneGraphToolBar(component_info));
  layout->addWidget(new tesseract_gui::SceneGraphWidget(component_info), 1);
  widget.setLayout(layout);
  widget.show();

  tesseract_gui::events::SceneGraphSet event(component_info, std::move(scene_graph));
  QApplication::sendEvent(qApp, &event);

  return app.exec();
}
