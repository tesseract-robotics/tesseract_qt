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

#include <tesseract_qt/common/events/contact_results_render_manager.h>
#include <tesseract_qt/common/events/contact_results_events.h>
#include <tesseract_qt/common/events/render_events.h>
#include <tesseract_qt/common/utils.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_environment/environment.h>

#include <QApplication>

namespace tesseract_gui
{
ContactResultsRenderManager::ContactResultsRenderManager(std::shared_ptr<const ComponentInfo> component_info)
  : component_info_(std::move(component_info))
{
  qApp->installEventFilter(this);
}

ContactResultsRenderManager::~ContactResultsRenderManager() = default;

bool ContactResultsRenderManager::eventFilter(QObject* obj, QEvent* event)
{
  std::scoped_lock lock(mutex_);
  if (event->type() == events::ContactResultsClear::kType)
  {
    assert(dynamic_cast<events::ContactResultsClear*>(event) != nullptr);
    auto* e = static_cast<events::ContactResultsClear*>(event);
    if (e->getComponentInfo() == component_info_ || e->getComponentInfo()->isParent(component_info_))
      events_.push_back(std::make_unique<events::ContactResultsClear>(*e));
  }
  else if (event->type() == events::ContactResultsSet::kType)
  {
    assert(dynamic_cast<events::ContactResultsSet*>(event) != nullptr);
    auto* e = static_cast<events::ContactResultsSet*>(event);
    if (e->getComponentInfo() == component_info_ || e->getComponentInfo()->isParent(component_info_))
      events_.push_back(std::make_unique<events::ContactResultsSet>(*e));
  }
  else if (event->type() == events::ContactResultsRemove::kType)
  {
    assert(dynamic_cast<events::ContactResultsRemove*>(event) != nullptr);
    auto* e = static_cast<events::ContactResultsRemove*>(event);
    if (e->getComponentInfo() == component_info_ || e->getComponentInfo()->isParent(component_info_))
      events_.push_back(std::make_unique<events::ContactResultsRemove>(*e));
  }
  else if (event->type() == events::ContactResultsVisbility::kType)
  {
    assert(dynamic_cast<events::ContactResultsVisbility*>(event) != nullptr);
    auto* e = static_cast<events::ContactResultsVisbility*>(event);
    if (e->getComponentInfo() == component_info_ || e->getComponentInfo()->isParent(component_info_))
      events_.push_back(std::make_unique<events::ContactResultsVisbility>(*e));
  }
  else if (event->type() == events::ContactResultsVisbilityAll::kType)
  {
    assert(dynamic_cast<events::ContactResultsVisbilityAll*>(event) != nullptr);
    auto* e = static_cast<events::ContactResultsVisbilityAll*>(event);
    if (e->getComponentInfo() == component_info_ || e->getComponentInfo()->isParent(component_info_))
      events_.push_back(std::make_unique<events::ContactResultsVisbilityAll>(*e));
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
