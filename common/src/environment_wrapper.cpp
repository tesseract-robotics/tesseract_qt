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

#include <tesseract_qt/common/environment_wrapper.h>
#include <tesseract_qt/common/component_info.h>

#include <tesseract_qt/common/events/allowed_collision_matrix_events.h>
#include <tesseract_qt/common/events/contact_results_events.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/events/group_joint_states_events.h>
#include <tesseract_qt/common/events/group_tcps_events.h>
#include <tesseract_qt/common/events/kinematic_groups_events.h>
#include <tesseract_qt/common/events/environment_events.h>
#include <tesseract_qt/common/events/status_log_events.h>

#include <tesseract_collision/core/discrete_contact_manager.h>

#include <tesseract_scene_graph/link.h>
#include <tesseract_scene_graph/joint.h>
#include <tesseract_scene_graph/graph.h>
#include <tesseract_scene_graph/scene_state.h>

#include <tesseract_state_solver/state_solver.h>

#include <tesseract_environment/environment.h>
#include <tesseract_environment/environment_monitor.h>
#include <tesseract_environment/events.h>
#include <tesseract_environment/command.h>
#include <tesseract_environment/commands/add_contact_managers_plugin_info_command.h>

#include <tesseract_collision/core/common.h>
#include <tesseract_common/yaml_utils.h>
#include <tesseract_common/yaml_extensions.h>

#include <QApplication>

static const std::string DEFAULT_CONTACT_MANAGER_PLUGINS =
    R"(contact_manager_plugins:
         search_paths:
           - /usr/local/lib
         search_libraries:
           - tesseract_collision_bullet_factories
         discrete_plugins:
           default: BulletDiscreteBVHManager
           plugins:
             BulletDiscreteBVHManager:
               class: BulletDiscreteBVHManagerFactory)";

void applyDefaultContactManager(tesseract_environment::Environment& env)
{
  YAML::Node config;
  try
  {
    config = YAML::Load(DEFAULT_CONTACT_MANAGER_PLUGINS);
  }
  // LCOV_EXCL_START
  catch (...)
  {
    return;
  }
  // LCOV_EXCL_STOP

  const YAML::Node& cm_plugin_info = config[tesseract_common::ContactManagersPluginInfo::CONFIG_KEY];
  auto contact_managers_plugin_info = cm_plugin_info.as<tesseract_common::ContactManagersPluginInfo>();

  env.applyCommand(
      std::make_shared<tesseract_environment::AddContactManagersPluginInfoCommand>(contact_managers_plugin_info));
}

///////////////////////////////////////

void tesseractEventFilterHelper(const tesseract_environment::Event& event,
                                tesseract_gui::EnvironmentWrapper& env_wrapper,
                                int& current_revision)
{
  if (!env_wrapper.getEnvironment()->isInitialized())
    return;

  switch (event.type)
  {
    case tesseract_environment::Events::COMMAND_APPLIED:
    {
      const auto& e = static_cast<const tesseract_environment::CommandAppliedEvent&>(event);
      if (current_revision == 0 || e.revision < current_revision)
      {
        env_wrapper.broadcast();
      }
      else
      {
        /** @todo update to handle explicit commands */
        bool broadcast{ false };
        for (std::size_t i = current_revision; i < e.revision; ++i)
        {
          const auto& cmd = e.commands.at(i);
          switch (cmd->getType())
          {
            case tesseract_environment::CommandType::ADD_SCENE_GRAPH:
            case tesseract_environment::CommandType::ADD_LINK:
            case tesseract_environment::CommandType::ADD_TRAJECTORY_LINK:
            case tesseract_environment::CommandType::CHANGE_LINK_VISIBILITY:
            case tesseract_environment::CommandType::REMOVE_LINK:
            case tesseract_environment::CommandType::REMOVE_JOINT:
            {
              broadcast = true;
              break;
            }
            case tesseract_environment::CommandType::MOVE_LINK:
            case tesseract_environment::CommandType::MOVE_JOINT:
            case tesseract_environment::CommandType::REPLACE_JOINT:
            case tesseract_environment::CommandType::CHANGE_JOINT_ORIGIN:
            case tesseract_environment::CommandType::CHANGE_LINK_ORIGIN:
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
              tesseract_gui::events::StatusLogInfo event("Tesseract Qt Environment Wrapper, Unhandled environment "
                                                         "command");
              QApplication::sendEvent(qApp, &event);
            }
          }
        }
        if (broadcast)
          env_wrapper.broadcast();
      }
      current_revision = e.revision;
      break;
    }
    case tesseract_environment::Events::SCENE_STATE_CHANGED:
    {
      const auto& e = static_cast<const tesseract_environment::SceneStateChangedEvent&>(event);
      tesseract_gui::events::SceneStateChanged event(env_wrapper.getComponentInfo(), e.state);
      QApplication::sendEvent(qApp, &event);
      break;
    }
  }
}

