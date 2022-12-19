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

#include <tesseract_qt/environment/environment_widget_config.h>
#include <tesseract_qt/environment/environment_commands_model.h>
#include <tesseract_qt/scene_graph/scene_graph_model.h>
#include <tesseract_qt/scene_graph/scene_state_model.h>
#include <tesseract_qt/scene_graph/scene_graph_events.h>
#include <tesseract_qt/kinematic_groups/kinematic_groups_model.h>
#include <tesseract_qt/kinematic_groups/group_tcps_model.h>
#include <tesseract_qt/kinematic_groups/group_tcps_events.h>
#include <tesseract_qt/kinematic_groups/group_joint_states_model.h>
#include <tesseract_qt/kinematic_groups/group_joint_states_events.h>
#include <tesseract_qt/acm/allowed_collision_matrix_model.h>
#include <tesseract_qt/acm/allowed_collision_matrix_events.h>

#include <tesseract_environment/environment.h>

#include <qobjectdefs.h>

#include <QApplication>

namespace tesseract_gui
{
struct EnvironmentWidgetConfig::Implementation
{
  Implementation(std::string scene_name)
    : scene_name(std::move(scene_name))
    , scene_model(this->scene_name)
    //    , group_model(this->scene_name)
    //    , group_tcps_model(this->scene_name)
    , scene_state_model(this->scene_name)
    , group_states_model(this->scene_name)
    , acm_model(this->scene_name)
  //    , commands_model(this->scene_name)
  {
  }

  std::size_t hash;
  std::string scene_name;
  tesseract_environment::Environment::Ptr environment;

  // Store link visibility properties
  LinkVisibilityPropertiesMap link_visibility_properties;

