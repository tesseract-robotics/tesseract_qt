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

#include <tesseract_qt/common/events/scene_graph_render_manager.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/events/allowed_collision_matrix_events.h>
#include <tesseract_qt/common/events/render_events.h>
#include <tesseract_qt/common/utils.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_environment/environment.h>

#include <QApplication>

namespace tesseract_gui
{
SceneGraphRenderManager::SceneGraphRenderManager(ComponentInfo component_info)
  : component_info_(std::make_unique<ComponentInfo>(std::move(component_info)))
{
  // Check if environment were created
  auto env_wrappers = EnvironmentManager::getAll();
  for (const auto& env_wrapper : env_wrappers)
  {
    if (env_wrapper.first == *component_info_ || env_wrapper.first.isParent(*component_info_))
    {
      auto env = env_wrapper.second->getEnvironment();
      if (env != nullptr && env->isInitialized())
        events_.push_back(std::make_unique<events::SceneGraphSet>(env_wrapper.first, env->getSceneGraph()->clone()));
    }
  }

  qApp->installEventFilter(this);
}

SceneGraphRenderManager::~SceneGraphRenderManager() = default;

bool SceneGraphRenderManager::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::SceneGraphClear::kType)
  {
    assert(dynamic_cast<events::SceneGraphClear*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphClear*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphClear>(*e));
  }
  else if (event->type() == events::SceneGraphSet::kType)
  {
    assert(dynamic_cast<events::SceneGraphSet*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphSet*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphSet>(*e));
  }
  else if (event->type() == events::SceneGraphAddLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphAddLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphAddLink*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphAddLink>(*e));
  }
  else if (event->type() == events::SceneGraphAddJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphAddJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphAddJoint*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphAddJoint>(*e));
  }
  else if (event->type() == events::SceneGraphMoveLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphMoveLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphMoveLink*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphMoveLink>(*e));
  }
  else if (event->type() == events::SceneGraphMoveJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphMoveJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphMoveJoint*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphMoveJoint>(*e));
  }
  else if (event->type() == events::SceneGraphRemoveLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphRemoveLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphRemoveLink*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphRemoveLink>(*e));
  }
  else if (event->type() == events::SceneGraphRemoveJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphRemoveJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphRemoveJoint*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphRemoveJoint>(*e));
  }
  else if (event->type() == events::SceneGraphReplaceJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphReplaceJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphReplaceJoint*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphReplaceJoint>(*e));
  }
  else if (event->type() == events::SceneGraphModifyLinkVisibility::kType)
  {
    assert(dynamic_cast<events::SceneGraphModifyLinkVisibility*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphModifyLinkVisibility*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphModifyLinkVisibility>(*e));
  }
  else if (event->type() == events::SceneGraphModifyLinkVisibilityALL::kType)
  {
    assert(dynamic_cast<events::SceneGraphModifyLinkVisibilityALL*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphModifyLinkVisibilityALL*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneGraphModifyLinkVisibilityALL>(*e));
  }
  else if (event->type() == events::SceneStateChanged::kType)
  {
    assert(dynamic_cast<events::SceneStateChanged*>(event) != nullptr);
    auto* e = static_cast<events::SceneStateChanged*>(event);
    if (e->getComponentInfo() == *component_info_ || e->getComponentInfo().isParent(*component_info_))
      events_.push_back(std::make_unique<events::SceneStateChanged>(*e));
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
