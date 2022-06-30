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
#ifndef TESSERACT_QT_MANIPULATION_MANIPULATION_WIDGET_H
#define TESSERACT_QT_MANIPULATION_MANIPULATION_WIDGET_H

#include <QWidget>
#include <memory>
#include <Eigen/Geometry>

namespace Ui
{
class ManipulationWidget;
}

namespace tesseract_environment
{
class Environment;
}

namespace tesseract_scene_graph
{
class SceneState;
}

namespace tesseract_kinematics
{
class KinematicGroup;
}

namespace tesseract_gui
{
enum class ManipulationStateType : int
{
  START,
  END
};

struct LinkVisibilityProperties;
struct ManipulationWidgetImpl;
class ManipulationWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ManipulationWidget(bool single_state = true, QWidget* parent = nullptr);
  ~ManipulationWidget();

  /**
   * @brief Check if widget is in a valid state
   * @return True if in a valid state, otherwise false
   */
  bool isValid() const;

  /**
   * @brief Get the number of state
   * @brief Currently only support 1 or 2 state
   * @return The number of state
   */
  int getStateCount() const;

  /**
   * @brief Set the environment
   * @param env The environment
   */
  void setEnvironment(std::shared_ptr<const tesseract_environment::Environment> env);

  /**
   * @brief Get the environment
   * @return The environment
   */
  std::shared_ptr<const tesseract_environment::Environment> getEnvironment() const;
  const tesseract_environment::Environment& environment() const;

  /**
   * @brief Get the kinematic group
   * @return The kinematic group
   */
  const tesseract_kinematics::KinematicGroup& kinematicGroup() const;

  /**
   * @brief Get the current mode
   * @details
   *   0 - Joint
   *   1 - Cartesian
   * @return The mode
   */
  int getMode() const;

  /**
   * @brief Get the group name
   * @return Then name of the group
   */
  QString getGroupName() const;

  /**
   * @brief Get the working frame
   * @return The working frame
   */
  QString getWorkingFrame() const;

  /**
   * @brief Get the TCP name
   * @return The TCP name
   */
  QString getTCPName() const;

  /**
   * @brief Get the TCP offset name
   * @return The TCP offset name
   */
  QString getTCPOffsetName() const;

  /**
   * @brief Get the tcp offset
   * @return The tcp offset
   */
  Eigen::Isometry3d getTCPOffset() const;

  /**
   * @brief Get the current state index
   * @return The active state index
   */
  int getActiveStateIndex() const;

  /**
   * @brief Get active state
   * @return The active state
   */
  tesseract_scene_graph::SceneState getActiveState() const;

  /**
   * @brief Get state for the given index
   * @param index The index to retrieve state under
   * @return The state
   */
  tesseract_scene_graph::SceneState getState(int index) const;

  /**
   * @brief Set the active state
   * @param state The state
   */
  void setActiveState(const std::unordered_map<std::string, double>& state);

  /**
   * @brief Set the active cartesian transform
   * @details It expects the transform to be in the work frame
   * @param transform The transform
   */
  void setActiveCartesianTransform(const Eigen::Isometry3d& transform);

  /**
   * @brief Get active cartesian transform base on the current working_frame, tcp_name and tcp_offset_name
   * @param in_world Indicate if the transform should be relative to world, otherwise it is relative to working frame
   * @return The active cartesian transform
   */
  Eigen::Isometry3d getActiveCartesianTransform(bool in_world = false) const;

  /**
   * @brief Get the joint names of the active state
   * @return The joint names
   */
  std::vector<std::string> getActiveJointNames() const;

  /**
   * @brief Get the joint values of the active state
   * @return The joint values in the order of what getActiveJointNames() returns
   */
  Eigen::VectorXd getActiveJointValues() const;

  /** @brief Get the link visibility properties */
  const std::unordered_map<std::string, LinkVisibilityProperties>& getLinkVisibilityProperties() const;
  std::unordered_map<std::string, LinkVisibilityProperties>& getLinkVisibilityProperties();

Q_SIGNALS:
  void environmentSet(const std::shared_ptr<const tesseract_environment::Environment>& env);
  void linkVisibilityChanged(const std::vector<std::string>& links);
  void manipulationStateChanged(const tesseract_scene_graph::SceneState& state, int state_index);
  void groupNameChanged(const QString& group_name);
  void modeChanged(int mode);
  void workingFrameChanged(const QString& working_frame_name);
  void tcpNameChanged(const QString& tcp_name);
  void tcpOffsetNameChanged(const QString& tcp_offset_name);

public Q_SLOTS:
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

private Q_SLOTS:
  void onGroupNameChanged();
  void onModeChanged();
  void onWorkingFrameChanged();
  void onTCPNameChanged();
  void onTCPOffsetNameChanged();
  void onManipulatorTypeChanged();
  void onJointStateSliderChanged(std::unordered_map<std::string, double> state);
  void onCartesianTransformChanged(const Eigen::Isometry3d& transform);
  void onReset();

private:
  std::unique_ptr<Ui::ManipulationWidget> ui;
  std::unique_ptr<ManipulationWidgetImpl> data_;

  void addToolBar();

  tesseract_scene_graph::SceneState getReducedSceneState(const tesseract_scene_graph::SceneState& scene_state);
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_MANIPULATION_MANIPULATION_WIDGET_H