void eventFilterHelper(QObject* /*obj*/,
                       QEvent* event,
                       const std::shared_ptr<const tesseract_gui::ComponentInfo>& component_info,
                       tesseract_environment::Environment& env)
{
  if (event->type() == tesseract_gui::events::EventType::CONTACT_RESULTS_COMPUTE)
  {
    assert(dynamic_cast<tesseract_gui::events::ContactResultsCompute*>(event) != nullptr);
    auto* e = static_cast<tesseract_gui::events::ContactResultsCompute*>(event);
    if (e->getComponentInfo() != component_info)
      return;

    tesseract_collision::ContactResultMap contacts;
    switch (e->getStateType())
    {
      case tesseract_gui::events::ContactResultsCompute::StateType::CURRENT_STATE:
      {
        tesseract_collision::DiscreteContactManager::UPtr contact_manager = env.getDiscreteContactManager();
        if (contact_manager == nullptr)
        {
          applyDefaultContactManager(env);
          contact_manager = env.getDiscreteContactManager();
        }
        contact_manager->applyContactManagerConfig(e->getContactManagerConfig());
        contact_manager->contactTest(contacts, e->getCollisionCheckConfig().contact_request);
        break;
      }
      case tesseract_gui::events::ContactResultsCompute::StateType::NAMED_STATE:
      {
        break;
      }
      case tesseract_gui::events::ContactResultsCompute::StateType::USER_DEFINED_STATE:
      {
        break;
      }
    }

    // Convert to tracked objects
    tesseract_gui::ContactResultMap tracked_object;
    for (const auto& contact : contacts)
    {
      tesseract_gui::ContactResultVector crv;
      for (const auto& result : contact.second)
        crv().emplace_back(tesseract_gui::ContactResult(result));

      tracked_object[contact.first] = crv;
    }

    tesseract_gui::events::ContactResultsSet event(component_info, tracked_object, e->getNamespace());
    QApplication::sendEvent(qApp, &event);
  }
  else if (event->type() == tesseract_gui::events::EventType::ACM_GENERATE)
  {
    assert(dynamic_cast<tesseract_gui::events::AllowedCollisionMatrixGenerate*>(event) != nullptr);
    auto* e = static_cast<tesseract_gui::events::AllowedCollisionMatrixGenerate*>(event);
    if (e->getComponentInfo() != component_info)
      return;

    tesseract_collision::DiscreteContactManager::UPtr contact_manager = env.getDiscreteContactManager();
    if (contact_manager == nullptr)
    {
      applyDefaultContactManager(env);
      contact_manager = env.getDiscreteContactManager();
    }

    auto state_solver = env.getStateSolver();

    // We want to disable the allowed contact function for this process so it is set null
    contact_manager->setContactAllowedValidator(nullptr);
    tesseract_collision::ContactResultMap results;
    tesseract_collision::ContactRequest request;
    request.type = tesseract_collision::ContactTestType::ALL;

    for (long i = 0; i < e->getResolution(); ++i)
    {
      tesseract_scene_graph::SceneState state = state_solver->getRandomState();
      contact_manager->setCollisionObjectsTransform(state.link_transforms);
      contact_manager->contactTest(results, request);
    }

    tesseract_common::AllowedCollisionMatrix acm;
    for (const auto& pair : results)
    {
      double percent = double(pair.second.size()) / double(e->getResolution());
      if (percent > 0.95)
      {
        std::vector<std::string> adj_first = env.getSceneGraph()->getAdjacentLinkNames(pair.first.first);
        std::vector<std::string> adj_second = env.getSceneGraph()->getAdjacentLinkNames(pair.first.second);
        if (std::find(adj_first.begin(), adj_first.end(), pair.first.second) != adj_first.end())
          acm.addAllowedCollision(pair.first.first, pair.first.second, "Adjacent");
        else if (std::find(adj_second.begin(), adj_second.end(), pair.first.first) != adj_second.end())
          acm.addAllowedCollision(pair.first.second, pair.first.first, "Adjacent");
        else
          acm.addAllowedCollision(pair.first.second, pair.first.first, "Allways");
      }
    }

    std::vector<std::string> link_names = env.getLinkNames();
    for (std::size_t i = 0; i < link_names.size() - 1; ++i)
    {
      const auto& link1 = env.getLink(link_names[i]);
      if (link1->collision.empty())
        continue;

      for (std::size_t j = i + 1; j < link_names.size(); ++j)
      {
        const auto& link2 = env.getLink(link_names[j]);
        if (link2->collision.empty())
          continue;

        if (results.find(tesseract_common::makeOrderedLinkPair(link_names[i], link_names[j])) == results.end())
          acm.addAllowedCollision(link_names[i], link_names[j], "Never");
      }
    }

    tesseract_gui::events::AllowedCollisionMatrixSet event(component_info, acm);
    QApplication::sendEvent(qApp, &event);
  }
  else if (event->type() == tesseract_gui::events::EventType::ENVIRONMENT_APPLY_COMMANDS)
  {
    assert(dynamic_cast<tesseract_gui::events::EnvironmentApplyCommand*>(event) != nullptr);
    auto* e = static_cast<tesseract_gui::events::EnvironmentApplyCommand*>(event);
    if (e->getComponentInfo() != component_info)
      return;

    env.applyCommands(e->getCommands());
  }
}

