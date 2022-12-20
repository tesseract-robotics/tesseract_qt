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

#include <tesseract_qt/scene_graph/scene_graph_tool_bar.h>
#include <tesseract_qt/scene_graph/scene_graph_events.h>
#include <tesseract_qt/scene_graph/scene_graph_link_visibility.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>
#include <QApplication>

namespace tesseract_gui
{
struct SceneGraphToolBar::Implementation
{
  ComponentInfo component_info;

  QAction* show_all_links_action;
  QAction* hide_all_links_action;

  QAction* show_visual_all_links_action;
  QAction* hide_visual_all_links_action;

  QAction* show_collision_all_links_action;
  QAction* hide_collision_all_links_action;

  QAction* select_all_links_action;
  QAction* deselect_all_links_action;

  QAction* show_axis_all_links_action;
  QAction* hide_axis_all_links_action;

  QAction* plot_scene_graph_action;
};

SceneGraphToolBar::SceneGraphToolBar(QWidget* parent) : QToolBar(parent), data_(std::make_unique<Implementation>())
{
  ctor(data_->component_info);
}

SceneGraphToolBar::SceneGraphToolBar(ComponentInfo component_info, QWidget* parent)
  : QToolBar(parent), data_(std::make_unique<Implementation>())
{
  ctor(component_info);
}

SceneGraphToolBar::~SceneGraphToolBar() = default;

void SceneGraphToolBar::ctor(ComponentInfo component_info)
{
  data_->component_info = component_info;
  //  data_->remove_all = addAction(icons::getClearIcon(), "Remove All", [scene_name]() {
  //    QApplication::sendEvent(qApp, new events::ToolPathRemoveAll(scene_name));
  //  });
  //  data_->remove_selected = addAction(icons::getTrashIcon(), "Remove Selected", [scene_name]() {
  //    QApplication::sendEvent(qApp, new events::ToolPathRemoveSelected(scene_name));
  //  });
  //  addSeparator();
  //  data_->hide_all = addAction(icons::getToolPathHideIcon(), "Hide All", [scene_name]() {
  //    QApplication::sendEvent(qApp, new events::ToolPathHideAll(scene_name));
  //  });
  //  data_->show_all = addAction(icons::getToolPathShowIcon(), "Show All", [scene_name]() {
  //    QApplication::sendEvent(qApp, new events::ToolPathShowAll(scene_name));
  //  });
  data_->show_all_links_action = addAction(icons::getShowAllLinksIcon(), "Show All Links", [component_info]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(component_info, LinkVisibilityFlags::LINK, true));
  });
  data_->hide_all_links_action = addAction(icons::getHideAllLinksIcon(), "Hide All Links", [component_info]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(component_info, LinkVisibilityFlags::LINK, false));
  });
  addSeparator();
  data_->show_visual_all_links_action =
      addAction(icons::getShowVisualAllLinksIcon(), "Show Visual All Links", [component_info]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(component_info, LinkVisibilityFlags::VISUAL, true));
      });
  data_->hide_visual_all_links_action =
      addAction(icons::getHideVisualAllLinksIcon(), "Hide Visual All Links", [component_info]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(component_info, LinkVisibilityFlags::VISUAL, false));
      });
  addSeparator();
  data_->show_collision_all_links_action =
      addAction(icons::getShowCollisionAllLinksIcon(), "Show Collision All Links", [component_info]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(component_info, LinkVisibilityFlags::COLLISION, true));
      });
  data_->hide_collision_all_links_action =
      addAction(icons::getHideCollisionAllLinksIcon(), "Hide Collision All Links", [component_info]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(component_info, LinkVisibilityFlags::COLLISION, false));
      });
  addSeparator();
  data_->select_all_links_action = addAction(icons::getSelectAllLinksIcon(), "Select All Links", [component_info]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(component_info, LinkVisibilityFlags::WIREBOX, true));
  });
  data_->deselect_all_links_action =
      addAction(icons::getDeselectAllLinksIcon(), "Deselect All Links", [component_info]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(component_info, LinkVisibilityFlags::WIREBOX, false));
      });
  addSeparator();
  data_->show_axis_all_links_action =
      addAction(icons::getShowAxisAllLinksIcon(), "Show Axis All Links", [component_info]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(component_info, LinkVisibilityFlags::AXIS, true));
      });
  data_->hide_axis_all_links_action =
      addAction(icons::getHideAxisAllLinksIcon(), "Hide Axis All Links", [component_info]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(component_info, LinkVisibilityFlags::AXIS, false));
      });
  addSeparator();
  data_->plot_scene_graph_action = addAction(icons::getPlotIcon(), "Plot Scene Graph", [component_info]() {
    QApplication::sendEvent(qApp, new events::SceneGraphPlot(component_info));
  });
}
}  // namespace tesseract_gui
