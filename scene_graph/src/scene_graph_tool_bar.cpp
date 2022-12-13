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
#include <QApplication>

namespace tesseract_gui
{
struct SceneGraphToolBar::Implementation
{
  std::string scene_name;

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

SceneGraphToolBar::SceneGraphToolBar(const std::string& scene_name, QWidget* parent)
  : QToolBar(parent), data_(std::make_unique<Implementation>())
{
  data_->scene_name = scene_name;
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
  data_->show_all_links_action = addAction(icons::getShowAllLinksIcon(), "Show All Links", [scene_name]() {
    QApplication::sendEvent(qApp,
                            new events::SceneGraphModifyLinkVisibilityALL(scene_name, LinkVisibilityFlags::LINK, true));
  });
  data_->hide_all_links_action = addAction(icons::getHideAllLinksIcon(), "Hide All Links", [scene_name]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(scene_name, LinkVisibilityFlags::LINK, false));
  });
  addSeparator();
  data_->show_visual_all_links_action =
      addAction(icons::getShowVisualAllLinksIcon(), "Show Visual All Links", [scene_name]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(scene_name, LinkVisibilityFlags::VISUAL, true));
      });
  data_->hide_visual_all_links_action =
      addAction(icons::getHideVisualAllLinksIcon(), "Hide Visual All Links", [scene_name]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(scene_name, LinkVisibilityFlags::VISUAL, false));
      });
  addSeparator();
  data_->show_collision_all_links_action =
      addAction(icons::getShowCollisionAllLinksIcon(), "Show Collision All Links", [scene_name]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(scene_name, LinkVisibilityFlags::COLLISION, true));
      });
  data_->hide_collision_all_links_action =
      addAction(icons::getHideCollisionAllLinksIcon(), "Hide Collision All Links", [scene_name]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(scene_name, LinkVisibilityFlags::COLLISION, false));
      });
  addSeparator();
  data_->select_all_links_action = addAction(icons::getSelectAllLinksIcon(), "Select All Links", [scene_name]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(scene_name, LinkVisibilityFlags::WIREBOX, true));
  });
  data_->deselect_all_links_action = addAction(icons::getDeselectAllLinksIcon(), "Deselect All Links", [scene_name]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(scene_name, LinkVisibilityFlags::WIREBOX, false));
  });
  addSeparator();
  data_->show_axis_all_links_action =
      addAction(icons::getShowAxisAllLinksIcon(), "Show Axis All Links", [scene_name]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(scene_name, LinkVisibilityFlags::AXIS, true));
      });
  data_->hide_axis_all_links_action =
      addAction(icons::getHideAxisAllLinksIcon(), "Hide Axis All Links", [scene_name]() {
        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(scene_name, LinkVisibilityFlags::AXIS, false));
      });
  addSeparator();
  data_->plot_scene_graph_action = addAction(icons::getPlotIcon(), "Plot Scene Graph", [scene_name]() {
    QApplication::sendEvent(qApp, new events::SceneGraphPlot(scene_name));
  });
}

SceneGraphToolBar::~SceneGraphToolBar() = default;
}  // namespace tesseract_gui
