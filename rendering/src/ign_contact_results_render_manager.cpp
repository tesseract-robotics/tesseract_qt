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
#include <tesseract_qt/rendering/ign_contact_results_render_manager.h>
#include <tesseract_qt/rendering/render_events.h>
#include <tesseract_qt/rendering/utils.h>

#include <tesseract_qt/common/events/contact_results_events.h>
#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/entity_container.h>
#include <tesseract_qt/common/component_info.h>

#include <ignition/rendering/Scene.hh>
#include <ignition/rendering/ArrowVisual.hh>
#include <ignition/math/eigen3/Conversions.hh>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <QApplication>
#include <QUuid>

#include <variant>

#include <console_bridge/console.h>

const std::string USER_VISIBILITY = "user_visibility";

namespace tesseract_gui
{
struct IgnContactResultsRenderManager::Implementation
{
  std::string scene_name;
  std::unordered_map<tesseract_gui::ComponentInfo, tesseract_gui::EntityManager::Ptr> entity_managers;

  void clear(ignition::rendering::Scene& scene, EntityContainer& container)
  {
    for (const auto& ns : container.getTrackedEntities())
    {
      for (const auto& entity : ns.second)
        scene.DestroyNodeById(entity.second.id);
    }

    for (const auto& ns : container.getUntrackedEntities())
    {
      for (const auto& entity : ns.second)
        scene.DestroyNodeById(entity.id);
    }

    container.clear();
  }

  void clear(ignition::rendering::Scene& scene, tesseract_gui::EntityManager& entity_manager)
  {
    for (auto& entity_container : entity_manager.getEntityContainers())
    {
      clear(scene, *entity_container.second);
      entity_manager.removeEntityContainer(entity_container.first);
    }
    entity_manager.clear();
  }

