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

#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/rendering/render_widget.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <DockAreaWidget.h>

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
  if (const YAML::Node& n = config["component_info"])  // NOLINT
  {
    auto uuid = boost::lexical_cast<boost::uuids::uuid>(n.as<std::string>());
    if (uuid.is_nil())
      throw std::runtime_error("StudioRenderPluginConfigWidget, config component info is nil.");
    ui->component_info_widget->setComponentInfo(uuid);
  }
  else
  {
    throw std::runtime_error("StudioRenderPluginConfigWidget, config is missing component info.");
  }

  if (const YAML::Node& n = config["show_sky"])  // NOLINT
    ui->show_sky_check_box->setChecked(n.as<bool>());

  if (const YAML::Node& n = config["show_grid"])  // NOLINT
    ui->show_grid_check_box->setChecked(n.as<bool>());
}

YAML::Node StudioRenderPluginConfigWidget::getConfig() const
{
  auto component_info = ui->component_info_widget->getComponentInfo();
  boost::uuids::uuid uuid{};
  if (component_info != nullptr)
    uuid = component_info->getNamespace();

  // Config
  YAML::Node config_node;
  config_node["component_info"] = boost::uuids::to_string(uuid);
  config_node["show_sky"] = ui->show_sky_check_box->isChecked();
  config_node["show_grid"] = ui->show_grid_check_box->isChecked();

  return config_node;
}

StudioFactoryResult::UPtr StudioRenderPluginConfigWidget::create()
{
  auto results = std::make_unique<StudioFactoryResult>();
  results->central_widget = true;

  auto component_info = ui->component_info_widget->getComponentInfo();
  auto widget = new tesseract_gui::RenderWidget(component_info->getSceneName());
  widget->setSkyEnabled(ui->show_sky_check_box->isChecked());
  widget->setGridEnabled(ui->show_grid_check_box->isChecked());

  results->dock_widget = new ads::CDockWidget(ui->name_line_edit->text());
  results->dock_widget->setWidget(widget);
  results->dock_widget->setFeature(ads::CDockWidget::DockWidgetFocusable, true);

  return results;
}

}  // namespace tesseract_gui
