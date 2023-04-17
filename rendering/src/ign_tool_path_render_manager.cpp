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
#include <tesseract_qt/rendering/ign_tool_path_render_manager.h>
#include <tesseract_qt/rendering/utils.h>

#include <tesseract_qt/common/events/tool_path_events.h>
#include <tesseract_qt/common/tool_path.h>
#include <tesseract_qt/common/entity_manager.h>
#include <tesseract_qt/common/entity_container.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <ignition/rendering/Scene.hh>
#include <ignition/rendering/AxisVisual.hh>
#include <ignition/rendering/ArrowVisual.hh>
#include <ignition/math/eigen3/Conversions.hh>

#include <tesseract_environment/environment.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <QApplication>
#include <QUuid>

const std::string USER_VISIBILITY = "user_visibility";
const std::string USER_PARENT_VISIBILITY = "user_parent_visibility";
const std::string USER_WORKING_FRAME = "user_working_frame";

namespace tesseract_gui
{
struct IgnToolPathRenderManager::Implementation
{
  EntityManager::Ptr entity_manager;
  std::map<boost::uuids::uuid, EntityContainer::Ptr> entity_containers;
  std::map<boost::uuids::uuid, std::pair<ignition::rendering::VisualPtr, std::string>> working_frames;

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

    entity_containers.erase(uuid);
    working_frames.erase(uuid);
  }

  void clearAll(const std::string& scene_name)
  {
    ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(scene_name);
    if (scene != nullptr)
    {
      for (auto& container : entity_containers)
        clear(*scene, *container.second);

      entity_containers.clear();
      working_frames.clear();
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

IgnToolPathRenderManager::IgnToolPathRenderManager(ComponentInfo component_info,
                                                   std::shared_ptr<EntityManager> entity_manager)
  : ToolPathRenderManager(std::move(component_info)), data_(std::make_unique<Implementation>())
{
  data_->entity_manager = std::move(entity_manager);
}

IgnToolPathRenderManager::~IgnToolPathRenderManager() { data_->clearAll(component_info_->scene_name); }

void IgnToolPathRenderManager::render()
{
  if (events_.empty())
  {
    updateWorkingFrameTransforms();
    return;
  }

  static const boost::uuids::uuid nil_uuid{};
  ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(component_info_->scene_name);

  for (const auto& event : events_)
  {
    if (event->type() == events::ToolPathAdd::kType)
    {
      auto& e = static_cast<events::ToolPathAdd&>(*event);
      // Clear it if it exists
      data_->clear(*scene, e.getToolPath().getUUID());
      EntityContainer::Ptr tool_path_container =
          data_->entity_manager->getEntityContainer(boost::uuids::to_string(e.getToolPath().getUUID()));
      data_->entity_containers[e.getToolPath().getUUID()] = tool_path_container;

      std::string tool_path_name = boost::uuids::to_string(e.getToolPath().getUUID());
      auto tool_path_entity =
          tool_path_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, tool_path_name);
      ignition::rendering::VisualPtr ign_tool_path =
          scene->CreateVisual(tool_path_entity.id, tool_path_entity.unique_name);
      ign_tool_path->SetUserData(USER_VISIBILITY, true);
      ign_tool_path->SetUserData(USER_PARENT_VISIBILITY, true);

      data_->working_frames[e.getToolPath().getUUID()] =
          std::make_pair(ign_tool_path, e.getToolPath().getWorkingFrame());

      for (const auto& segment : e.getToolPath())
      {
        std::string segment_name = boost::uuids::to_string(segment.getUUID());
        auto segment_entity =
            tool_path_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, segment_name);
        ignition::rendering::VisualPtr ign_segment = scene->CreateVisual(segment_entity.id, segment_entity.unique_name);
        ign_segment->SetUserData(USER_VISIBILITY, true);
        ign_segment->SetUserData(USER_PARENT_VISIBILITY, true);
        for (const auto& pose : segment)
        {
          std::string pose_name = boost::uuids::to_string(pose.getUUID());
          auto pose_entity =
              tool_path_container->addTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, pose_name);
          ignition::rendering::AxisVisualPtr axis = scene->CreateAxisVisual(pose_entity.id, pose_entity.unique_name);
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
    else if (event->type() == events::ToolPathRemove::kType)
    {
      auto& e = static_cast<events::ToolPathRemove&>(*event);
      data_->clear(*scene, e.getUUID());
    }
    else if (event->type() == events::ToolPathRemoveAll::kType)
    {
      auto& e = static_cast<events::ToolPathRemoveAll&>(*event);
      data_->clearAll(component_info_->scene_name);
    }
    else if (event->type() == events::ToolPathHideAll::kType)
    {
      for (const auto& container : data_->entity_containers)
        data_->setVisibility(*scene, container.first, nil_uuid, false, true);
    }
    else if (event->type() == events::ToolPathShowAll::kType)
    {
      for (const auto& container : data_->entity_containers)
        data_->setVisibility(*scene, container.first, nil_uuid, true, true);
    }
    else if (event->type() == events::ToolPathHide::kType)
    {
      auto& e = static_cast<events::ToolPathHide&>(*event);
      data_->setVisibility(*scene, e.getUUID(), e.getChildUUID(), false, false);
    }
    else if (event->type() == events::ToolPathShow::kType)
    {
      auto& e = static_cast<events::ToolPathShow&>(*event);
      data_->setVisibility(*scene, e.getUUID(), e.getChildUUID(), true, false);
    }
  }

  events_.clear();

  // Update tool path based on working frame
  updateWorkingFrameTransforms();
}

void IgnToolPathRenderManager::updateWorkingFrameTransforms()
{
  // Update tool path based on working frame
  if (!data_->working_frames.empty())
  {
    auto env_wrapper = EnvironmentManager::get(*component_info_);
    if (env_wrapper != nullptr && env_wrapper->getEnvironment()->isInitialized())
    {
      tesseract_scene_graph::SceneState state = env_wrapper->getEnvironment()->getState();
      for (auto& working_frame : data_->working_frames)
      {
        auto it = state.link_transforms.find(working_frame.second.second);
        if (it != state.link_transforms.end())
          working_frame.second.first->SetLocalPose(ignition::math::eigen3::convert(it->second));
      }
    }
  }
}
}  // namespace tesseract_gui
