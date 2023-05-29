/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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

#include <tesseract_qt/studio/plugins/render/studio_render_dock_widget.h>
#include <tesseract_qt/studio/plugins/render/studio_render_config_dialog.h>
#include <tesseract_qt/studio/studio_plugin_utils.h>

#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_qt/common/widgets/load_environment_dialog.h>

#include <tesseract_qt/rendering/render_widget.h>
#include <tesseract_qt/rendering/ign_scene_graph_render_manager.h>
#include <tesseract_qt/rendering/ign_tool_path_render_manager.h>
#include <tesseract_qt/rendering/ign_contact_results_render_manager.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include <QMenu>
#include <QAction>

namespace tesseract_gui
{
struct StudioRenderDockWidget::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;
  EntityManager::Ptr entity_manager;
  RenderWidget* render_widget{ nullptr };
  std::unique_ptr<IgnSceneGraphRenderManager> scene_graph_manager;
  std::unique_ptr<IgnToolPathRenderManager> tool_path_manager;
  std::unique_ptr<IgnContactResultsRenderManager> contact_results_manager;

  QMenu* file_menu{ nullptr };
  QAction* open_action{ nullptr };

  std::unique_ptr<LoadEnvironmentDialog> open_dialog;

  void addMenu();
};

void StudioRenderDockWidget::Implementation::addMenu()
{
  open_dialog = std::make_unique<LoadEnvironmentDialog>(component_info);

  // Add to open action to menu if it does not exist
  file_menu = getStudioMenu("File");
  QAction* action = getStudioMenuAction(file_menu, "Load URDF/SRDF");
  if (action == nullptr)
  {
    file_menu->addSeparator();
    open_action = file_menu->addAction(icons::getOpenIcon(), "Load URDF/SRDF", [this]() { open_dialog->show(); });
  }
}

StudioRenderDockWidget::StudioRenderDockWidget(const QString& title, QWidget* parent)
  : StudioDockWidget(title, parent), data_(std::make_unique<Implementation>())
{
  data_->entity_manager = std::make_shared<EntityManager>();
}

StudioRenderDockWidget::~StudioRenderDockWidget() = default;

std::string StudioRenderDockWidget::getFactoryClassName() const { return "StudioRenderDockWidgetFactory"; }

void StudioRenderDockWidget::loadConfig(const YAML::Node& config)
{
  if (const YAML::Node& n = config["component_info"])  // NOLINT
  {
    auto uuid = boost::lexical_cast<boost::uuids::uuid>(n.as<std::string>());
    if (uuid.is_nil())
      throw std::runtime_error("StudioRenderDockWidget, config component info is nil.");

    data_->component_info = ComponentInfoManager::get(uuid);
    if (data_->component_info == nullptr)
      throw std::runtime_error("StudioRenderDockWidget, config component info was not found.");
  }
  else
  {
    throw std::runtime_error("StudioRenderDockWidget, config is missing component info.");
  }

  std::string engine_name;
  if (const YAML::Node& n = config["engine_name"])  // NOLINT
    engine_name = n.as<std::string>();
  else
    throw std::runtime_error("StudioRenderDockWidget, config is missing 'engine_name'.");

  if (const YAML::Node& n = config["central_widget"])  // NOLINT
    assign_as_central_widget_ = n.as<bool>();

  auto render_widget = new tesseract_gui::RenderWidget(data_->component_info->getSceneName(), engine_name);

  if (const YAML::Node& n = config["show_sky"])  // NOLINT
    render_widget->setSkyEnabled(n.as<bool>());

  if (const YAML::Node& n = config["show_grid"])  // NOLINT
    render_widget->setGridEnabled(n.as<bool>());

  if (const YAML::Node& n = config["show_shadows"])  // NOLINT
    render_widget->setShadowsEnabled(n.as<bool>());

  data_->scene_graph_manager =
      std::make_unique<IgnSceneGraphRenderManager>(data_->component_info, data_->entity_manager);
  data_->tool_path_manager = std::make_unique<IgnToolPathRenderManager>(data_->component_info, data_->entity_manager);
  data_->contact_results_manager = std::make_unique<IgnContactResultsRenderManager>(data_->component_info);

  setWidget(render_widget);
  setFeature(ads::CDockWidget::DockWidgetFocusable, true);

  // Add menu
  data_->addMenu();
}

YAML::Node StudioRenderDockWidget::getConfig() const
{
  auto* render_widget = qobject_cast<tesseract_gui::RenderWidget*>(widget());

  // Config
  YAML::Node config_node;
  config_node["component_info"] = boost::uuids::to_string(data_->component_info->getNamespace());
  config_node["engine_name"] = render_widget->getEngineName();
  config_node["show_sky"] = render_widget->skyEnabled();
  config_node["show_grid"] = render_widget->gridEnabled();
  config_node["show_shadows"] = render_widget->shadowsEnabled();
  config_node["central_widget"] = assign_as_central_widget_;

  return config_node;
}

void StudioRenderDockWidget::onInitialize()
{
  if (isInitialized())
    return;

  StudioRenderConfigDialog dialog(this);
  if (dialog.exec())
  {
    assign_as_central_widget_ = dialog.assignAsCentralWidget();

    data_->component_info = dialog.getComponentInfo();
    if (data_->component_info == nullptr)
      return;

    data_->render_widget = new tesseract_gui::RenderWidget(data_->component_info->getSceneName());
    data_->render_widget->setSkyEnabled(dialog.skyEnabled());
    data_->render_widget->setGridEnabled(dialog.gridEnabled());
    data_->render_widget->setShadowsEnabled(dialog.shadowsEnabled());

    data_->scene_graph_manager =
        std::make_unique<IgnSceneGraphRenderManager>(data_->component_info, data_->entity_manager);
    data_->tool_path_manager = std::make_unique<IgnToolPathRenderManager>(data_->component_info, data_->entity_manager);
    data_->contact_results_manager = std::make_unique<IgnContactResultsRenderManager>(data_->component_info);

    setWidget(data_->render_widget);
    setFeature(ads::CDockWidget::DockWidgetFocusable, true);

    // Add menu
    data_->addMenu();
  }
}
}  // namespace tesseract_gui
