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

#include <tesseract_qt/common/utils.h>
#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/entity_container.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/events/allowed_collision_matrix_events.h>

#include <tesseract_qt/common/link_visibility.h>

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
  std::unordered_map<ComponentInfo, EntityContainer::Ptr> entity_containers;
  std::vector<std::unique_ptr<events::ComponentEvent>> events;

  void clear()
  {
    ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(component_info.scene_name);
    for (const auto& entity_container : entity_containers)
    {
      for (const auto& ns : entity_container.second->getTrackedEntities())
      {
        for (const auto& entity : ns.second)
          scene->DestroyNodeById(entity.second.id);
      }

      for (const auto& ns : entity_container.second->getUntrackedEntities())
      {
        for (const auto& entity : ns.second)
          scene->DestroyNodeById(entity.id);
      }

      entity_manager->removeEntityContainer(boost::uuids::to_string(entity_container.first.ns));
      entity_container.second->clear();
    }

    entity_containers.clear();
  }

  void clear(const ComponentInfo& ci)
  {
    assert(ci.scene_name == component_info.scene_name);

    auto it = entity_containers.find(ci);
    if (it != entity_containers.end())
    {
      ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(component_info.scene_name);
      for (const auto& ns : it->second->getTrackedEntities())
      {
        for (const auto& entity : ns.second)
          scene->DestroyNodeById(entity.second.id);
      }

      for (const auto& ns : it->second->getUntrackedEntities())
      {
        for (const auto& entity : ns.second)
          scene->DestroyNodeById(entity.id);
      }

      it->second->clear();

      entity_containers.erase(it);
      entity_manager->removeEntityContainer(boost::uuids::to_string(ci.ns));
    }
  }
};

SceneGraphRenderManager::SceneGraphRenderManager(ComponentInfo component_info,
                                                 std::shared_ptr<EntityManager> entity_manager)
  : data_(std::make_unique<Implementation>())
{
  data_->component_info = std::move(component_info);
  data_->entity_manager = std::move(entity_manager);

  qApp->installEventFilter(this);
}

SceneGraphRenderManager::~SceneGraphRenderManager() { data_->clear(); }

