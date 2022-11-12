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
#include <tesseract_qt/rendering/render_environment_widget.h>
#include <tesseract_qt/rendering/conversions.h>
#include <tesseract_qt/rendering/events.h>
#include <tesseract_qt/rendering/utils.h>
#include <tesseract_qt/common/entity_container.h>
#include <tesseract_qt/common/link_visibility_properties.h>
#include <tesseract_environment/commands.h>
#include <ignition/rendering/Scene.hh>

#include <ignition/math/eigen3/Conversions.hh>

#include <QUuid>

namespace tesseract_gui
{
RenderEnvironmentWidget::RenderEnvironmentWidget(std::string scene_name, EntityManager& entity_manager, QWidget* parent)
  : EnvironmentWidget(parent)
  , container_name_(QUuid::createUuid().toString().toStdString())
  , scene_name_(std::move(scene_name))
  , entity_container_(entity_manager.getEntityContainer(container_name_))
{
  connect(this,
          SIGNAL(linkVisibilityChanged(std::vector<std::string>)),
          this,
          SLOT(onLinkVisibilityChanged(std::vector<std::string>)));
  connect(this,
          SIGNAL(showContactResults(tesseract_collision::ContactResultVector)),
          this,
          SLOT(onShowContactResults(tesseract_collision::ContactResultVector)));
  connect(this,
          SIGNAL(environmentSet(std::shared_ptr<tesseract_environment::Environment>)),
          this,
          SLOT(onEnvironmentSet(std::shared_ptr<tesseract_environment::Environment>)));
  connect(this,
          SIGNAL(environmentChanged(tesseract_environment::Environment)),
          this,
          SLOT(onEnvironmentChanged(tesseract_environment::Environment)));
  connect(this,
          SIGNAL(environmentCurrentStateChanged(tesseract_environment::Environment)),
          this,
          SLOT(onEnvironmentCurrentStateChanged(tesseract_environment::Environment)));

  qApp->installEventFilter(this);
}

RenderEnvironmentWidget::~RenderEnvironmentWidget()
{
  ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(scene_name_);
  if (scene != nullptr)
  {
    for (const auto& ns : entity_container_->getTrackedEntities())
    {
      for (const auto& entity : ns.second)
        scene->DestroyNodeById(entity.second.id);
    }

    for (const auto& ns : entity_container_->getUntrackedEntities())
    {
      for (const auto& entity : ns.second)
        scene->DestroyNodeById(entity.id);
    }

    entity_container_->clear();
  }
}

tesseract_gui::EnvironmentWidget* RenderEnvironmentWidget::clone() const
{
  return new RenderEnvironmentWidget(scene_name_, entity_container_->getEntityManager());
}

void RenderEnvironmentWidget::onEnvironmentSet(const std::shared_ptr<tesseract_environment::Environment>& /*env*/)
{
  render_revision_ = 0;
  render_dirty_ = true;
  render_reset_ = true;
  render_state_dirty_ = true;
  emit triggerRender();
}

void RenderEnvironmentWidget::onEnvironmentChanged(const tesseract_environment::Environment& /*env*/)
{
  render_dirty_ = true;
  render_state_dirty_ = true;
  emit triggerRender();
}

void RenderEnvironmentWidget::onEnvironmentCurrentStateChanged(const tesseract_environment::Environment& /*env*/)
{
  render_state_dirty_ = true;
  emit triggerRender();
}

void RenderEnvironmentWidget::onLinkVisibilityChanged(const std::vector<std::string>& links)
{
  link_visibility_properties_changed_ = links;
  render_dirty_ = true;
  emit triggerRender();
}

void RenderEnvironmentWidget::onShowContactResults(const tesseract_collision::ContactResultVector& contact_results)
{
  contact_results_changed_ = contact_results;
  render_contact_results_dirty_ = true;
  render_dirty_ = true;
  emit triggerRender();
}

bool RenderEnvironmentWidget::eventFilter(QObject* _obj, QEvent* _event)
{
  if (_event->type() == events::PreRender::kType)
  {
    assert(dynamic_cast<events::PreRender*>(_event) != nullptr);
    if (static_cast<events::PreRender*>(_event)->getSceneName() == scene_name_ && render_dirty_)
    {
      ignition::rendering::ScenePtr scene = sceneFromFirstRenderEngine(scene_name_);
      if (scene != nullptr && render_dirty_)
      {
        if (render_reset_)  // Remove all
        {
          if (!entity_container_->empty())
          {
            for (const auto& ns : entity_container_->getTrackedEntities())
            {
              for (const auto& entity : ns.second)
                scene->DestroyNodeById(entity.second.id);
            }

            for (const auto& ns : entity_container_->getUntrackedEntities())
            {
              for (const auto& entity : ns.second)
                scene->DestroyNodeById(entity.id);
            }

            entity_container_->clear();
            render_link_names_.clear();
            render_revision_ = 0;
            render_state_timestamp_ = environment().getCurrentStateTimestamp();
          }
          render_reset_ = false;
        }

        {  // Check environment
          auto lock = environment().lockRead();
          auto revision = environment().getRevision();
          auto state_timestamp = environment().getCurrentStateTimestamp();
          if (render_dirty_ || revision > render_revision_)
          {
            if (revision > render_revision_)
            {
              tesseract_environment::Commands commands = environment().getCommandHistory();

              bool links_removed{ false };
              for (std::size_t i = render_revision_; i < commands.size(); ++i)
              {
                const tesseract_environment::Command::ConstPtr& command = commands.at(i);
                switch (command->getType())
                {
                  case tesseract_environment::CommandType::ADD_SCENE_GRAPH:
                  {
                    auto cmd = std::static_pointer_cast<const tesseract_environment::AddSceneGraphCommand>(command);
                    auto link_names =
                        loadSceneGraph(*scene, *entity_container_, *cmd->getSceneGraph(), cmd->getPrefix());
                    render_link_names_.insert(render_link_names_.end(), link_names.begin(), link_names.end());
                    break;
                  }
                  case tesseract_environment::CommandType::ADD_LINK:
                  {
                    auto cmd = std::static_pointer_cast<const tesseract_environment::AddLinkCommand>(command);
                    scene->RootVisual()->AddChild(loadLink(*scene, *entity_container_, *cmd->getLink()));
                    render_link_names_.push_back(cmd->getLink()->getName());
                    break;
                  }
                  case tesseract_environment::CommandType::CHANGE_LINK_VISIBILITY:
                  {
                    auto cmd =
                        std::static_pointer_cast<const tesseract_environment::ChangeLinkVisibilityCommand>(command);
                    auto entity = entity_container_->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS,
                                                                      cmd->getLinkName());
                    auto visual_node = scene->VisualById(entity.id);
                    if (visual_node != nullptr)
                      visual_node->SetVisible(cmd->getEnabled());
                    break;
                  }
                  case tesseract_environment::CommandType::REMOVE_LINK:
                  case tesseract_environment::CommandType::REMOVE_JOINT:
                  {
                    links_removed = true;
                    break;
                  }
                  case tesseract_environment::CommandType::MOVE_LINK:
                  case tesseract_environment::CommandType::MOVE_JOINT:
                  case tesseract_environment::CommandType::REPLACE_JOINT:
                  case tesseract_environment::CommandType::CHANGE_JOINT_ORIGIN:
                  case tesseract_environment::CommandType::CHANGE_LINK_ORIGIN:
                  {
                    render_state_dirty_ = true;
                    break;
                  }
                  case tesseract_environment::CommandType::CHANGE_LINK_COLLISION_ENABLED:
                  case tesseract_environment::CommandType::MODIFY_ALLOWED_COLLISIONS:
                  case tesseract_environment::CommandType::REMOVE_ALLOWED_COLLISION_LINK:
                  case tesseract_environment::CommandType::CHANGE_JOINT_POSITION_LIMITS:
                  case tesseract_environment::CommandType::CHANGE_JOINT_VELOCITY_LIMITS:
                  case tesseract_environment::CommandType::CHANGE_JOINT_ACCELERATION_LIMITS:
                  case tesseract_environment::CommandType::ADD_KINEMATICS_INFORMATION:
                  case tesseract_environment::CommandType::CHANGE_COLLISION_MARGINS:
                  case tesseract_environment::CommandType::ADD_CONTACT_MANAGERS_PLUGIN_INFO:
                  case tesseract_environment::CommandType::SET_ACTIVE_CONTINUOUS_CONTACT_MANAGER:
                  case tesseract_environment::CommandType::SET_ACTIVE_DISCRETE_CONTACT_MANAGER:
                  {
                    break;
                  }
                  // LCOV_EXCL_START
                  default:
                  {
                    CONSOLE_BRIDGE_logError("IgnitionEnvironmentWidget, Unhandled environment command");
                    //                success &= false;
                  }
                    // LCOV_EXCL_STOP
                }
              }

              if (links_removed)
              {
                std::vector<std::string> link_names = environment().getLinkNames();
                std::vector<std::string> diff;

                std::sort(link_names.begin(), link_names.end());
                std::sort(render_link_names_.begin(), render_link_names_.end());

                std::set_difference(render_link_names_.begin(),
                                    render_link_names_.end(),
                                    link_names.begin(),
                                    link_names.end(),
                                    std::inserter(diff, diff.begin()));

                for (const auto& removed_link : diff)
                {
                  auto entity =
                      entity_container_->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, removed_link);
                  scene->DestroyNodeById(entity.id);
                }
              }
              render_revision_ = revision;
            }
          }

          if (render_state_dirty_ || state_timestamp > render_state_timestamp_)
          {
            tesseract_scene_graph::SceneState state = environment().getState();
            for (const auto& pair : state.link_transforms)
            {
              Entity entity =
                  entity_container_->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, pair.first);
              scene->VisualById(entity.id)->SetWorldPose(ignition::math::eigen3::convert(pair.second));
            }
            render_state_dirty_ = false;
            render_state_timestamp_ = state_timestamp;
          }
        }

        {  // Update Link Visibility
          auto link_visibility_properties = getLinkVisibilityProperties();
          for (const auto& l : link_visibility_properties_changed_)
          {
            auto link_visibility_property = link_visibility_properties.at(l);

            {  // Link Property
              if (entity_container_->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, l))
              {
                auto entity = entity_container_->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, l);
                auto visual_node = scene->VisualById(entity.id);
                if (visual_node != nullptr)
                  visual_node->SetVisible(link_visibility_property.link);
              }
            }

            {  // Link Visual Property
              std::string visual_key = l + "::Visuals";
              if (entity_container_->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key))
              {
                auto entity =
                    entity_container_->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key);
                auto visual_node = scene->VisualById(entity.id);
                if (visual_node != nullptr)
                  visual_node->SetVisible(link_visibility_property.link && link_visibility_property.visual);
              }
            }

            {  // Link Collision Property
              std::string visual_key = l + "::Collisions";
              if (entity_container_->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key))
              {
                auto entity =
                    entity_container_->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key);
                auto visual_node = scene->VisualById(entity.id);
                if (visual_node != nullptr)
                  visual_node->SetVisible(link_visibility_property.link && link_visibility_property.collision);
              }
            }

            {  // Link WireBox Property
              std::string visual_key = l + "::WireBox";
              if (entity_container_->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key))
              {
                auto entity =
                    entity_container_->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key);
                auto visual_node = scene->VisualById(entity.id);
                if (visual_node != nullptr)
                  visual_node->SetVisible(link_visibility_property.link && link_visibility_property.wirebox);
              }
            }

            {  // Link Axis Property
              std::string visual_key = l + "::Axis";
              if (entity_container_->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key))
              {
                auto entity =
                    entity_container_->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, visual_key);
                auto visual_node = scene->VisualById(entity.id);
                if (visual_node != nullptr)
                  visual_node->SetVisible(link_visibility_property.link && link_visibility_property.axis);
              }
            }
          }
          link_visibility_properties_changed_.clear();
        }

        if (render_contact_results_dirty_)
        {
          // Update contact results
          if (entity_container_->hasTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, "ContactResults"))
          {
            auto entity =
                entity_container_->getTrackedEntity(tesseract_gui::EntityContainer::VISUAL_NS, "ContactResults");
            scene->DestroyNodeById(entity.id);
          }

          if (!contact_results_changed_.empty())
          {
            auto visual_node = loadContactResults(*scene, *entity_container_, contact_results_changed_);
            scene->RootVisual()->AddChild(visual_node);
          }

          contact_results_changed_.clear();
          render_contact_results_dirty_ = false;
        }
        render_dirty_ = false;
      }
    }
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

}  // namespace tesseract_gui
