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

#include <tesseract_environment/environment.h>
#include <tesseract_environment/environment_monitor.h>
#include <tesseract_collision/core/common.h>

#include <QApplication>

namespace tesseract_gui
{
EnvironmentWrapper::EnvironmentWrapper(ComponentInfo component_info)
  : component_info_(std::make_unique<ComponentInfo>(std::move(component_info)))
{
}

EnvironmentWrapper::~EnvironmentWrapper() = default;

const ComponentInfo& EnvironmentWrapper::getComponentInfo() const { return *component_info_; }

///////////////////////////////////////

void tesseractEventFilterHelper(const tesseract_environment::Event& event,
                                const ComponentInfo& component_info,
                                const tesseract_environment::Environment& env)
{
  switch (event.type)
  {
    case tesseract_environment::Events::COMMAND_APPLIED:
    {
      //      QApplication::sendEvent(qApp, new events::ToolPathRemoveAll(component_info));
      //      onUpdateModels();
      //      emit environmentChanged(*data_->environment);
      //      break;
    }
    case tesseract_environment::Events::SCENE_STATE_CHANGED:
    {
      //      onUpdateCurrentStateModel();
      //      emit environmentCurrentStateChanged(*data_->environment);
      //      break;
    }
  }
}

void eventFilterHelper(QObject* /*obj*/,
                       QEvent* event,
                       const ComponentInfo& component_info,
                       const tesseract_environment::Environment& env)
{
  if (event->type() == events::ContactResultsCompute::kType)
  {
    assert(dynamic_cast<events::ContactResultsCompute*>(event) != nullptr);
    auto* e = static_cast<events::ContactResultsCompute*>(event);
    if (e->getComponentInfo() != component_info)
      return;

    tesseract_collision::ContactResultMap contacts;
    switch (e->getStateType())
    {
      case events::ContactResultsCompute::StateType::CURRENT_STATE:
      {
        auto contact_manager = env.getDiscreteContactManager();
        contact_manager->applyContactManagerConfig(e->getConfig().contact_manager_config);
        contact_manager->contactTest(contacts, e->getConfig().contact_request);
        break;
      }
      case events::ContactResultsCompute::StateType::NAMED_STATE:
      {
        break;
      }
      case events::ContactResultsCompute::StateType::USER_DEFINED_STATE:
      {
        break;
      }
    }

    // Convert to tracked objects
    ContactResultMap tracked_object;
    for (const auto& contact : contacts)
    {
      ContactResultVector crv;
      for (const auto& result : contact.second)
        crv().push_back(ContactResult(result));

      tracked_object[contact.first] = crv;
    }

    QApplication::sendEvent(qApp, new events::ContactResultsSet(component_info, tracked_object, e->getNamespace()));
  }
  else if (event->type() == events::AllowedCollisionMatrixGenerate::kType)
  {
    assert(dynamic_cast<events::AllowedCollisionMatrixGenerate*>(event) != nullptr);
    auto* e = static_cast<events::AllowedCollisionMatrixGenerate*>(event);
    if (e->getComponentInfo() != component_info)
      return;

    auto contact_manager = env.getDiscreteContactManager();
    auto state_solver = env.getStateSolver();

    // We want to disable the allowed contact function for this process so it is set null
    contact_manager->setIsContactAllowedFn(nullptr);
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

        if (results.find(tesseract_collision::getObjectPairKey(link_names[i], link_names[j])) == results.end())
          acm.addAllowedCollision(link_names[i], link_names[j], "Never");
      }
    }

    QApplication::sendEvent(qApp, new events::AllowedCollisionMatrixSet(component_info, acm));
  }
}

void broadcastHelper(const ComponentInfo& component_info, const tesseract_environment::Environment& env)
{
  auto lock = env.lockRead();

  // Broadcast environment data
  QApplication::sendEvent(qApp, new events::SceneGraphSet(component_info, env.getSceneGraph()->clone()));
  QApplication::sendEvent(qApp, new events::SceneStateChanged(component_info, env.getState()));
  QApplication::sendEvent(qApp, new events::EnvironmentCommandsSet(component_info, env.getCommandHistory()));
  QApplication::sendEvent(qApp,
                          new events::AllowedCollisionMatrixSet(component_info, *env.getAllowedCollisionMatrix()));

  auto kin_info = env.getKinematicsInformation();
  QApplication::sendEvent(qApp,
                          new events::KinematicGroupsSet(
                              component_info, kin_info.chain_groups, kin_info.joint_groups, kin_info.link_groups));
  QApplication::sendEvent(qApp, new events::GroupJointStatesSet(component_info, kin_info.group_states));
  QApplication::sendEvent(qApp, new events::GroupTCPsSet(component_info, kin_info.group_tcps));
}

DefaultEnvironmentWrapper::DefaultEnvironmentWrapper(std::shared_ptr<tesseract_environment::Environment> env)
  : DefaultEnvironmentWrapper(ComponentInfo(), std::move(env))
{
}

DefaultEnvironmentWrapper::DefaultEnvironmentWrapper(ComponentInfo component_info,
                                                     std::shared_ptr<tesseract_environment::Environment> env)
  : EnvironmentWrapper(std::move(component_info)), env_(std::move(env))
{
  std::size_t uuid = std::hash<DefaultEnvironmentWrapper*>()(this);
  env_->addEventCallback(uuid, [this](const tesseract_environment::Event& event) {
    tesseractEventFilterHelper(event, getComponentInfo(), *env_);
  });

  // Broadcast data to initialize available widgets
  broadcastHelper(getComponentInfo(), *env_);

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

DefaultEnvironmentWrapper::~DefaultEnvironmentWrapper() = default;

std::shared_ptr<const tesseract_environment::Environment> DefaultEnvironmentWrapper::getEnvironment() const
{
  return env_;
}

void DefaultEnvironmentWrapper::broadcast() const { broadcastHelper(getComponentInfo(), *env_); }

bool DefaultEnvironmentWrapper::eventFilter(QObject* obj, QEvent* event)
{
  eventFilterHelper(obj, event, getComponentInfo(), *env_);

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

MonitorEnvironmentWrapper::MonitorEnvironmentWrapper(
    std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor)
  : MonitorEnvironmentWrapper(ComponentInfo(), std::move(env_monitor))
{
}

MonitorEnvironmentWrapper::MonitorEnvironmentWrapper(
    ComponentInfo component_info,
    std::shared_ptr<tesseract_environment::EnvironmentMonitor> env_monitor)
  : EnvironmentWrapper(std::move(component_info)), env_monitor_(std::move(env_monitor))
{
  std::size_t uuid = std::hash<MonitorEnvironmentWrapper*>()(this);
  env_monitor_->environment().addEventCallback(uuid, [this](const tesseract_environment::Event& event) {
    tesseractEventFilterHelper(event, getComponentInfo(), env_monitor_->environment());
  });

  broadcastHelper(getComponentInfo(), env_monitor_->environment());

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}
MonitorEnvironmentWrapper::~MonitorEnvironmentWrapper() = default;

std::shared_ptr<const tesseract_environment::Environment> MonitorEnvironmentWrapper::getEnvironment() const
{
  return env_monitor_->getEnvironment();
}

void MonitorEnvironmentWrapper::broadcast() const { broadcastHelper(getComponentInfo(), env_monitor_->environment()); }

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