void broadcastHelper(const std::shared_ptr<const tesseract_gui::ComponentInfo>& component_info,
                     const tesseract_environment::Environment& env)
{
  auto lock = env.lockRead();

  if (!env.isInitialized())
    return;

  // Broadcast environment data
  tesseract_gui::events::SceneGraphSet set_scene_graph_event(component_info, env.getSceneGraph()->clone());
  QApplication::sendEvent(qApp, &set_scene_graph_event);

  tesseract_gui::events::SceneStateChanged scene_state_changed_event(component_info, env.getState());
  QApplication::sendEvent(qApp, &scene_state_changed_event);

  tesseract_gui::events::EnvironmentCommandsSet set_environment_commands_event(component_info, env.getCommandHistory());
  QApplication::sendEvent(qApp, &set_environment_commands_event);

  tesseract_gui::events::AllowedCollisionMatrixSet set_acm_event(component_info, *env.getAllowedCollisionMatrix());
  QApplication::sendEvent(qApp, &set_acm_event);

  auto kin_info = env.getKinematicsInformation();
  tesseract_gui::events::KinematicGroupsSet set_kin_groups_event(
      component_info, kin_info.chain_groups, kin_info.joint_groups, kin_info.link_groups);
  QApplication::sendEvent(qApp, &set_kin_groups_event);

  tesseract_gui::events::GroupJointStatesSet set_group_joint_states_event(component_info, kin_info.group_states);
  QApplication::sendEvent(qApp, &set_group_joint_states_event);

  tesseract_gui::events::GroupTCPsSet set_group_tcps_event(component_info, kin_info.group_tcps);
  QApplication::sendEvent(qApp, &set_group_tcps_event);
}

