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
#ifndef TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_WIDGET_H
#define TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_WIDGET_H

#include <QWidget>
#include <memory>
#include <tesseract_environment/environment.h>

namespace Ui
{
class EnvironmentWidget;
}

class QStandardItem;

namespace tesseract_gui
{
class EnvironmentWidgetConfig;
class EnvironmentWidgetImpl;
struct LinkVisibilityProperties;

class EnvironmentWidget : public QWidget
{
  Q_OBJECT

public:
  explicit EnvironmentWidget(QWidget* parent = nullptr, bool add_toolbar = true);
  ~EnvironmentWidget() override;

  /**
   * @brief Configure the environment widget
   * @param config The environment widgets configuration
   */
  void setConfiguration(std::shared_ptr<EnvironmentWidgetConfig> config);

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
  const std::unordered_map<std::string, LinkVisibilityProperties>& getLinkVisibilityProperties() const;

  /**
   * @brief Make a clone of the widget
   * @return A unique clone of the widget
   */
  virtual EnvironmentWidget* clone() const;

Q_SIGNALS:
  void configurationSet(EnvironmentWidgetConfig& config);
  void environmentSet(const std::shared_ptr<tesseract_environment::Environment>& env);
  void environmentChanged(const tesseract_environment::Environment& env);
  void environmentCurrentStateChanged(const tesseract_environment::Environment& env);
  void linkVisibilityChanged(const std::vector<std::string>& links);
  void showContactResults(const tesseract_collision::ContactResultVector& contact_results);
  void triggerRender();

public Q_SLOTS:
  virtual void onModelsUpdated();
  virtual void onRender(float dt);
  virtual void onShowAllLinks();
  virtual void onHideAllLinks();
  virtual void onShowVisualAllLinks();
  virtual void onHideVisualAllLinks();
  virtual void onShowCollisionAllLinks();
  virtual void onHideCollisionAllLinks();
  virtual void onShowAxisAllLinks();
  virtual void onHideAxisAllLinks();
  virtual void onSelectAllLinks();
  virtual void onDeselectAllLinks();
  virtual void onEnable();

  void onACMSelectedLinks(const std::vector<std::string>& link_names);
  void onShowGroupsJointState(const std::unordered_map<std::string, double>& groups_joint_state);

private Q_SLOTS:
  void onSceneGraphModelItemChanged(QStandardItem* item);
  void onSceneStateModelItemChanged(QStandardItem* item);
  void updateVisibilityCheckedStates(const std::vector<std::string>& links);

protected:
  std::unique_ptr<Ui::EnvironmentWidget> ui;
  std::unique_ptr<EnvironmentWidgetImpl> data_;

  void updateModels();

  void createToolBar();
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_ENVIRONMENT_ENVIRONMENT_WIDGET_H
