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

#include <tesseract_qt/scene_graph/widgets/scene_graph_tool_bar.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/link_visibility.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>
#include <QApplication>

namespace tesseract_gui
{
struct SceneGraphToolBar::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;

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

SceneGraphToolBar::SceneGraphToolBar(QWidget* parent) : SceneGraphToolBar(nullptr, parent) {}

SceneGraphToolBar::SceneGraphToolBar(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent)
  : QToolBar(parent), data_(std::make_unique<Implementation>())
{
  data_->component_info = std::move(component_info);
  data_->show_all_links_action = addAction(icons::getShowAllLinksIcon(), "Show All Links", [this]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(data_->component_info, LinkVisibilityFlags::LINK, true));
  });
  data_->hide_all_links_action = addAction(icons::getHideAllLinksIcon(), "Hide All Links", [this]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(data_->component_info, LinkVisibilityFlags::LINK, false));
  });
  addSeparator();
  data_->show_visual_all_links_action =
      addAction(icons::getShowVisualAllLinksIcon(), "Show Visual All Links", [this]() {
        QApplication::sendEvent(
            qApp,
            new events::SceneGraphModifyLinkVisibilityALL(data_->component_info, LinkVisibilityFlags::VISUAL, true));
      });
  data_->hide_visual_all_links_action =
      addAction(icons::getHideVisualAllLinksIcon(), "Hide Visual All Links", [this]() {
        QApplication::sendEvent(
            qApp,
            new events::SceneGraphModifyLinkVisibilityALL(data_->component_info, LinkVisibilityFlags::VISUAL, false));
      });
  addSeparator();
  data_->show_collision_all_links_action =
      addAction(icons::getShowCollisionAllLinksIcon(), "Show Collision All Links", [this]() {
        QApplication::sendEvent(
            qApp,
            new events::SceneGraphModifyLinkVisibilityALL(data_->component_info, LinkVisibilityFlags::COLLISION, true));
      });
  data_->hide_collision_all_links_action =
      addAction(icons::getHideCollisionAllLinksIcon(), "Hide Collision All Links", [this]() {
        QApplication::sendEvent(qApp,
                                new events::SceneGraphModifyLinkVisibilityALL(
                                    data_->component_info, LinkVisibilityFlags::COLLISION, false));
      });
  addSeparator();
  data_->select_all_links_action = addAction(icons::getSelectAllLinksIcon(), "Select All Links", [this]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(data_->component_info, LinkVisibilityFlags::WIREBOX, true));
  });
  data_->deselect_all_links_action = addAction(icons::getDeselectAllLinksIcon(), "Deselect All Links", [this]() {
    QApplication::sendEvent(
        qApp,
        new events::SceneGraphModifyLinkVisibilityALL(data_->component_info, LinkVisibilityFlags::WIREBOX, false));
  });
  addSeparator();
  data_->show_axis_all_links_action = addAction(icons::getShowAxisAllLinksIcon(), "Show Axis All Links", [this]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(data_->component_info, LinkVisibilityFlags::AXIS, true));
  });
  data_->hide_axis_all_links_action = addAction(icons::getHideAxisAllLinksIcon(), "Hide Axis All Links", [this]() {
    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(data_->component_info, LinkVisibilityFlags::AXIS, false));
  });
  addSeparator();
  data_->plot_scene_graph_action = addAction(icons::getPlotIcon(), "Plot Scene Graph", [this]() {
    QApplication::sendEvent(qApp, new events::SceneGraphPlot(data_->component_info));
  });
}

SceneGraphToolBar::~SceneGraphToolBar() = default;

void SceneGraphToolBar::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  data_->component_info = component_info;
}
std::shared_ptr<const ComponentInfo> SceneGraphToolBar::getComponentInfo() const { return data_->component_info; }

}  // namespace tesseract_gui
