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
#include <tesseract_qt/studio/plugins/render/studio_render_plugin_config_widget.h>
#include "ui_studio_render_plugin_config_widget.h"

#include <QStringListModel>

namespace tesseract_gui
{
StudioRenderPluginConfigWidget::StudioRenderPluginConfigWidget(QWidget* parent)
  : StudioPluginConfigWidget(parent), ui(std::make_unique<Ui::StudioRenderPluginConfigWidget>())
{
  ui->setupUi(this);
}

StudioRenderPluginConfigWidget::~StudioRenderPluginConfigWidget() = default;

std::string StudioRenderPluginConfigWidget::getClassName() const { return "StudioRenderPluginWidgetFactory"; }

void StudioRenderPluginConfigWidget::setName(const std::string& name) { ui->name_line_edit->setText(name.c_str()); }

std::string StudioRenderPluginConfigWidget::getName() const { return ui->name_line_edit->text().toStdString(); }

void StudioRenderPluginConfigWidget::setConfig(const YAML::Node& config)
{
  if (config["component_info"])  // NOLINT
  {
    const YAML::Node& component_info = config["component_info"];
  }

  if (config["show_sky"])  // NOLINT
    ui->show_sky_check_box->setChecked(config["show_sky"].as<bool>());

  if (config["show_grid"])  // NOLINT
    ui->show_grid_check_box->setChecked(config["show_grid"].as<bool>());
}

YAML::Node StudioRenderPluginConfigWidget::getConfig() const
{
  // Component Info
  YAML::Node component_info;
  component_info["scene_name"] = "scene_name_test";
  component_info["ns"] = "ns_test";
  component_info["description"] = "description_test";

  // Config
  YAML::Node config_node;
  config_node["component_info"] = component_info;
  config_node["show_sky"] = ui->show_sky_check_box->isChecked();
  config_node["show_grid"] = ui->show_grid_check_box->isChecked();

  return config_node;
}

}  // namespace tesseract_gui
