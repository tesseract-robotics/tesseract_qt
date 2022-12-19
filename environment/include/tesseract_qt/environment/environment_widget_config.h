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
#ifndef TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_WIDGET_CONFIG_H
#define TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_WIDGET_CONFIG_H

#include <memory>
#include <QObject>

#include <tesseract_qt/common/link_visibility_properties.h>
#include <tesseract_environment/environment.h>

class QStandardItemModel;

namespace tesseract_gui
{
class SceneGraphModel;
class SceneStateModel;
class KinematicGroupsModel;
class GroupTCPsModel;
class GroupJointStatesModel;
class AllowedCollisionMatrixModel;
class EnvironmentCommandsModel;

class EnvironmentWidgetConfig : public QObject
{
  Q_OBJECT
public:
  using Ptr = std::shared_ptr<EnvironmentWidgetConfig>;
  using ConstPtr = std::shared_ptr<const EnvironmentWidgetConfig>;
  using UPtr = std::unique_ptr<EnvironmentWidgetConfig>;
  using ConstUPtr = std::unique_ptr<const EnvironmentWidgetConfig>;

  EnvironmentWidgetConfig(std::string scene_name = "scene_name");
  ~EnvironmentWidgetConfig() override;

  /**
   * @brief Set environment
   * @param env The environment
   * @param scene_name
   */
  void setEnvironment(tesseract_environment::Environment::Ptr env);

  /**
   * @brief The the environment
   * @return The environment
   */
  const tesseract_environment::Environment& environment() const;
  tesseract_environment::Environment& environment();

  /**
   * @brief The the environment
   * @return The environment
   */
  tesseract_environment::Environment::ConstPtr getEnvironment() const;
  tesseract_environment::Environment::Ptr getEnvironment();

  /** @brief Get the link visibility properties */
  const LinkVisibilityPropertiesMap& getLinkVisibilityProperties() const;
  LinkVisibilityPropertiesMap& getLinkVisibilityProperties();

  SceneGraphModel& getSceneGraphModel();
  SceneStateModel& getSceneStateModel();
  KinematicGroupsModel& getKinematicGroupsModel();
  GroupTCPsModel& getGroupTCPsModel();
  GroupJointStatesModel& getGroupJointStatesModel();
  AllowedCollisionMatrixModel& getAllowedCollisionMatrixModel();
  EnvironmentCommandsModel& getEnvironmentCommandsModel();

  void clear();

  /**
   * @brief Check if config is valid
   * @return Returns true if environment is not a nullptr and is initialized
   */
  bool isValid() const;

  EnvironmentWidgetConfig::UPtr clone() const;

Q_SIGNALS:
  void environmentSet(const std::shared_ptr<tesseract_environment::Environment>& env);
  void environmentChanged(const tesseract_environment::Environment& env);
  void environmentCurrentStateChanged(const tesseract_environment::Environment& env);
  void modelsUpdated();

public Q_SLOTS:
  void onUpdateModels();
  void onUpdateSceneGraphModel();
  void onUpdateCurrentStateModel();
  void onUpdateAllowedCollisionMatrixModel();
  void onUpdateKinematicsInformationModels();
  void onUpdateCommandHistoryModel();

protected:
  struct Implementation;
  std::unique_ptr<Implementation> data_;

  void tesseractEventFilter(const tesseract_environment::Event& event);
};
}  // namespace tesseract_gui
#endif  // TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_WIDGET_CONFIG_H
