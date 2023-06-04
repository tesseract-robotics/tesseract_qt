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

#include <tesseract_qt/common/events/tool_path_render_manager.h>
#include <tesseract_qt/common/events/render_events.h>
#include <tesseract_qt/common/events/tool_path_events.h>
#include <tesseract_qt/common/component_info.h>

#include <QApplication>

namespace tesseract_gui
{
ToolPathRenderManager::ToolPathRenderManager(std::shared_ptr<const ComponentInfo> component_info)
  : component_info_(std::move(component_info))
{
  qApp->installEventFilter(this);
}

ToolPathRenderManager::~ToolPathRenderManager() = default;

bool ToolPathRenderManager::eventFilter(QObject* obj, QEvent* event)
{
  std::scoped_lock lock(mutex_);
  if (event->type() == events::ToolPathAdd::kType)
  {
    assert(dynamic_cast<events::ToolPathAdd*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathAdd*>(event);
    if (e->getComponentInfo() == component_info_)
      events_.push_back(std::make_unique<events::ToolPathAdd>(*e));
  }
  else if (event->type() == events::ToolPathRemove::kType)
  {
    assert(dynamic_cast<events::ToolPathRemove*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathRemove*>(event);
    if (e->getComponentInfo() == component_info_)
      events_.push_back(std::make_unique<events::ToolPathRemove>(*e));
  }
  else if (event->type() == events::ToolPathRemoveAll::kType)
  {
    assert(dynamic_cast<events::ToolPathRemoveAll*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathRemoveAll*>(event);
    if (e->getComponentInfo() == component_info_)
      events_.push_back(std::make_unique<events::ToolPathRemoveAll>(*e));
  }
  else if (event->type() == events::ToolPathHideAll::kType)
  {
    assert(dynamic_cast<events::ToolPathHideAll*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathHideAll*>(event);
    if (e->getComponentInfo() == component_info_)
      events_.push_back(std::make_unique<events::ToolPathHideAll>(*e));
  }
  else if (event->type() == events::ToolPathShowAll::kType)
  {
    assert(dynamic_cast<events::ToolPathShowAll*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathShowAll*>(event);
    if (e->getComponentInfo() == component_info_)
      events_.push_back(std::make_unique<events::ToolPathShowAll>(*e));
  }
  else if (event->type() == events::ToolPathHide::kType)
  {
    assert(dynamic_cast<events::ToolPathHide*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathHide*>(event);
    if (e->getComponentInfo() == component_info_)
      events_.push_back(std::make_unique<events::ToolPathHide>(*e));
  }
  else if (event->type() == events::ToolPathShow::kType)
  {
    assert(dynamic_cast<events::ToolPathShow*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathShow*>(event);
    if (e->getComponentInfo() == component_info_)
      events_.push_back(std::make_unique<events::ToolPathShow>(*e));
  }
  else if (event->type() == events::PreRender::kType)
  {
    assert(dynamic_cast<events::PreRender*>(event) != nullptr);
    if (static_cast<events::PreRender*>(event)->getSceneName() == component_info_->getSceneName())
      render();
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

}  // namespace tesseract_gui
