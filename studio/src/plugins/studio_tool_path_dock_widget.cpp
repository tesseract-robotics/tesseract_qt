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

#include <tesseract_qt/studio/plugins/studio_tool_path_dock_widget.h>

#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_qt/common/widgets/component_info_dialog.h>

#include <tesseract_qt/tool_path/widgets/tool_path_widget.h>
#include <tesseract_qt/tool_path/widgets/tool_path_tool_bar.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include <yaml-cpp/yaml.h>

namespace tesseract_gui
{
struct StudioToolPathDockWidget::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;
  ToolPathWidget* widget{ nullptr };
  ToolPathToolBar* tool_bar{ nullptr };
};

StudioToolPathDockWidget::StudioToolPathDockWidget(const QString& title, QWidget* parent)
  : StudioDockWidget(title, parent), data_(std::make_unique<Implementation>())
{
}

StudioToolPathDockWidget::~StudioToolPathDockWidget() = default;

std::string StudioToolPathDockWidget::getFactoryClassName() const { return "StudioToolPathDockWidgetFactory"; }

void StudioToolPathDockWidget::loadConfig(const YAML::Node& config)
{
  if (const YAML::Node& n = config["component_info"])  // NOLINT
  {
    auto uuid = boost::lexical_cast<boost::uuids::uuid>(n.as<std::string>());
    if (uuid.is_nil())
      throw std::runtime_error("StudioToolPathDockWidget, config component info is nil.");

    data_->component_info = ComponentInfoManager::get(uuid);
    if (data_->component_info == nullptr)
      throw std::runtime_error("StudioToolPathDockWidget, config component info was not found.");
  }
  else
  {
    throw std::runtime_error("StudioToolPathDockWidget, config is missing component info.");
  }

  setup();
}

YAML::Node StudioToolPathDockWidget::getConfig() const
{
  // Config
  YAML::Node config_node;
  config_node["component_info"] = boost::uuids::to_string(data_->component_info->getNamespace());
  return config_node;
}

void StudioToolPathDockWidget::onInitialize()
{
  if (isInitialized())
    return;

  ComponentInfoDialog dialog(this);
  if (dialog.exec())
  {
    data_->component_info = dialog.getComponentInfo();
    if (data_->component_info == nullptr)
      return;

    setup();
  }
}

void StudioToolPathDockWidget::setup()
{
  data_->widget = new ToolPathWidget(data_->component_info);
  data_->tool_bar = new ToolPathToolBar(data_->component_info);

  setWidget(data_->widget);
  setToolBar(data_->tool_bar);
  toolBar()->setIconSize(QSize(25, 25));
  setFeature(ads::CDockWidget::DockWidgetFocusable, true);
}
}  // namespace tesseract_gui
