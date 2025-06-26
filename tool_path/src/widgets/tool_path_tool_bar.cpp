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
#include <QMessageBox>

namespace tesseract_gui
{
struct ToolPathToolBar::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;

  QAction* remove_all{ nullptr };
  QAction* remove_selected{ nullptr };
  QAction* hide_all{ nullptr };
  QAction* show_all{ nullptr };
  QAction* open_action{ nullptr };
  QAction* save_action{ nullptr };
  QAction* component_info_action{ nullptr };
};

ToolPathToolBar::ToolPathToolBar(QWidget* parent) : ToolPathToolBar(nullptr, parent) {}

ToolPathToolBar::ToolPathToolBar(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent)
  : QToolBar(parent), data_(std::make_unique<Implementation>())
{
  data_->component_info = component_info;
  data_->remove_all = addAction(icons::getClearIcon(), "Remove All", [component_info]() {
    events::ToolPathRemoveAll event(component_info);
    QApplication::sendEvent(qApp, &event);
  });
  data_->remove_selected = addAction(icons::getTrashIcon(), "Remove Selected", [component_info]() {
    events::ToolPathRemoveSelected event(component_info);
    QApplication::sendEvent(qApp, &event);
  });
  addSeparator();
  data_->hide_all = addAction(icons::getToolPathHideIcon(), "Hide All", [component_info]() {
    events::ToolPathHideAll event(component_info);
    QApplication::sendEvent(qApp, &event);
  });
  data_->show_all = addAction(icons::getToolPathShowIcon(), "Show All", [component_info]() {
    events::ToolPathShowAll event(component_info);
    QApplication::sendEvent(qApp, &event);
  });
  addSeparator();
  data_->open_action = addAction(icons::getImportIcon(), "Open", [component_info]() {
    events::ToolPathOpen event(component_info);
    QApplication::sendEvent(qApp, &event);
  });
  data_->save_action = addAction(icons::getSaveIcon(), "Save", [component_info]() {
    events::ToolPathSave event(component_info);
    QApplication::sendEvent(qApp, &event);
  });
  addSeparator();
  data_->component_info_action = addAction(QIcon::fromTheme("dialog-information"), "Component Info", [this]() {
    if (data_->component_info != nullptr)
      QMessageBox::information(
          this, "Component Information", QString::fromStdString(data_->component_info->toString()));
    else
      QMessageBox::information(this, "Component Information", "Null");
  });
}

ToolPathToolBar::~ToolPathToolBar() = default;

void ToolPathToolBar::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  data_->component_info = std::move(component_info);
}

std::shared_ptr<const ComponentInfo> ToolPathToolBar::getComponentInfo() const { return data_->component_info; }

}  // namespace tesseract_gui
