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

#include <gz/rendering/Scene.hh>
#include <gz/rendering/ArrowVisual.hh>
#include <gz/math/eigen3/Conversions.hh>

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
  std::map<std::shared_ptr<const ComponentInfo>, tesseract_gui::EntityManager::Ptr> entity_managers;

  void clear(gz::rendering::Scene& scene, EntityContainer& container)
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

  void clear(gz::rendering::Scene& scene, tesseract_gui::EntityManager& entity_manager)
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
    gz::rendering::ScenePtr scene = sceneFromFirstRenderEngine(scene_name);
    if (scene != nullptr)
    {
      for (auto& container : entity_managers)
        clear(*scene, *container.second);

      entity_managers.clear();
    }
  }

  gz::rendering::MaterialPtr getRedMaterial(gz::rendering::Scene& scene)
  {
    auto ign_material = scene.Material("TesseractContactResultsRedColor");
    if (ign_material == nullptr)
    {
      ign_material = scene.CreateMaterial("TesseractContactResultsRedColor");
      ign_material->SetAmbient(1, 0, 0, 1);
      ign_material->SetDiffuse(1, 0, 0, 1);
      ign_material->SetSpecular(1, 0, 0, 1);
    }
    return ign_material;
  }

  gz::rendering::MaterialPtr getBlueMaterial(gz::rendering::Scene& scene)
  {
    auto ign_material = scene.Material("TesseractContactResultsBlueColor");
    if (ign_material == nullptr)
    {
      ign_material = scene.CreateMaterial("TesseractContactResultsBlueColor");
      ign_material->SetAmbient(0, 0, 1, 1);
      ign_material->SetDiffuse(0, 0, 1, 1);
      ign_material->SetSpecular(0, 0, 1, 1);
    }
    return ign_material;
  }

  gz::rendering::VisualPtr createArrow(const tesseract_collision::ContactResult& cr,
                                       gz::rendering::Scene& scene,
                                       Entity arrow_entity)
  {
    // Calculate Pose
    Eigen::Vector3d direction = cr.nearest_points[1] - cr.nearest_points[0];
    Eigen::Isometry3d pose{ Eigen::Isometry3d::Identity() };
    pose.translation() = cr.nearest_points[0];
    Eigen::Quaterniond q;
    q.setFromTwoVectors(Eigen::Vector3d::UnitZ(), direction.normalized());
    pose.linear() = q.toRotationMatrix();

    const double length = direction.norm();
    const double head_height = 0.025;
    const double shaft_length = length - head_height;
    const double shaft_diameter = 0.01;
    const double point_diameter = 0.02;

    // Create Head
    Eigen::Isometry3d head_pose{ Eigen::Isometry3d::Identity() };
    head_pose.translation() = Eigen::Vector3d(0, 0, length - (head_height / 2.0));
    head_pose = pose * head_pose;

    gz::rendering::VisualPtr ign_head = scene.CreateVisual();
    ign_head->SetLocalPose(gz::math::eigen3::convert(head_pose));
    ign_head->AddGeometry(scene.CreateCone());
    ign_head->SetLocalScale(head_height, head_height, head_height);
    ign_head->SetInheritScale(false);
    ign_head->SetMaterial(getRedMaterial(scene));

    // Create Shaft
    Eigen::Isometry3d shaft_pose{ Eigen::Isometry3d::Identity() };
    shaft_pose.translation() = Eigen::Vector3d(0, 0, shaft_length / 2.0);
    shaft_pose = pose * shaft_pose;

    gz::rendering::VisualPtr ign_shaft = scene.CreateVisual();
    ign_shaft->SetLocalPose(gz::math::eigen3::convert(shaft_pose));
    ign_shaft->AddGeometry(scene.CreateCylinder());
    ign_shaft->SetLocalScale(shaft_diameter, shaft_diameter, shaft_length);
    ign_shaft->SetInheritScale(false);
    ign_shaft->SetMaterial(getRedMaterial(scene));

    // Create contact point 0
    gz::rendering::VisualPtr ign_point0 = scene.CreateVisual();
    Eigen::Isometry3d point0{ Eigen::Isometry3d::Identity() };
    point0.translation() = cr.nearest_points[0];

    ign_point0->SetLocalPose(gz::math::eigen3::convert(point0));
    ign_point0->AddGeometry(scene.CreateSphere());
    ign_point0->SetLocalScale(point_diameter, point_diameter, point_diameter);
    ign_point0->SetInheritScale(false);
    ign_point0->SetMaterial(getBlueMaterial(scene));

    // Create contact point 1
    gz::rendering::VisualPtr ign_point1 = scene.CreateVisual();
    Eigen::Isometry3d point1{ Eigen::Isometry3d::Identity() };
    point1.translation() = cr.nearest_points[1];

    ign_point1->SetLocalPose(gz::math::eigen3::convert(point1));
    ign_point1->AddGeometry(scene.CreateSphere());
    ign_point1->SetLocalScale(point_diameter, point_diameter, point_diameter);
    ign_point1->SetInheritScale(false);
    ign_point1->SetMaterial(getBlueMaterial(scene));

    gz::rendering::VisualPtr ign_contact_result = scene.CreateVisual(arrow_entity.id, arrow_entity.unique_name);
    ign_contact_result->AddChild(ign_head);
    ign_contact_result->AddChild(ign_shaft);
    ign_contact_result->AddChild(ign_point0);
    ign_contact_result->AddChild(ign_point1);
    ign_contact_result->SetUserData(USER_VISIBILITY, true);
    ign_contact_result->SetVisible(false);

    return ign_contact_result;
  }
};

IgnContactResultsRenderManager::IgnContactResultsRenderManager(std::shared_ptr<const ComponentInfo> component_info)
  : ContactResultsRenderManager(std::move(component_info)), data_(std::make_unique<Implementation>())
{
  data_->scene_name = component_info_->getSceneName();
}

IgnContactResultsRenderManager::~IgnContactResultsRenderManager() { data_->clearAll(); }

void IgnContactResultsRenderManager::render()
{
  if (events_.empty())
    return;

  auto getEntityManager =
      [this](const std::shared_ptr<const ComponentInfo>& component_info) -> tesseract_gui::EntityManager::Ptr {
    auto it = data_->entity_managers.find(component_info);
    if (it != data_->entity_managers.end())
      return it->second;

    auto entity_manager = std::make_shared<tesseract_gui::EntityManager>();
    data_->entity_managers[component_info] = entity_manager;
    return entity_manager;
  };

  gz::rendering::ScenePtr scene = sceneFromFirstRenderEngine(data_->scene_name);

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
        gz::rendering::VisualPtr ign_contact_results = scene->CreateVisual(entity.id, entity.unique_name);
        ign_contact_results->SetUserData(USER_VISIBILITY, false);

        for (const auto& crt : crv())
        {
          const auto& cr = crt();

          const std::string arrow_key_name = boost::uuids::to_string(crt.getUUID());
          auto arrow_entity =
              entity_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, arrow_key_name);

          ign_contact_results->AddChild(data_->createArrow(cr, *scene, arrow_entity));
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
          gz::rendering::VisualPtr ign_contact_results = scene->CreateVisual(entity.id, entity.unique_name);
          ign_contact_results->SetUserData(USER_VISIBILITY, false);

          for (const auto& crt : pair.second())
          {
            const auto& cr = crt();

            std::string arrow_key_name = boost::uuids::to_string(crt.getUUID());
            auto arrow_entity =
                entity_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, arrow_key_name);

            ign_contact_results->AddChild(data_->createArrow(cr, *scene, arrow_entity));
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
