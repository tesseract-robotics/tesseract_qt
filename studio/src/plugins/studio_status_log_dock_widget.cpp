/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2025 Levi Armstrong <levi.armstrong@gmail.com>
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

#include <tesseract_qt/studio/plugins/studio_status_log_dock_widget.h>

#include <tesseract_qt/common/widgets/status_log_widget.h>
#include <tesseract_qt/common/widgets/status_log_tool_bar.h>

#include <yaml-cpp/yaml.h>

#include <QMenu>
#include <QAction>

namespace tesseract_gui
{
struct StudioStatusLogDockWidget::Implementation
{
  StatusLogWidget* widget{ nullptr };
  StatusLogToolBar* tool_bar{ nullptr };
};

StudioStatusLogDockWidget::StudioStatusLogDockWidget(const QString& title, QWidget* parent)
  : StudioDockWidget(title, parent), data_(std::make_unique<Implementation>())
{
}

StudioStatusLogDockWidget::~StudioStatusLogDockWidget() = default;

std::string StudioStatusLogDockWidget::getFactoryClassName() const { return "StudioStatusLogDockWidgetFactory"; }

void StudioStatusLogDockWidget::loadConfig(const YAML::Node& /*config*/) { setup(); }

YAML::Node StudioStatusLogDockWidget::getConfig() const { return {}; }

void StudioStatusLogDockWidget::onInitialize()
{
  if (isInitialized())
    return;

  setup();
}

void StudioStatusLogDockWidget::setup()
{
  data_->widget = new StatusLogWidget();
  data_->tool_bar = new StatusLogToolBar();
  setWidget(data_->widget);
  setToolBar(data_->tool_bar);
  toolBar()->setIconSize(QSize(25, 25));
  setFeature(ads::CDockWidget::DockWidgetFocusable, true);
}
}  // namespace tesseract_gui
