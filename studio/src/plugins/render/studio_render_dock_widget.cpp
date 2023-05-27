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
#include <tesseract_qt/rendering/render_widget.h>

#include <tesseract_qt/common/component_info.h>

namespace tesseract_gui
{
StudioRenderDockWidget::~StudioRenderDockWidget() = default;

std::string StudioRenderDockWidget::getFactoryClassName() const { return "StudioRenderDockWidgetFactory"; }

void StudioRenderDockWidget::loadConfig(const YAML::Node& config)
{
  //  if (const YAML::Node& n = config["component_info"])  // NOLINT
  //  {
  //    auto uuid = boost::lexical_cast<boost::uuids::uuid>(n.as<std::string>());
  //    if (uuid.is_nil())
  //      throw std::runtime_error("StudioRenderPluginConfigWidget, config component info is nil.");
  //    ui->component_info_widget->setComponentInfo(uuid);
  //  }
  //  else
  //  {
  //    throw std::runtime_error("StudioRenderPluginConfigWidget, config is missing component info.");
  //  }

  //  if (const YAML::Node& n = config["show_sky"])  // NOLINT
  //    ui->show_sky_check_box->setChecked(n.as<bool>());

  //  if (const YAML::Node& n = config["show_grid"])  // NOLINT
  //    ui->show_grid_check_box->setChecked(n.as<bool>());
}

YAML::Node StudioRenderDockWidget::getConfig() const
{
  //  auto component_info = ui->component_info_widget->getComponentInfo();
  //  boost::uuids::uuid uuid{};
  //  if (component_info != nullptr)
  //    uuid = component_info->getNamespace();

  //  // Config
  //  YAML::Node config_node;
  //  config_node["component_info"] = boost::uuids::to_string(uuid);
  //  config_node["show_sky"] = ui->show_sky_check_box->isChecked();
  //  config_node["show_grid"] = ui->show_grid_check_box->isChecked();

  //  return config_node;
}

void StudioRenderDockWidget::onInitialize()
{
  if (isInitialized())
    return;

  StudioRenderConfigDialog dialog(this);
  if (dialog.exec())
  {
    assign_as_central_widget_ = dialog.assignAsCentralWidget();

    auto component_info = dialog.getComponentInfo();
    if (component_info == nullptr)
      return;

    auto render_widget = new tesseract_gui::RenderWidget(component_info->getSceneName());
    render_widget->setSkyEnabled(dialog.skyEnabled());
    render_widget->setGridEnabled(dialog.gridEnabled());
    render_widget->setShadowsEnabled(dialog.shadowsEnabled());
    setWidget(render_widget);
    setFeature(ads::CDockWidget::DockWidgetFocusable, true);
  }
}
}  // namespace tesseract_gui
