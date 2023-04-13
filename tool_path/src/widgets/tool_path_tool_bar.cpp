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

#include <tesseract_qt/tool_path/widgets/tool_path_tool_bar.h>
#include <tesseract_qt/common/events/tool_path_events.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>
#include <QApplication>

namespace tesseract_gui
{
struct ToolPathToolBar::Implementation
{
  ComponentInfo component_info;

  QAction* remove_all;
  QAction* remove_selected;
  QAction* hide_all;
  QAction* show_all;
  QAction* open_action;
  QAction* save_action;
};

ToolPathToolBar::ToolPathToolBar(QWidget* parent) : ToolPathToolBar(ComponentInfo(), parent) {}

ToolPathToolBar::ToolPathToolBar(ComponentInfo component_info, QWidget* parent)
  : QToolBar(parent), data_(std::make_unique<Implementation>())
{
  data_->component_info = component_info;
  data_->remove_all = addAction(icons::getClearIcon(), "Remove All", [component_info]() {
    QApplication::sendEvent(qApp, new events::ToolPathRemoveAll(component_info));
  });
  data_->remove_selected = addAction(icons::getTrashIcon(), "Remove Selected", [component_info]() {
    QApplication::sendEvent(qApp, new events::ToolPathRemoveSelected(component_info));
  });
  addSeparator();
  data_->hide_all = addAction(icons::getToolPathHideIcon(), "Hide All", [component_info]() {
    QApplication::sendEvent(qApp, new events::ToolPathHideAll(component_info));
  });
  data_->show_all = addAction(icons::getToolPathShowIcon(), "Show All", [component_info]() {
    QApplication::sendEvent(qApp, new events::ToolPathShowAll(component_info));
  });
  addSeparator();
  data_->open_action = addAction(icons::getImportIcon(), "Open", [component_info]() {
    QApplication::sendEvent(qApp, new events::ToolPathOpen(component_info));
  });
  data_->save_action = addAction(icons::getSaveIcon(), "Save", [component_info]() {
    QApplication::sendEvent(qApp, new events::ToolPathSave(component_info));
  });
}

ToolPathToolBar::~ToolPathToolBar() = default;

}  // namespace tesseract_gui
