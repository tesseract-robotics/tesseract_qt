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

#include <tesseract_qt/studio/plugins/manipulation/studio_manipulation_dock_widget.h>
#include <tesseract_qt/studio/plugins/manipulation/studio_manipulation_config_dialog.h>

#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_qt/common/widgets/component_info_dialog.h>

#include <tesseract_qt/manipulation/manipulation_widget.h>
#include <tesseract_qt/manipulation/manipulation_tool_bar.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace tesseract_gui
{
struct StudioManipulationDockWidget::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;
  bool create_child_environment{ false };
  ManipulationWidget* widget{ nullptr };
  ManipulationToolBar* tool_bar{ nullptr };
};

StudioManipulationDockWidget::StudioManipulationDockWidget(const QString& title, QWidget* parent)
  : StudioDockWidget(title, parent), data_(std::make_unique<Implementation>())
{
}

StudioManipulationDockWidget::~StudioManipulationDockWidget() = default;

std::string StudioManipulationDockWidget::getFactoryClassName() const { return "StudioManipulationDockWidgetFactory"; }

void StudioManipulationDockWidget::loadConfig(const YAML::Node& config)
{
  if (const YAML::Node& n = config["component_info"])  // NOLINT
  {
    auto uuid = boost::lexical_cast<boost::uuids::uuid>(n.as<std::string>());
    if (uuid.is_nil())
      throw std::runtime_error("StudioManipulationDockWidget, config component info is nil.");

    data_->component_info = ComponentInfoManager::get(uuid);
    if (data_->component_info == nullptr)
      throw std::runtime_error("StudioManipulationDockWidget, config component info was not found.");
  }
  else
  {
    throw std::runtime_error("StudioManipulationDockWidget, config is missing component info.");
  }

  if (const YAML::Node& n = config["create_child_environment"])  // NOLINT
    data_->create_child_environment = n.as<bool>();

  setup();
}

YAML::Node StudioManipulationDockWidget::getConfig() const
{
  // Config
  YAML::Node config_node;
  config_node["component_info"] = boost::uuids::to_string(data_->component_info->getNamespace());
  config_node["create_child_environment"] = data_->create_child_environment;
  return config_node;
}

void StudioManipulationDockWidget::onInitialize()
{
  if (isInitialized())
    return;

  StudioManipulationConfigDialog dialog(this);
  if (dialog.exec())
  {
    data_->component_info = dialog.getComponentInfo();
    if (data_->component_info == nullptr)
      return;

    data_->create_child_environment = dialog.createChildEnvironment();

    setup();
  }
}

void StudioManipulationDockWidget::setup()
{
  data_->widget = new ManipulationWidget(data_->component_info, !data_->create_child_environment);
  data_->tool_bar = new ManipulationToolBar(data_->component_info);

  setWidget(data_->widget);
  setToolBar(data_->tool_bar);
  toolBar()->setIconSize(QSize(25, 25));
  setFeature(ads::CDockWidget::DockWidgetFocusable, true);
}
}  // namespace tesseract_gui
