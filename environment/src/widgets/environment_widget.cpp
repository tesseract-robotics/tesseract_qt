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
#include <tesseract_qt/environment/widgets/environment_widget.h>
#include "ui_environment_widget.h"

#include <tesseract_qt/common/widgets/image_viewer_widget.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/utils.h>

namespace tesseract_gui
{
struct EnvironmentWidget::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;
};

EnvironmentWidget::EnvironmentWidget(QWidget* parent) : EnvironmentWidget(nullptr, parent) {}
EnvironmentWidget::EnvironmentWidget(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::EnvironmentWidget>()), data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);
  ui->tab_widget->setCurrentIndex(0);

  setComponentInfo(std::move(component_info));
}

EnvironmentWidget::~EnvironmentWidget() = default;

void EnvironmentWidget::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  data_->component_info = std::move(component_info);

  ui->scene_widget->setComponentInfo(data_->component_info);
  ui->scene_state_widget->setComponentInfo(data_->component_info);
  ui->acm_widget->setComponentInfo(data_->component_info);
  ui->groups_widget->setComponentInfo(data_->component_info);
  ui->group_states_widget->setComponentInfo(data_->component_info);
  ui->group_tcps_widget->setComponentInfo(data_->component_info);
  ui->cmd_history_widget->setComponentInfo(data_->component_info);
  ui->contacts_widget->setComponentInfo(data_->component_info);
}

std::shared_ptr<const ComponentInfo> EnvironmentWidget::getComponentInfo() const { return data_->component_info; }

void EnvironmentWidget::onPlotSceneGraph()
{
  //  tesseract_common::fs::path dot_path("/tmp/environment_widget_scene_graph.dot");
  //  tesseract_common::fs::path image_path("/tmp/environment_widget_scene_graph.png");
  //  data_->config->getEnvironment()->getSceneGraph()->saveDOT(dot_path.c_str());
  //  saveDotImage(dot_path, image_path, "png");

  //  auto* image_viewer = new ImageViewerWidget();
  //  image_viewer->loadImage(image_path.c_str());
  //  image_viewer->show();
}

}  // namespace tesseract_gui
