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

#include <tesseract_qt/rendering/scene_graph_render_manager.h>
#include <tesseract_qt/rendering/render_events.h>
#include <tesseract_qt/rendering/utils.h>
#include <tesseract_qt/rendering/conversions.h>

#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/entity_container.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/events/scene_graph_events.h>

#include <QApplication>

#include <ignition/math/eigen3/Conversions.hh>

#include <tesseract_scene_graph/scene_state.h>

#include <boost/uuid/uuid_io.hpp>

const std::string USER_VISIBILITY = "user_visibility";
const std::string USER_PARENT_VISIBILITY = "user_parent_visibility";

namespace tesseract_gui
{
struct SceneGraphRenderManager::Implementation
{
  ComponentInfo component_info;
  EntityManager::Ptr entity_manager;
  EntityContainer::Ptr entity_container;
  std::vector<std::unique_ptr<events::ComponentEvent>> events;

  void clear()
  {
    ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(component_info.scene_name);
    for (const auto& ns : entity_container->getTrackedEntities())
    {
      for (const auto& entity : ns.second)
        scene->DestroyNodeById(entity.second.id);
    }

    for (const auto& ns : entity_container->getUntrackedEntities())
    {
      for (const auto& entity : ns.second)
        scene->DestroyNodeById(entity.id);
    }

    entity_container->clear();
  }

  //  /**
  //   * @brief Currently there is a bug in ignition which does not correctly handle set visibility. This is a
  //   workaround to
  //   * solve the issue until a long term fix is introduced.
  //   * @details https://github.com/gazebosim/gz-rendering/issues/771
  //   */
  //  void updateVisibility(const ignition::rendering::NodePtr& node, bool parent_visibility, bool recursive)
  //  {
  //    for (std::size_t i = 0; i < node->ChildCount(); ++i)
  //    {
  //      auto child_node = node->ChildByIndex(i);
  //      auto axis = std::dynamic_pointer_cast<ignition::rendering::AxisVisual>(child_node);
  //      if (axis != nullptr)
  //      {
  //        if (recursive)
  //        {
  //          axis->SetVisible(parent_visibility);
  //          axis->SetUserData(USER_PARENT_VISIBILITY, parent_visibility);
  //          axis->SetUserData(USER_VISIBILITY, parent_visibility);
  //        }
  //        else
  //        {
  //          bool local_visibility = std::get<bool>(axis->UserData(USER_VISIBILITY));
  //          axis->SetVisible(parent_visibility & local_visibility);
  //          axis->SetUserData(USER_PARENT_VISIBILITY, parent_visibility);
  //        }
  //      }
  //      else
  //      {
  //        auto visual = std::dynamic_pointer_cast<ignition::rendering::Visual>(child_node);
  //        if (visual != nullptr)
  //        {
  //          if (recursive)
  //          {
  //            visual->SetVisible(parent_visibility);
  //            visual->SetUserData(USER_PARENT_VISIBILITY, parent_visibility);
  //            visual->SetUserData(USER_VISIBILITY, parent_visibility);
  //            updateVisibility(child_node, parent_visibility, recursive);
  //          }
  //          else
  //          {
  //            bool local_visibility = std::get<bool>(visual->UserData(USER_VISIBILITY));
  //            visual->SetVisible(parent_visibility & local_visibility);
  //            visual->SetUserData(USER_PARENT_VISIBILITY, parent_visibility);
  //            updateVisibility(child_node, parent_visibility & local_visibility, recursive);
  //          }
  //        }
  //        else
  //        {
  //          updateVisibility(child_node, parent_visibility, recursive);
  //        }
  //      }
  //    }
  //  }

  //  void setVisibility(ignition::rendering::Scene& scene,
  //                     const boost::uuids::uuid& uuid,
  //                     const boost::uuids::uuid& child_uuid,
  //                     bool visible,
  //                     bool recursive)
  //  {
  //    auto it = entity_containers.find(uuid);
  //    if (it != entity_containers.end())
  //    {
  //      std::string visual_key = boost::uuids::to_string(uuid);
  //      if (!child_uuid.is_nil())
  //        visual_key = boost::uuids::to_string(child_uuid);

