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
#include <tesseract_qt/rendering/tool_path_render_manager.h>
#include <tesseract_qt/rendering/events.h>
#include <tesseract_qt/rendering/utils.h>

#include <tesseract_qt/tool_path/tool_path_events.h>

#include <tesseract_common/tool_path.h>
#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/entity_container.h>

#include <ignition/rendering/Scene.hh>
#include <ignition/rendering/AxisVisual.hh>
#include <ignition/math/eigen3/Conversions.hh>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <QApplication>
#include <QUuid>

namespace tesseract_gui
{
struct ToolPathRenderManagerPrivate
{
  std::string scene_name;
  EntityManager::Ptr entity_manager;
  std::map<boost::uuids::uuid, EntityContainer::Ptr> entity_containers;
  bool render_dirty{ true };
  bool render_reset{ true };
  bool render_show_all{ true };
  bool render_hide_all{ false };

  std::vector<tesseract_common::ToolPath> added;
  std::vector<boost::uuids::uuid> removed;
  std::vector<boost::uuids::uuid> show;
  std::vector<boost::uuids::uuid> hide;

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

  void clear(ignition::rendering::Scene& scene, const boost::uuids::uuid& uuid)
  {
    auto it = entity_containers.find(uuid);
    if (it != entity_containers.end())
      clear(scene, *(it->second));
  }

  void clearAll()
  {
    ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(scene_name);
    if (scene != nullptr)
    {
      for (auto& container : entity_containers)
        clear(*scene, *container.second);

      entity_containers.clear();
    }
  }

  void setVisibility(ignition::rendering::Scene& scene, const boost::uuids::uuid& uuid, bool visible)
  {
    auto it = entity_containers.find(uuid);
    if (it != entity_containers.end())
    {
      std::string visual_key = boost::uuids::to_string(uuid);
      if (it->second->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key))
      {
        auto entity = it->second->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key);
        auto visual_node = scene.VisualById(entity.id);
        if (visual_node != nullptr)
          visual_node->SetVisible(visible);
      }
    }
  }
};

ToolPathRenderManager::ToolPathRenderManager(std::string scene_name, std::shared_ptr<EntityManager> entity_manager)
  : data_(std::make_unique<ToolPathRenderManagerPrivate>())
{
  data_->scene_name = std::move(scene_name);
  data_->entity_manager = std::move(entity_manager);

  qApp->installEventFilter(this);
}

ToolPathRenderManager::~ToolPathRenderManager() { data_->clearAll(); }

bool ToolPathRenderManager::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::ToolPathAdd::kType)
  {
    assert(dynamic_cast<events::ToolPathAdd*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathAdd*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->added.push_back(e->getToolPath());
      data_->render_dirty = true;
    }
  }
  else if (event->type() == events::ToolPathRemove::kType)
  {
    assert(dynamic_cast<events::ToolPathRemove*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathRemove*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->removed.push_back(e->getUUID());
      data_->render_dirty = true;
    }
  }
  else if (event->type() == events::ToolPathRemoveAll::kType)
  {
    assert(dynamic_cast<events::ToolPathRemoveAll*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathRemoveAll*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->render_dirty = true;
      data_->render_reset = true;
    }
  }
  else if (event->type() == events::ToolPathHideAll::kType)
  {
    assert(dynamic_cast<events::ToolPathHideAll*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathHideAll*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->render_dirty = true;
      data_->render_hide_all = true;
    }
  }
  else if (event->type() == events::ToolPathShowAll::kType)
  {
    assert(dynamic_cast<events::ToolPathShowAll*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathShowAll*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->render_dirty = true;
      data_->render_show_all = true;
    }
  }
  else if (event->type() == events::ToolPathHide::kType)
  {
    assert(dynamic_cast<events::ToolPathHide*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathHide*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->hide.push_back(e->getUUID());
      data_->render_dirty = true;
    }
  }
  else if (event->type() == events::ToolPathShow::kType)
  {
    assert(dynamic_cast<events::ToolPathShow*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathShow*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->show.push_back(e->getUUID());
      data_->render_dirty = true;
    }
  }
  else if (event->type() == events::PreRender::kType)
  {
    assert(dynamic_cast<events::PreRender*>(event) != nullptr);
    if (static_cast<events::PreRender*>(event)->getSceneName() == data_->scene_name && data_->render_dirty)
    {
      ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(data_->scene_name);
      if (scene != nullptr && data_->render_dirty)
      {
        if (data_->render_reset)  // Remove all
        {
          data_->clearAll();
          data_->render_reset = false;
        }

        // Check Added
        if (!data_->added.empty())
        {
          for (const auto& tool_path : data_->added)
          {
            // Clear it if it exists
            data_->clear(*scene, tool_path.getUUID());
            EntityContainer::Ptr tool_path_container =
                data_->entity_manager->getEntityContainer(boost::uuids::to_string(tool_path.getUUID()));
            data_->entity_containers[tool_path.getUUID()] = tool_path_container;

            std::string tool_path_name = boost::uuids::to_string(tool_path.getUUID());
            auto tool_path_entity =
                tool_path_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, tool_path_name);
            ignition::rendering::VisualPtr ign_tool_path =
                scene->CreateVisual(tool_path_entity.id, tool_path_entity.unique_name);
            for (const auto& segment : tool_path)
            {
              std::string segment_name = tool_path_name + "::" + boost::uuids::to_string(segment.getUUID());
              auto segment_entity =
                  tool_path_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, segment_name);
              ignition::rendering::VisualPtr ign_segment =
                  scene->CreateVisual(segment_entity.id, segment_entity.unique_name);
              for (const auto& pose : segment)
              {
                auto pose_entity = tool_path_container->addUntrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS);
                ignition::rendering::AxisVisualPtr axis =
                    scene->CreateAxisVisual(pose_entity.id, pose_entity.unique_name);
                axis->SetLocalPose(ignition::math::eigen3::convert(pose));
                axis->SetInheritScale(false);
                axis->Scale(0.05, 0.05, 0.05);
                axis->SetVisible(true);

                ign_segment->AddChild(axis);
              }
              ign_tool_path->AddChild(ign_segment);
            }
            scene->RootVisual()->AddChild(ign_tool_path);
          }
          data_->added.clear();
        }

        // Check Removed
        if (!data_->removed.empty())
        {
          for (const auto& r : data_->removed)
            data_->clear(*scene, r);

          data_->removed.clear();
        }

        // Check Hide
        if (!data_->hide.empty())
        {
          for (const auto& h : data_->hide)
            data_->setVisibility(*scene, h, false);

          data_->hide.clear();
        }

        // Check Show
        if (!data_->show.empty())
        {
          for (const auto& s : data_->show)
            data_->setVisibility(*scene, s, true);

          data_->show.clear();
        }

        // Check Hide All
        if (data_->render_hide_all)
        {
          for (const auto& e : data_->entity_containers)
            data_->setVisibility(*scene, e.first, false);
        }

        // Check Show All
        if (data_->render_show_all)
        {
          for (const auto& e : data_->entity_containers)
            data_->setVisibility(*scene, e.first, true);
        }

        data_->render_dirty = false;
      }
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