  SceneGraphModel scene_model;
  SceneStateModel scene_state_model;
  KinematicGroupsModel group_model;
  GroupTCPsModel group_tcps_model;
  GroupJointStatesModel group_states_model;
  AllowedCollisionMatrixModel acm_model;
  EnvironmentCommandsModel commands_model;
};

EnvironmentWidgetConfig::EnvironmentWidgetConfig(std::string scene_name)
  : data_(std::make_unique<Implementation>(std::move(scene_name)))
{
  data_->hash = std::hash<EnvironmentWidgetConfig*>{}(this);
  data_->environment = std::make_shared<tesseract_environment::Environment>();
}

EnvironmentWidgetConfig::~EnvironmentWidgetConfig()
{
  // disconnect the callback
  if (data_->environment != nullptr)
    data_->environment->removeEventCallback(data_->hash);
}

void EnvironmentWidgetConfig::setEnvironment(tesseract_environment::Environment::Ptr env)
{
  // disconnect the callback
  if (data_->environment != nullptr)
    data_->environment->removeEventCallback(data_->hash);

  data_->environment = std::move(env);
  data_->environment->addEventCallback(
      data_->hash, [this](const tesseract_environment::Event& event) { this->tesseractEventFilter(event); });

  clear();

  onUpdateModels();

  emit environmentSet(data_->environment);
}

const tesseract_environment::Environment& EnvironmentWidgetConfig::environment() const { return *(data_->environment); }

tesseract_environment::Environment& EnvironmentWidgetConfig::environment() { return *(data_->environment); }

tesseract_environment::Environment::ConstPtr EnvironmentWidgetConfig::getEnvironment() const
{
  return data_->environment;
}

tesseract_environment::Environment::Ptr EnvironmentWidgetConfig::getEnvironment() { return data_->environment; }

const LinkVisibilityPropertiesMap& EnvironmentWidgetConfig::getLinkVisibilityProperties() const
{
  return data_->link_visibility_properties;
}

LinkVisibilityPropertiesMap& EnvironmentWidgetConfig::getLinkVisibilityProperties()
{
  return data_->link_visibility_properties;
}

SceneGraphModel& EnvironmentWidgetConfig::getSceneGraphModel() { return data_->scene_model; }
SceneStateModel& EnvironmentWidgetConfig::getSceneStateModel() { return data_->scene_state_model; }
KinematicGroupsModel& EnvironmentWidgetConfig::getKinematicGroupsModel() { return data_->group_model; }
GroupTCPsModel& EnvironmentWidgetConfig::getGroupTCPsModel() { return data_->group_tcps_model; }
GroupJointStatesModel& EnvironmentWidgetConfig::getGroupJointStatesModel() { return data_->group_states_model; }
AllowedCollisionMatrixModel& EnvironmentWidgetConfig::getAllowedCollisionMatrixModel() { return data_->acm_model; }
EnvironmentCommandsModel& EnvironmentWidgetConfig::getEnvironmentCommandsModel() { return data_->commands_model; }

void EnvironmentWidgetConfig::clear()
{
  data_->link_visibility_properties.clear();
  QApplication::sendEvent(qApp, new events::SceneGraphClear(data_->scene_name));
  QApplication::sendEvent(qApp, new events::AllowedCollisionMatrixClear(data_->scene_name));
  data_->group_model.clear();
  QApplication::sendEvent(qApp, new events::GroupJointStatesClear(data_->scene_name));
  QApplication::sendEvent(qApp, new events::GroupTCPsClear(data_->scene_name));
  data_->commands_model.clear();
}

bool EnvironmentWidgetConfig::isValid() const
{
  return (data_->environment != nullptr && data_->environment->isInitialized());
}

void EnvironmentWidgetConfig::onUpdateModels()
{
  onUpdateSceneGraphModel();
  onUpdateCurrentStateModel();
  onUpdateAllowedCollisionMatrixModel();
  onUpdateKinematicsInformationModels();
  onUpdateCommandHistoryModel();

  emit modelsUpdated();
}

void EnvironmentWidgetConfig::onUpdateSceneGraphModel()
{
  if (!data_->environment->isInitialized())
    return;

  QApplication::sendEvent(qApp, new events::SceneGraphSet(data_->scene_name, data_->environment->getSceneGraph()));

  // Update link visibility properties
  std::vector<std::string> link_names = data_->environment->getLinkNames();
  LinkVisibilityPropertiesMap link_prop;
  link_prop.reserve(link_names.size());
  for (const auto& link_name : link_names)
  {
    auto it = data_->link_visibility_properties.find(link_name);
    if (it == data_->link_visibility_properties.end())
      link_prop[link_name] = LinkVisibilityProperties();
    else
      link_prop[link_name] = it->second;
  }
  data_->link_visibility_properties = link_prop;
}

void EnvironmentWidgetConfig::onUpdateCurrentStateModel()
{
  if (!data_->environment->isInitialized())
    return;

  QApplication::sendEvent(qApp, new events::SceneStateChanged(data_->scene_name, data_->environment->getState()));
}
void EnvironmentWidgetConfig::onUpdateAllowedCollisionMatrixModel()
{
  if (!data_->environment->isInitialized())
    return;

  QApplication::sendEvent(
      qApp, new events::AllowedCollisionMatrixSet(data_->scene_name, *data_->environment->getAllowedCollisionMatrix()));
}

void EnvironmentWidgetConfig::onUpdateKinematicsInformationModels()
{
  if (!data_->environment->isInitialized())
    return;

  // Kinematics Information
  auto kin_info = data_->environment->getKinematicsInformation();

  // Kinematic Groups
  data_->group_model.set(kin_info.chain_groups, kin_info.joint_groups, kin_info.link_groups);

  // Groups States
  QApplication::sendEvent(qApp, new events::GroupJointStatesSet(data_->scene_name, kin_info.group_states));

  // Tool Center Points
  QApplication::sendEvent(qApp, new events::GroupTCPsSet(data_->scene_name, kin_info.group_tcps));
}

void EnvironmentWidgetConfig::onUpdateCommandHistoryModel()
{
  if (!data_->environment->isInitialized())
    return;

  data_->commands_model.set(data_->environment->getCommandHistory());
}

void EnvironmentWidgetConfig::tesseractEventFilter(const tesseract_environment::Event& event)
{
  switch (event.type)
  {
    case tesseract_environment::Events::COMMAND_APPLIED:
    {
      onUpdateModels();
      emit environmentChanged(*data_->environment);
      break;
    }
    case tesseract_environment::Events::SCENE_STATE_CHANGED:
    {
      onUpdateCurrentStateModel();
      emit environmentCurrentStateChanged(*data_->environment);
      break;
    }
  }
}

EnvironmentWidgetConfig::UPtr EnvironmentWidgetConfig::clone() const
{
  auto c = std::make_unique<EnvironmentWidgetConfig>();
  c->setEnvironment(data_->environment->clone());
  return c;
}

}  // namespace tesseract_gui
