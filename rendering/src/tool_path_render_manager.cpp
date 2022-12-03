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

#include <tesseract_qt/common/tool_path.h>
#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/entity_container.h>

#include <ignition/rendering/Scene.hh>
#include <ignition/rendering/AxisVisual.hh>
#include <ignition/rendering/ArrowVisual.hh>
#include <ignition/math/eigen3/Conversions.hh>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <QApplication>
#include <QUuid>

const std::string USER_VISIBILITY = "user_visibility";
const std::string USER_PARENT_VISIBILITY = "user_parent_visibility";

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

  std::vector<tesseract_gui::ToolPath> added;
  std::vector<boost::uuids::uuid> removed;
  std::vector<std::pair<boost::uuids::uuid, boost::uuids::uuid>> show;
  std::vector<std::pair<boost::uuids::uuid, boost::uuids::uuid>> hide;

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

  /**
   * @brief Currently there is a bug in ignition which does not correctly handle set visibility. This is a workaround to
   * solve the issue until a long term fix is introduced.
   * @details https://github.com/gazebosim/gz-rendering/issues/771
   */
  void updateVisibility(const ignition::rendering::NodePtr& node, bool parent_visibility, bool recursive)
  {
    for (std::size_t i = 0; i < node->ChildCount(); ++i)
    {
      auto child_node = node->ChildByIndex(i);
      auto axis = std::dynamic_pointer_cast<ignition::rendering::AxisVisual>(child_node);
      if (axis != nullptr)
      {
        if (recursive)
        {
          axis->SetVisible(parent_visibility);
          axis->SetUserData(USER_PARENT_VISIBILITY, parent_visibility);
          axis->SetUserData(USER_VISIBILITY, parent_visibility);
        }
        else
        {
          bool local_visibility = std::get<bool>(axis->UserData(USER_VISIBILITY));
          axis->SetVisible(parent_visibility & local_visibility);
          axis->SetUserData(USER_PARENT_VISIBILITY, parent_visibility);
        }
      }
      else
      {
        auto visual = std::dynamic_pointer_cast<ignition::rendering::Visual>(child_node);
        if (visual != nullptr)
        {
          if (recursive)
          {
            visual->SetVisible(parent_visibility);
            visual->SetUserData(USER_PARENT_VISIBILITY, parent_visibility);
            visual->SetUserData(USER_VISIBILITY, parent_visibility);
            updateVisibility(child_node, parent_visibility, recursive);
          }
          else
          {
            bool local_visibility = std::get<bool>(visual->UserData(USER_VISIBILITY));
            visual->SetVisible(parent_visibility & local_visibility);
            visual->SetUserData(USER_PARENT_VISIBILITY, parent_visibility);
            updateVisibility(child_node, parent_visibility & local_visibility, recursive);
          }
        }
        else
        {
          updateVisibility(child_node, parent_visibility, recursive);
        }
      }
    }
  }

  void setVisibility(ignition::rendering::Scene& scene,
                     const boost::uuids::uuid& uuid,
                     const boost::uuids::uuid& child_uuid,
                     bool visible,
                     bool recursive)
  {
    auto it = entity_containers.find(uuid);
    if (it != entity_containers.end())
    {
      std::string visual_key = boost::uuids::to_string(uuid);
      if (!child_uuid.is_nil())
        visual_key = boost::uuids::to_string(child_uuid);

      if (it->second->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key))
      {
        auto entity = it->second->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key);
        auto visual_node = scene.VisualById(entity.id);
        if (visual_node != nullptr)
        {
          bool parent_visibility = std::get<bool>(visual_node->UserData(USER_PARENT_VISIBILITY));
          visual_node->SetVisible(parent_visibility & visible);
          visual_node->SetUserData(USER_VISIBILITY, visible);
          auto axis = std::dynamic_pointer_cast<ignition::rendering::AxisVisual>(visual_node);
          // There is a bug in the axis visual object which shows the rings so must hide
          if (axis == nullptr)
            updateVisibility(visual_node, visible, recursive);
        }
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
      data_->hide.push_back(std::make_pair(e->getUUID(), e->getChildUUID()));
      data_->render_dirty = true;
    }
  }
  else if (event->type() == events::ToolPathShow::kType)
  {
    assert(dynamic_cast<events::ToolPathShow*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathShow*>(event);
    if (e->getSceneName() == data_->scene_name)
    {
      data_->show.push_back(std::make_pair(e->getUUID(), e->getChildUUID()));
      data_->render_dirty = true;
    }
  }
  else if (event->type() == events::PreRender::kType)
  {
    static const boost::uuids::uuid nil_uuid{};

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
            ign_tool_path->SetUserData(USER_VISIBILITY, true);
            ign_tool_path->SetUserData(USER_PARENT_VISIBILITY, true);
            for (const auto& segment : tool_path)
            {
              std::string segment_name = boost::uuids::to_string(segment.getUUID());
              auto segment_entity =
                  tool_path_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, segment_name);
              ignition::rendering::VisualPtr ign_segment =
                  scene->CreateVisual(segment_entity.id, segment_entity.unique_name);
              ign_segment->SetUserData(USER_VISIBILITY, true);
              ign_segment->SetUserData(USER_PARENT_VISIBILITY, true);
              for (const auto& pose : segment)
              {
                std::string pose_name = boost::uuids::to_string(pose.getUUID());
                auto pose_entity =
                    tool_path_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, pose_name);
                ignition::rendering::AxisVisualPtr axis =
                    scene->CreateAxisVisual(pose_entity.id, pose_entity.unique_name);
                axis->SetLocalPose(ignition::math::eigen3::convert(pose.getTransform()));
                axis->SetInheritScale(false);
                axis->SetLocalScale(0.05, 0.05, 0.05);
                axis->SetUserData(USER_VISIBILITY, true);
                axis->SetUserData(USER_PARENT_VISIBILITY, true);

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
            data_->setVisibility(*scene, h.first, h.second, false, false);

          data_->hide.clear();
        }

        // Check Show
        if (!data_->show.empty())
        {
          for (const auto& s : data_->show)
            data_->setVisibility(*scene, s.first, s.second, true, false);

          data_->show.clear();
        }

        // Check Hide All
        if (data_->render_hide_all)
        {
          for (const auto& e : data_->entity_containers)
            data_->setVisibility(*scene, e.first, nil_uuid, false, true);

          data_->render_hide_all = false;
        }

        // Check Show All
        if (data_->render_show_all)
        {
          for (const auto& e : data_->entity_containers)
            data_->setVisibility(*scene, e.first, nil_uuid, true, true);

          data_->render_show_all = false;
        }

        data_->render_dirty = false;
      }
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
