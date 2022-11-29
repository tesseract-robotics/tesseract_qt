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

#include <tesseract_qt/tool_path/tool_path_tool_bar.h>
#include <tesseract_qt/tool_path/tool_path_events.h>
#include <tesseract_qt/common/icon_utils.h>
#include <QApplication>

namespace tesseract_gui
{
struct ToolPathToolBarImpl
{
  std::string scene_name;

  QAction* remove_all;
  QAction* remove_selected;
  QAction* hide_all;
  QAction* show_all;
};

ToolPathToolBar::ToolPathToolBar(const std::string& scene_name, QWidget* parent)
  : QToolBar(parent), data_(std::make_unique<ToolPathToolBarImpl>())
{
  data_->scene_name = scene_name;
  data_->remove_all = addAction(icons::getClearIcon(), "Remove All", [scene_name]() {
    QApplication::sendEvent(qApp, new events::ToolPathRemoveAll(scene_name));
  });
  data_->remove_selected = addAction(icons::getTrashIcon(), "Remove Selected", [scene_name]() {
    QApplication::sendEvent(qApp, new events::ToolPathRemoveSelected(scene_name));
  });
  addSeparator();
  data_->hide_all = addAction(icons::getToolPathHideIcon(), "Hide All", [scene_name]() {
    QApplication::sendEvent(qApp, new events::ToolPathHideAll(scene_name));
  });
  data_->show_all = addAction(icons::getToolPathShowIcon(), "Show All", [scene_name]() {
    QApplication::sendEvent(qApp, new events::ToolPathShowAll(scene_name));
  });
}

ToolPathToolBar::~ToolPathToolBar() = default;
}  // namespace tesseract_gui