namespace tesseract_gui
{
EnvironmentWrapper::EnvironmentWrapper(std::shared_ptr<const ComponentInfo> component_info)
  : component_info_(std::move(component_info))
{
}

EnvironmentWrapper::~EnvironmentWrapper()
{
  // clear environment data
  events::SceneGraphClear clear_scene_graph_event(component_info_);
  QApplication::sendEvent(qApp, &clear_scene_graph_event);

  events::EnvironmentCommandsClear clear_environment_commands_event(component_info_);
  QApplication::sendEvent(qApp, &clear_environment_commands_event);

  events::AllowedCollisionMatrixClear clear_acm_event(component_info_);
  QApplication::sendEvent(qApp, &clear_acm_event);

  events::KinematicGroupsClear clear_kin_groups_event(component_info_);
  QApplication::sendEvent(qApp, &clear_kin_groups_event);

  events::GroupJointStatesClear clear_group_joint_states_event(component_info_);
  QApplication::sendEvent(qApp, &clear_group_joint_states_event);

  events::GroupTCPsClear clear_group_tcps_event(component_info_);
  QApplication::sendEvent(qApp, &clear_group_tcps_event);
}

std::shared_ptr<const ComponentInfo> EnvironmentWrapper::getComponentInfo() const { return component_info_; }

void EnvironmentWrapper::broadcast() const { broadcastHelper(getComponentInfo(), *getEnvironment()); }

void EnvironmentWrapper::init()
{
  if (initialized_)
    throw std::runtime_error("EnvironmentWrapper::init was called multipel times");

  // Get current revision
  revision_ = environment().getRevision();

  // Add environment event callback
  std::size_t uuid = std::hash<EnvironmentWrapper*>()(this);
  environment().addEventCallback(
      uuid, [this](const tesseract_environment::Event& event) { tesseractEventFilterHelper(event, *this, revision_); });

  // Broadcast data to initialize available widgets
  broadcast();

  initialized_ = true;
}

DefaultEnvironmentWrapper::DefaultEnvironmentWrapper(std::shared_ptr<tesseract_environment::Environment> env)
  : DefaultEnvironmentWrapper(nullptr, std::move(env))
{
}

DefaultEnvironmentWrapper::DefaultEnvironmentWrapper(std::shared_ptr<const ComponentInfo> component_info,
                                                     std::shared_ptr<tesseract_environment::Environment> env)
  : EnvironmentWrapper(std::move(component_info)), env_(std::move(env))
{
  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

DefaultEnvironmentWrapper::~DefaultEnvironmentWrapper()
{
  if (initialized_ && env_)
    env_->removeEventCallback(std::hash<EnvironmentWrapper*>()(this));
}

std::shared_ptr<const tesseract_environment::Environment> DefaultEnvironmentWrapper::getEnvironment() const
{
  return env_;
}

std::shared_ptr<tesseract_environment::Environment> DefaultEnvironmentWrapper::getEnvironment() { return env_; }

const tesseract_environment::Environment& DefaultEnvironmentWrapper::environment() const { return *env_; }

tesseract_environment::Environment& DefaultEnvironmentWrapper::environment() { return *env_; }

bool DefaultEnvironmentWrapper::eventFilter(QObject* obj, QEvent* event)
{
  eventFilterHelper(obj, event, getComponentInfo(), *env_);

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

MonitorEnvironmentWrapper::MonitorEnvironmentWrapper(
    std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor)
  : MonitorEnvironmentWrapper(nullptr, std::move(env_monitor))
{
}

MonitorEnvironmentWrapper::MonitorEnvironmentWrapper(
    std::shared_ptr<const ComponentInfo> component_info,
    std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor)
  : EnvironmentWrapper(std::move(component_info)), env_monitor_(std::move(env_monitor))
{
  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

MonitorEnvironmentWrapper::~MonitorEnvironmentWrapper()
{
  if (initialized_ && env_monitor_)
    environment().removeEventCallback(std::hash<EnvironmentWrapper*>()(this));
}

std::shared_ptr<const tesseract_environment::Environment> MonitorEnvironmentWrapper::getEnvironment() const
{
  return env_monitor_->getEnvironment();
}

std::shared_ptr<tesseract_environment::Environment> MonitorEnvironmentWrapper::getEnvironment()
{
  return env_monitor_->getEnvironment();
}

const tesseract_environment::Environment& MonitorEnvironmentWrapper::environment() const
{
  return env_monitor_->environment();
}

tesseract_environment::Environment& MonitorEnvironmentWrapper::environment() { return env_monitor_->environment(); }

std::shared_ptr<tesseract_environment::EnvironmentMonitor> MonitorEnvironmentWrapper::getEnvironmentMonitor()
{
  return env_monitor_;
}

std::shared_ptr<const tesseract_environment::EnvironmentMonitor>
MonitorEnvironmentWrapper::getEnvironmentMonitor() const
{
  return env_monitor_;
}

bool MonitorEnvironmentWrapper::eventFilter(QObject* obj, QEvent* event)
{
  eventFilterHelper(obj, event, getComponentInfo(), env_monitor_->environment());

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract_gui
