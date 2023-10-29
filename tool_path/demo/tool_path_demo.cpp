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
#include <QVBoxLayout>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/tool_path/widgets/tool_path_widget.h>
#include <tesseract_qt/tool_path/widgets/tool_path_tool_bar.h>
#include <tesseract_qt/common/events/tool_path_events.h>
#include <tesseract_qt/common/widgets/tree_view.h>
#include <tesseract_qt/common/tool_path.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  Q_INIT_RESOURCE(tesseract_qt_resources);

  tesseract_gui::ToolPath tool_path("Demo Tool Path");
  for (int i = 0; i < 5; ++i)
  {
    tesseract_gui::ToolPathSegment segment("Segment [" + std::to_string(i) + "]");

    for (int j = 0; j < 5; ++j)
    {
      Eigen::Isometry3d pose{ Eigen::Isometry3d::Identity() };
      for (Eigen::Index k = 0; k < 5; ++k)
        pose.translation() = Eigen::Vector3d(std::rand(), std::rand(), std::rand());

      segment.push_back(pose);
    }
    tool_path.push_back(segment);
  }

  tesseract_gui::ToolPath tool_path1{ tool_path };
  tool_path1.setDescription("Demo Tool Path 1");
  tool_path1.regenerateUUID();

  tesseract_gui::ToolPath tool_path2{ tool_path };
  tool_path2.setDescription("Demo Tool Path 2");
  tool_path2.regenerateUUID();

  tesseract_gui::ToolPath tool_path3{ tool_path };
  tool_path3.setDescription("Demo Tool Path 3");
  tool_path3.regenerateUUID();

  tesseract_gui::ToolPath tool_path4{ tool_path };
  tool_path4.setDescription("Demo Tool Path 4");
  tool_path4.regenerateUUID();

  auto component_info = tesseract_gui::ComponentInfoManager::create("scene_name");

  QWidget widget;
  auto layout = new QVBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addWidget(new tesseract_gui::ToolPathToolBar(component_info));
  layout->addWidget(new tesseract_gui::ToolPathWidget(component_info), 1);
  widget.setLayout(layout);
  widget.show();

  tesseract_gui::events::ToolPathAdd event(component_info, tool_path);
  QApplication::sendEvent(qApp, &event);

  tesseract_gui::events::ToolPathAdd event1(component_info, tool_path1);
  QApplication::sendEvent(qApp, &event1);

  tesseract_gui::events::ToolPathAdd event2(component_info, tool_path2);
  QApplication::sendEvent(qApp, &event2);

  tesseract_gui::events::ToolPathAdd event3(component_info, tool_path3);
  QApplication::sendEvent(qApp, &event3);

  tesseract_gui::events::ToolPathAdd event4(component_info, tool_path4);
  QApplication::sendEvent(qApp, &event4);

  tesseract_gui::events::ToolPathRemove event5(component_info, tool_path3.getUUID());
  QApplication::sendEvent(qApp, &event5);

  return QApplication::exec();
}