bool SceneGraphRenderManager::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::SceneGraphClear::kType)
  {
    assert(dynamic_cast<events::SceneGraphClear*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphClear*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphClear>(*e));
  }
  else if (event->type() == events::SceneGraphSet::kType)
  {
    assert(dynamic_cast<events::SceneGraphSet*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphSet*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphSet>(*e));
  }
  else if (event->type() == events::SceneGraphAddLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphAddLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphAddLink*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphAddLink>(*e));
  }
  else if (event->type() == events::SceneGraphAddJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphAddJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphAddJoint*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphAddJoint>(*e));
  }
  else if (event->type() == events::SceneGraphMoveLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphMoveLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphMoveLink*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphMoveLink>(*e));
  }
  else if (event->type() == events::SceneGraphMoveJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphMoveJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphMoveJoint*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphMoveJoint>(*e));
  }
  else if (event->type() == events::SceneGraphRemoveLink::kType)
  {
    assert(dynamic_cast<events::SceneGraphRemoveLink*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphRemoveLink*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphRemoveLink>(*e));
  }
  else if (event->type() == events::SceneGraphRemoveJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphRemoveJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphRemoveJoint*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphRemoveJoint>(*e));
  }
  else if (event->type() == events::SceneGraphReplaceJoint::kType)
  {
    assert(dynamic_cast<events::SceneGraphReplaceJoint*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphReplaceJoint*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphReplaceJoint>(*e));
  }
  else if (event->type() == events::SceneGraphModifyLinkVisibility::kType)
  {
    assert(dynamic_cast<events::SceneGraphModifyLinkVisibility*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphModifyLinkVisibility*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphModifyLinkVisibility>(*e));
  }
  else if (event->type() == events::SceneGraphModifyLinkVisibilityALL::kType)
  {
    assert(dynamic_cast<events::SceneGraphModifyLinkVisibilityALL*>(event) != nullptr);
    auto* e = static_cast<events::SceneGraphModifyLinkVisibilityALL*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
      data_->events.push_back(std::make_unique<events::SceneGraphModifyLinkVisibilityALL>(*e));
  }
  else if (event->type() == events::SceneStateChanged::kType)
  {
    assert(dynamic_cast<events::SceneStateChanged*>(event) != nullptr);
    auto* e = static_cast<events::SceneStateChanged*>(event);
    if (e->getComponentInfo() == data_->component_info || e->getComponentInfo().isParent(data_->component_info))
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

  auto getEntityContainer = [this](const ComponentInfo& component_info) -> EntityContainer::Ptr {
    auto it = data_->entity_containers.find(component_info);
    if (it != data_->entity_containers.end())
      return it->second;

    auto entity_container = data_->entity_manager->getEntityContainer(boost::uuids::to_string(component_info.ns));
    data_->entity_containers[component_info] = entity_container;
    return entity_container;
  };

  for (const auto& event : data_->events)
  {
    if (event->type() == events::SceneGraphClear::kType)
    {
      auto& e = static_cast<events::SceneGraphClear&>(*event);
      data_->clear(e.getComponentInfo());
    }
    else if (event->type() == events::SceneGraphSet::kType)
    {
      auto& e = static_cast<events::SceneGraphSet&>(*event);
      data_->clear(e.getComponentInfo());
      EntityContainer::Ptr entity_container = getEntityContainer(e.getComponentInfo());
      loadSceneGraph(*scene, *entity_container, *e.getSceneGraph(), "");
    }
    else if (event->type() == events::SceneGraphAddLink::kType)
    {
      auto& e = static_cast<events::SceneGraphAddLink&>(*event);
      EntityContainer::Ptr entity_container = getEntityContainer(e.getComponentInfo());
      scene->RootVisual()->AddChild(loadLink(*scene, *entity_container, *e.getLink()));
    }
    else if (event->type() == events::SceneGraphRemoveLink::kType)
    {
      auto& e = static_cast<events::SceneGraphRemoveLink&>(*event);
      EntityContainer::Ptr entity_container = getEntityContainer(e.getComponentInfo());
      if (entity_container->hasTrackedEntity(EntityContainer::VISUAL_NS, e.getLinkName()))
      {
        auto entity = entity_container->getTrackedEntity(EntityContainer::VISUAL_NS, e.getLinkName());
        scene->DestroyNodeById(entity.id);
      }
    }
    else if (event->type() == events::SceneGraphModifyLinkVisibility::kType)
    {
      auto& e = static_cast<events::SceneGraphModifyLinkVisibility&>(*event);
      EntityContainer::Ptr entity_container = getEntityContainer(e.getComponentInfo());
      for (const auto& link_name : e.getLinkNames())
      {
        if (entity_container->hasTrackedEntity(EntityContainer::VISUAL_NS, link_name))
        {
          auto link_entity = entity_container->getTrackedEntity(EntityContainer::VISUAL_NS, link_name);
          auto link_visual_node = scene->VisualById(link_entity.id);

          if (e.getVisibilityFlags() & LinkVisibilityFlags::LINK || e.getVisibilityFlags() & LinkVisibilityFlags::ALL)
            link_visual_node->SetUserData(USER_VISIBILITY, e.visible());

          bool link_visible = std::get<bool>(link_visual_node->UserData(USER_VISIBILITY));

          std::string visual_key = link_name + "::Visuals";
          if (entity_container->hasTrackedEntity(EntityContainer::VISUAL_NS, visual_key))
          {
            auto entity = entity_container->getTrackedEntity(EntityContainer::VISUAL_NS, visual_key);
            auto node = scene->VisualById(entity.id);
            if (e.getVisibilityFlags() & LinkVisibilityFlags::VISUAL ||
                e.getVisibilityFlags() & LinkVisibilityFlags::ALL)
              node->SetUserData(USER_VISIBILITY, e.visible());

            bool visible = std::get<bool>(node->UserData(USER_VISIBILITY));
            node->SetVisible(link_visible & visible);
          }

          visual_key = link_name + "::Collisions";
          if (entity_container->hasTrackedEntity(EntityContainer::VISUAL_NS, visual_key))
          {
            auto entity = entity_container->getTrackedEntity(EntityContainer::VISUAL_NS, visual_key);
            auto node = scene->VisualById(entity.id);
            if (e.getVisibilityFlags() & LinkVisibilityFlags::COLLISION ||
                e.getVisibilityFlags() & LinkVisibilityFlags::ALL)
              node->SetUserData(USER_VISIBILITY, e.visible());

            bool visible = std::get<bool>(node->UserData(USER_VISIBILITY));
            node->SetVisible(link_visible & visible);
          }

          visual_key = link_name + "::WireBox";
          if (entity_container->hasTrackedEntity(EntityContainer::VISUAL_NS, visual_key))
          {
            auto entity = entity_container->getTrackedEntity(EntityContainer::VISUAL_NS, visual_key);
            auto node = scene->VisualById(entity.id);
            if (e.getVisibilityFlags() & LinkVisibilityFlags::WIREBOX ||
                e.getVisibilityFlags() & LinkVisibilityFlags::ALL)
              node->SetUserData(USER_VISIBILITY, e.visible());

            bool visible = std::get<bool>(node->UserData(USER_VISIBILITY));
            node->SetVisible(visible);
          }

          visual_key = link_name + "::Axis";
          if (entity_container->hasTrackedEntity(EntityContainer::VISUAL_NS, visual_key))
          {
            auto entity = entity_container->getTrackedEntity(EntityContainer::VISUAL_NS, visual_key);
            auto node = scene->VisualById(entity.id);
            if (e.getVisibilityFlags() & LinkVisibilityFlags::AXIS || e.getVisibilityFlags() & LinkVisibilityFlags::ALL)
              node->SetUserData(USER_VISIBILITY, e.visible());

            bool visible = std::get<bool>(node->UserData(USER_VISIBILITY));
            node->SetVisible(visible);
          }
        }
      }
    }
    else if (event->type() == events::SceneGraphModifyLinkVisibilityALL::kType)
    {
      auto& e = static_cast<events::SceneGraphModifyLinkVisibilityALL&>(*event);
      EntityContainer::Ptr entity_container = getEntityContainer(e.getComponentInfo());
      for (const auto& ns : entity_container->getTrackedEntities(EntityContainer::VISUAL_NS))
      {
        std::vector<std::string> sub_ns = getNamespaces(ns.first);
        if (sub_ns.size() == 2)
        {
          if (sub_ns[1] == "Visuals" || sub_ns[1] == "Collisions")
          {
            auto link_entity = entity_container->getTrackedEntity(EntityContainer::VISUAL_NS, sub_ns[0]);
            auto link_visual_node = scene->VisualById(link_entity.id);

            if (e.getVisibilityFlags() & LinkVisibilityFlags::LINK)
            {
              link_visual_node->SetUserData(USER_VISIBILITY, e.visible());
              bool link_visible = std::get<bool>(link_visual_node->UserData(USER_VISIBILITY));

              auto node = scene->VisualById(ns.second.id);
              bool visible = std::get<bool>(node->UserData(USER_VISIBILITY));
              node->SetVisible(link_visible & visible);
            }

            if (sub_ns[1] == "Visuals" && e.getVisibilityFlags() & LinkVisibilityFlags::VISUAL)
            {
              if (e.visible())
                link_visual_node->SetUserData(USER_VISIBILITY, e.visible());

              bool link_visible = std::get<bool>(link_visual_node->UserData(USER_VISIBILITY));
              auto node = scene->VisualById(ns.second.id);
              node->SetUserData(USER_VISIBILITY, e.visible());
              node->SetVisible(link_visible && e.visible());
            }

            if (sub_ns[1] == "Collisions" && e.getVisibilityFlags() & LinkVisibilityFlags::COLLISION)
            {
              if (e.visible())
                link_visual_node->SetUserData(USER_VISIBILITY, e.visible());

              bool link_visible = std::get<bool>(link_visual_node->UserData(USER_VISIBILITY));
              auto node = scene->VisualById(ns.second.id);
              node->SetUserData(USER_VISIBILITY, e.visible());
              node->SetVisible(link_visible && e.visible());
            }
          }
          else if (sub_ns[1] == "Axis" && e.getVisibilityFlags() & LinkVisibilityFlags::AXIS)
          {
            auto node = scene->VisualById(ns.second.id);
            node->SetUserData(USER_VISIBILITY, e.visible());
            node->SetVisible(e.visible());
          }
          else if (sub_ns[1] == "WireBox" && e.getVisibilityFlags() & LinkVisibilityFlags::WIREBOX)
          {
            auto node = scene->VisualById(ns.second.id);
            node->SetUserData(USER_VISIBILITY, e.visible());
            node->SetVisible(e.visible());
          }
        }
      }
    }
    else if (event->type() == events::SceneStateChanged::kType)
    {
      auto& e = static_cast<events::SceneStateChanged&>(*event);
      EntityContainer::Ptr entity_container = getEntityContainer(e.getComponentInfo());
      for (const auto& pair : e.getState().link_transforms)
      {
        if (entity_container->hasTrackedEntity(EntityContainer::VISUAL_NS, pair.first))
        {
          Entity entity = entity_container->getTrackedEntity(EntityContainer::VISUAL_NS, pair.first);
          scene->VisualById(entity.id)->SetWorldPose(ignition::math::eigen3::convert(pair.second));
        }
      }
    }
  }

  data_->events.clear();
}

}  // namespace tesseract_gui
