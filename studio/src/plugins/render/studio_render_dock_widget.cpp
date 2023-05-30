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
};

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

  data_->render_widget = new tesseract_gui::RenderWidget(data_->component_info->getSceneName(), engine_name);

  if (const YAML::Node& n = config["show_sky"])  // NOLINT
    data_->render_widget->setSkyEnabled(n.as<bool>());

  if (const YAML::Node& n = config["show_grid"])  // NOLINT
    data_->render_widget->setGridEnabled(n.as<bool>());

  if (const YAML::Node& n = config["show_shadows"])  // NOLINT
    data_->render_widget->setShadowsEnabled(n.as<bool>());

  data_->scene_graph_manager =
      std::make_unique<IgnSceneGraphRenderManager>(data_->component_info, data_->entity_manager);
  data_->tool_path_manager = std::make_unique<IgnToolPathRenderManager>(data_->component_info, data_->entity_manager);
  data_->contact_results_manager = std::make_unique<IgnContactResultsRenderManager>(data_->component_info);

  setWidget(data_->render_widget);
  setFeature(ads::CDockWidget::DockWidgetFocusable, true);
}

YAML::Node StudioRenderDockWidget::getConfig() const
{
  // Config
  YAML::Node config_node;
  config_node["component_info"] = boost::uuids::to_string(data_->component_info->getNamespace());
  config_node["engine_name"] = data_->render_widget->getEngineName();
  config_node["show_sky"] = data_->render_widget->skyEnabled();
  config_node["show_grid"] = data_->render_widget->gridEnabled();
  config_node["show_shadows"] = data_->render_widget->shadowsEnabled();
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
  }
}
}  // namespace tesseract_gui