  void clearAll()
  {
    ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(scene_name);
    if (scene != nullptr)
    {
      for (auto& container : entity_managers)
        clear(*scene, *container.second);

      entity_managers.clear();
    }
  }
};

IgnContactResultsRenderManager::IgnContactResultsRenderManager(ComponentInfo component_info)
  : ContactResultsRenderManager(std::move(component_info)), data_(std::make_unique<Implementation>())
{
  data_->scene_name = component_info_->scene_name;
}

IgnContactResultsRenderManager::~IgnContactResultsRenderManager() { data_->clearAll(); }

void IgnContactResultsRenderManager::render()
{
  if (events_.empty())
    return;

  auto getEntityManager =
      [this](const tesseract_gui::ComponentInfo& component_info) -> tesseract_gui::EntityManager::Ptr {
    auto it = data_->entity_managers.find(component_info);
    if (it != data_->entity_managers.end())
      return it->second;

    auto entity_manager = std::make_shared<tesseract_gui::EntityManager>();
    data_->entity_managers[component_info] = entity_manager;
    return entity_manager;
  };

  ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(data_->scene_name);

  for (const auto& event : events_)
  {
    if (event->type() == tesseract_gui::events::ContactResultsClear::kType)
    {
      auto& e = static_cast<tesseract_gui::events::ContactResultsClear&>(*event);
      tesseract_gui::EntityManager::Ptr entity_manager = getEntityManager(e.getComponentInfo());
      data_->clear(*scene, *entity_manager);
    }
    else if (event->type() == events::ContactResultsRemove::kType)
    {
      auto& e = static_cast<tesseract_gui::events::ContactResultsRemove&>(*event);
      tesseract_gui::EntityManager::Ptr entity_manager = getEntityManager(e.getComponentInfo());
      const std::string parent_key = boost::uuids::to_string(e.getUUID());
      auto entity_container = entity_manager->getEntityContainer(parent_key);
      if (e.getChildUUID().is_nil())
      {
        data_->clear(*scene, *entity_container);
        entity_manager->removeEntityContainer(parent_key);
      }
      else
      {
        CONSOLE_BRIDGE_logError("IgnContactResultsRenderManager, removing child elements is currently not supported");
      }
    }
    else if (event->type() == tesseract_gui::events::ContactResultsSet::kType)
    {
      auto& e = static_cast<tesseract_gui::events::ContactResultsSet&>(*event);
      tesseract_gui::EntityManager::Ptr entity_manager = getEntityManager(e.getComponentInfo());
      data_->clear(*scene, *entity_manager);

      auto contacts = e.getContactResults();
      if (contacts.index() == 0)
      {
        const tesseract_gui::ContactResultVector& crv = std::get<tesseract_gui::ContactResultVector>(contacts);
        const std::string parent_key = boost::uuids::to_string(crv.getUUID());

        tesseract_gui::EntityContainer::Ptr entity_container = entity_manager->getEntityContainer(parent_key);
        auto entity = entity_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, parent_key);
        ignition::rendering::VisualPtr ign_contact_results = scene->CreateVisual(entity.id, entity.unique_name);
        ign_contact_results->SetUserData(USER_VISIBILITY, false);

        for (const auto& crt : crv())
        {
          const auto& cr = crt();

          Eigen::Vector3d direction = cr.nearest_points[1] - cr.nearest_points[0];
          Eigen::Isometry3d pose{ Eigen::Isometry3d::Identity() };
          pose.translation() = cr.nearest_points[0];
          Eigen::Quaterniond q;
          q.setFromTwoVectors(Eigen::Vector3d::UnitZ(), direction.normalized());
          pose.linear() = q.toRotationMatrix();

          const std::string arrow_key_name = boost::uuids::to_string(crt.getUUID());
          auto arrow_entity =
              entity_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, arrow_key_name);
          ignition::rendering::ArrowVisualPtr arrow =
              scene->CreateArrowVisual(arrow_entity.id, arrow_entity.unique_name);

          auto ign_material = scene->Material("TesseractContactResultsColor");
          if (ign_material == nullptr)
          {
            ign_material = scene->CreateMaterial("TesseractContactResultsColor");
            ign_material->SetAmbient(1, 0, 0, 1);
            ign_material->SetDiffuse(1, 0, 0, 1);
            ign_material->SetSpecular(1, 0, 0, 1);
          }

          arrow->SetMaterial(ign_material);
          arrow->SetLocalPose(ignition::math::eigen3::convert(pose));
          arrow->SetInheritScale(false);
          arrow->SetLocalScale(0.1, 0.1, direction.norm());
          arrow->SetUserData(USER_VISIBILITY, true);
          arrow->SetVisible(false);
          ign_contact_results->AddChild(arrow);
        }

        scene->RootVisual()->AddChild(ign_contact_results);
      }
      else
      {
        const tesseract_gui::ContactResultMap& crm = std::get<tesseract_gui::ContactResultMap>(contacts);
        for (const auto& pair : crm)
        {
          const std::string parent_key = boost::uuids::to_string(pair.second.getUUID());
          tesseract_gui::EntityContainer::Ptr entity_container = entity_manager->getEntityContainer(parent_key);
          auto entity = entity_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, parent_key);
          ignition::rendering::VisualPtr ign_contact_results = scene->CreateVisual(entity.id, entity.unique_name);
          ign_contact_results->SetUserData(USER_VISIBILITY, false);

          for (const auto& crt : pair.second())
          {
            const auto& cr = crt();
            Eigen::Vector3d direction = cr.nearest_points[1] - cr.nearest_points[0];
            Eigen::Isometry3d pose = Eigen::Isometry3d::Identity();
            pose.translation() = cr.nearest_points[0];
            Eigen::Quaterniond q;
            q.setFromTwoVectors(Eigen::Vector3d::UnitZ(), direction.normalized());
            pose.linear() = q.toRotationMatrix();

            std::string arrow_key_name = boost::uuids::to_string(crt.getUUID());
            auto arrow_entity =
                entity_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, arrow_key_name);
            ignition::rendering::ArrowVisualPtr arrow =
                scene->CreateArrowVisual(arrow_entity.id, arrow_entity.unique_name);

            auto ign_material = scene->Material("TesseractContactResultsColor");
            if (ign_material == nullptr)
            {
              ign_material = scene->CreateMaterial("TesseractContactResultsColor");
              ign_material->SetAmbient(1, 0, 0, 1);
              ign_material->SetDiffuse(1, 0, 0, 1);
              ign_material->SetSpecular(1, 0, 0, 1);
            }

            arrow->SetMaterial(ign_material);
            arrow->SetLocalPose(ignition::math::eigen3::convert(pose));
            arrow->SetInheritScale(false);
            arrow->SetLocalScale(0.1, 0.1, direction.norm());
            arrow->SetUserData(USER_VISIBILITY, true);
            arrow->SetVisible(false);

            ign_contact_results->AddChild(arrow);
          }

          scene->RootVisual()->AddChild(ign_contact_results);
        }
      }
    }
    else if (event->type() == events::ContactResultsVisbility::kType)
    {
      auto& e = static_cast<tesseract_gui::events::ContactResultsVisbility&>(*event);
      tesseract_gui::EntityManager::Ptr entity_manager = getEntityManager(e.getComponentInfo());
      const std::string parent_key = boost::uuids::to_string(e.getUUID());
      auto entity_container = entity_manager->getEntityContainer(parent_key);
      if (entity_container->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, parent_key))
      {
        auto parent_entity = entity_container->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, parent_key);
        auto parent_scene_node = scene->VisualById(parent_entity.id);
        auto parent_visibility = std::get<bool>(parent_scene_node->UserData(USER_VISIBILITY));
        if (e.getChildUUID().is_nil())
        {
          parent_visibility = e.getVisibility();
          parent_scene_node->SetUserData(USER_VISIBILITY, parent_visibility);

          auto entities = entity_container->getTrackedEntities(tesseract_gui::EntityContainer::VISUAL_NS);
          for (auto& entity : entities)
          {
            if (entity.second.id != parent_entity.id)
            {
              auto arrow = scene->VisualById(entity.second.id);
              auto user_visible = std::get<bool>(arrow->UserData(USER_VISIBILITY));
              arrow->SetVisible(parent_visibility & user_visible);
            }
          }
        }
        else
        {
          const std::string arrow_key_name = boost::uuids::to_string(e.getChildUUID());
          if (entity_container->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, arrow_key_name))
          {
            auto entity = entity_container->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, arrow_key_name);
            auto arrow = scene->VisualById(entity.id);
            arrow->SetUserData(USER_VISIBILITY, e.getVisibility());
            arrow->SetVisible(parent_visibility & e.getVisibility());
          }
        }
      }
    }
    else if (event->type() == events::ContactResultsVisbilityAll::kType)
    {
      auto& e = static_cast<tesseract_gui::events::ContactResultsVisbilityAll&>(*event);
      const bool visibility = e.getVisibility();
      tesseract_gui::EntityManager::Ptr entity_manager = getEntityManager(e.getComponentInfo());
      for (auto& container : entity_manager->getEntityContainers())
      {
        auto parent_entity =
            container.second->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, container.first);
        auto parent_scene_node = scene->VisualById(parent_entity.id);
        parent_scene_node->SetUserData(USER_VISIBILITY, visibility);

        auto entities = container.second->getTrackedEntities(tesseract_gui::EntityContainer::VISUAL_NS);
        for (auto& entity : entities)
        {
          if (entity.second.id != parent_entity.id)
          {
            auto arrow = scene->VisualById(entity.second.id);
            arrow->SetUserData(USER_VISIBILITY, visibility);
            arrow->SetVisible(visibility);
          }
        }
      }
    }
  }

  events_.clear();
}

}  // namespace tesseract_gui
