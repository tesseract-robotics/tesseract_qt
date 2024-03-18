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
#include <QDebug>
#include <sstream>

#include <tesseract_qt/acm/widgets/allowed_collision_matrix_editor_widget.h>
#include <tesseract_qt/common/events/allowed_collision_matrix_events.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  std::shared_ptr<const tesseract_gui::ComponentInfo> component_info = tesseract_gui::ComponentInfoManager::create("sce"
                                                                                                                   "ne_"
                                                                                                                   "nam"
                                                                                                                   "e");

  tesseract_gui::AllowedCollisionMatrixEditorWidget widget(component_info);
  widget.show();

  std::vector<std::array<std::string, 3>> entries;
  entries.push_back({ "link_1", "link_2", "Adjacent" });
  entries.push_back({ "link_2", "link_3", "Adjacent" });
  entries.push_back({ "link_3", "link_4", "Adjacent" });

  tesseract_gui::events::AllowedCollisionMatrixAdd event(component_info, entries);
  QApplication::sendEvent(qApp, &event);

  return QApplication::exec();
}