  //      if (it->second->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key))
  //      {
  //        auto entity = it->second->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key);
  //        auto visual_node = scene.VisualById(entity.id);
  //        if (visual_node != nullptr)
  //        {
  //          bool parent_visibility = std::get<bool>(visual_node->UserData(USER_PARENT_VISIBILITY));
  //          visual_node->SetVisible(parent_visibility & visible);
  //          visual_node->SetUserData(USER_VISIBILITY, visible);
  //          auto axis = std::dynamic_pointer_cast<ignition::rendering::AxisVisual>(visual_node);
  //          // There is a bug in the axis visual object which shows the rings so must hide
  //          if (axis == nullptr)
  //            updateVisibility(visual_node, visible, recursive);
  //        }
  //      }
  //    }
  //  }
};

SceneGraphRenderManager::SceneGraphRenderManager(ComponentInfo component_info,
                                                 std::shared_ptr<EntityManager> entity_manager)
  : data_(std::make_unique<Implementation>())
{
  data_->component_info = std::move(component_info);
  data_->entity_manager = std::move(entity_manager);
  data_->entity_container = data_->entity_manager->getEntityContainer(boost::uuids::to_string(component_info.ns));

  qApp->installEventFilter(this);
}

SceneGraphRenderManager::~SceneGraphRenderManager() { data_->clear(); }

bool SceneGraphRenderManager::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::SceneGraphClear::kType)
  {
    assert(dynamic_cast<events::SceneGraphClear*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphClear*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphClear>(*e));
  }
  else if (event->type() == events::SceneGraphSet::kType)
  {
    assert(dynamic_cast<events::SceneGraphSet*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphSet*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphSet>(*e));
  }
  else if (event->type() == events::SceneGraphAddLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphAddLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphAddLink*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphAddLink>(*e));
  }
  else if (event->type() == events::SceneGraphAddJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphAddJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphAddJoint*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphAddJoint>(*e));
  }
  else if (event->type() == events::SceneGraphMoveLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphMoveLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphMoveLink*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphMoveLink>(*e));
  }
  else if (event->type() == events::SceneGraphMoveJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphMoveJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphMoveJoint*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphMoveJoint>(*e));
  }
  else if (event->type() == events::SceneGraphRemoveLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphRemoveLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphRemoveLink*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphRemoveLink>(*e));
  }
  else if (event->type() == events::SceneGraphRemoveJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphRemoveJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphRemoveJoint*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphRemoveJoint>(*e));
  }
  else if (event->type() == events::SceneGraphReplaceJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphReplaceJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphReplaceJoint*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphReplaceJoint>(*e));
  }
  else if (event->type() == events::SceneGraphModifyLinkVisibility::kType)
  {
    assert(dynamic_cast<events::SceneGraphModifyLinkVisibility*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphModifyLinkVisibility*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphModifyLinkVisibility>(*e));
  }
  else if (event->type() == events::SceneGraphModifyLinkVisibilityALL::kType)
  {
    assert(dynamic_cast<events::SceneGraphModifyLinkVisibilityALL*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphModifyLinkVisibilityALL*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneGraphModifyLinkVisibilityALL>(*e));
  }
  else if (event->type() == events::SceneStateChanged::kType)
  {
    assert(dynamic_cast<events::SceneStateChanged*>(event) != nullptr);
    auto* e = static_cast<events::SceneStateChanged*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->events.push_back(std::make_unique<events::SceneStateChanged>(*e));
  }
  else if (event->type() == events::PreRender::kType)
  {
    assert(dynamic_cast<events::PreRender*>(event) != nullptr);
    if (static_cast<events::PreRender*>(event)->getSceneName() == data_->component_info.scene_name)
      render();
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

void SceneGraphRenderManager::render()
{
  if (data_->events.empty())
    return;

  ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(data_->component_info.scene_name);

  for (const auto& event : data_->events)
  {
    if (event->type() == events::SceneGraphClear::kType)
    {
      data_->clear();
    }
    else if (event->type() == events::SceneGraphSet::kType)
    {
      auto& e = static_cast<events::SceneGraphSet&>(*event);
      loadSceneGraph(*scene, *data_->entity_container, *e.getSceneGraph(), "");
    }
    else if (event->type() == events::SceneGraphAddLink::kType)
    {
      auto& e = static_cast<events::SceneGraphAddLink&>(*event);
      scene->RootVisual()->AddChild(loadLink(*scene, *data_->entity_container, *e.getLink()));
    }
    else if (event->type() == events::SceneGraphRemoveLink::kType)
    {
      auto& e = static_cast<events::SceneGraphRemoveLink&>(*event);
      if (data_->entity_container->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, e.getLinkName()))
      {
        auto entity =
            data_->entity_container->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, e.getLinkName());
        scene->DestroyNodeById(entity.id);
      }
    }
    else if (event->type() == events::SceneGraphModifyLinkVisibility::kType)
    {
      auto& e = static_cast<events::SceneGraphModifyLinkVisibility&>(*event);
    }
    else if (event->type() == events::SceneGraphModifyLinkVisibilityALL::kType)
    {
      auto& e = static_cast<events::SceneGraphModifyLinkVisibilityALL&>(*event);
    }
    else if (event->type() == events::SceneStateChanged::kType)
    {
      auto& e = static_cast<events::SceneStateChanged&>(*event);
      for (const auto& pair : e.getState().link_transforms)
      {
        if (data_->entity_container->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, pair.first))
        {
          Entity entity =
              data_->entity_container->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, pair.first);
          scene->VisualById(entity.id)->SetWorldPose(ignition::math::eigen3::convert(pair.second));
        }
      }
    }
  }

  data_->events.clear();
}

}  // namespace tesseract_gui
