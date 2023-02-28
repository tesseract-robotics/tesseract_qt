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

#include <tesseract_qt/manipulation/manipulation_tool_bar.h>
#include <tesseract_qt/common/events/manipulation_events.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/link_visibility.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>

#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QStringListModel>

namespace tesseract_gui
{
struct ManipulationToolBar::Implementation
{
  ComponentInfo parent_component_info;
  std::string state_name;
  std::unordered_map<std::string, tesseract_gui::ComponentInfo> state_component_infos;

  QStringListModel state_names_model;
  QComboBox* state_names_combo_box;

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
};

ManipulationToolBar::ManipulationToolBar(QWidget* parent) : ManipulationToolBar(ComponentInfo(), parent) {}

ManipulationToolBar::ManipulationToolBar(ComponentInfo parent_component_info, QWidget* parent)
  : QToolBar(parent), data_(std::make_unique<Implementation>())
{
  data_->parent_component_info = parent_component_info;

  addWidget(new QLabel("State:"));

  data_->state_names_combo_box = new QComboBox();
  data_->state_names_combo_box->setEditable(false);
  data_->state_names_combo_box->setModel(&data_->state_names_model);
  connect(data_->state_names_combo_box, SIGNAL(currentTextChanged(QString)), this, SLOT(onStateNameChanged(QString)));
  addWidget(data_->state_names_combo_box);

  data_->show_all_links_action = addAction(icons::getShowAllLinksIcon(), "Show All Links", [this]() {
    auto it = data_->state_component_infos.find(data_->state_name);
    if (it == data_->state_component_infos.end())
      return;

    QApplication::sendEvent(qApp,
                            new events::SceneGraphModifyLinkVisibilityALL(it->second, LinkVisibilityFlags::LINK, true));
  });
  data_->hide_all_links_action = addAction(icons::getHideAllLinksIcon(), "Hide All Links", [this]() {
    auto it = data_->state_component_infos.find(data_->state_name);
    if (it == data_->state_component_infos.end())
      return;

    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(it->second, LinkVisibilityFlags::LINK, false));
  });
  addSeparator();
  data_->show_visual_all_links_action =
      addAction(icons::getShowVisualAllLinksIcon(), "Show Visual All Links", [this]() {
        auto it = data_->state_component_infos.find(data_->state_name);
        if (it == data_->state_component_infos.end())
          return;

        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(it->second, LinkVisibilityFlags::VISUAL, true));
      });
  data_->hide_visual_all_links_action =
      addAction(icons::getHideVisualAllLinksIcon(), "Hide Visual All Links", [this]() {
        auto it = data_->state_component_infos.find(data_->state_name);
        if (it == data_->state_component_infos.end())
          return;

        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(it->second, LinkVisibilityFlags::VISUAL, false));
      });
  addSeparator();
  data_->show_collision_all_links_action =
      addAction(icons::getShowCollisionAllLinksIcon(), "Show Collision All Links", [this]() {
        auto it = data_->state_component_infos.find(data_->state_name);
        if (it == data_->state_component_infos.end())
          return;

        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(it->second, LinkVisibilityFlags::COLLISION, true));
      });
  data_->hide_collision_all_links_action =
      addAction(icons::getHideCollisionAllLinksIcon(), "Hide Collision All Links", [this]() {
        auto it = data_->state_component_infos.find(data_->state_name);
        if (it == data_->state_component_infos.end())
          return;

        QApplication::sendEvent(
            qApp, new events::SceneGraphModifyLinkVisibilityALL(it->second, LinkVisibilityFlags::COLLISION, false));
      });
  addSeparator();
  data_->select_all_links_action = addAction(icons::getSelectAllLinksIcon(), "Select All Links", [this]() {
    auto it = data_->state_component_infos.find(data_->state_name);
    if (it == data_->state_component_infos.end())
      return;

    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(it->second, LinkVisibilityFlags::WIREBOX, true));
  });
  data_->deselect_all_links_action = addAction(icons::getDeselectAllLinksIcon(), "Deselect All Links", [this]() {
    auto it = data_->state_component_infos.find(data_->state_name);
    if (it == data_->state_component_infos.end())
      return;

    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(it->second, LinkVisibilityFlags::WIREBOX, false));
  });
  addSeparator();
  data_->show_axis_all_links_action = addAction(icons::getShowAxisAllLinksIcon(), "Show Axis All Links", [this]() {
    auto it = data_->state_component_infos.find(data_->state_name);
    if (it == data_->state_component_infos.end())
      return;

    QApplication::sendEvent(qApp,
                            new events::SceneGraphModifyLinkVisibilityALL(it->second, LinkVisibilityFlags::AXIS, true));
  });
  data_->hide_axis_all_links_action = addAction(icons::getHideAxisAllLinksIcon(), "Hide Axis All Links", [this]() {
    auto it = data_->state_component_infos.find(data_->state_name);
    if (it == data_->state_component_infos.end())
      return;

    QApplication::sendEvent(
        qApp, new events::SceneGraphModifyLinkVisibilityALL(it->second, LinkVisibilityFlags::AXIS, false));
  });

  // Install event filter
  qGuiApp->installEventFilter(this);
}

ManipulationToolBar::~ManipulationToolBar() = default;

void ManipulationToolBar::setComponentInfo(ComponentInfo component_info)
{
  data_->parent_component_info = component_info;
  data_->state_name.clear();
  data_->state_names_model.setStringList(QStringList());
  data_->state_names_combo_box->clear();
}
const ComponentInfo& ManipulationToolBar::getComponentInfo() const { return data_->parent_component_info; }

void ManipulationToolBar::onStateNameChanged(const QString& text) { data_->state_name = text.toStdString(); }

bool ManipulationToolBar::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::ManipulationChanged::kType)
  {
    assert(dynamic_cast<events::ManipulationChanged*>(event) != nullptr);
    auto* e = static_cast<events::ManipulationChanged*>(event);
    if (e->getComponentInfo() == data_->parent_component_info)
    {
      std::string state_name = e->getStateName();
      data_->state_name = state_name;
      data_->state_component_infos = e->getStateComponentInfos();
      QStringList state_names;
      for (const auto& it : data_->state_component_infos)
        state_names.push_back(it.first.c_str());

      data_->state_names_model.setStringList(state_names);
      data_->state_names_combo_box->setCurrentText(QString::fromStdString(state_name));
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
