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
#ifndef TESSERACT_QT_JOINT_TRAJECTORY_JOINT_TRAJECTORY_WIDGET_H
#define TESSERACT_QT_JOINT_TRAJECTORY_JOINT_TRAJECTORY_WIDGET_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#ifndef Q_MOC_RUN
#include <memory>
#endif
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <QWidget>
#include <QItemSelectionModel>

namespace Ui
{
class JointTrajectoryWidget;
}

namespace tesseract_visualization
{
class TrajectoryPlayer;
}

namespace tesseract_common
{
class JointState;
class JointTrajectorySet;
}  // namespace tesseract_common

namespace tesseract_environment
{
class Environment;
class Command;
}  // namespace tesseract_environment

namespace tesseract_gui
{
class JointTrajectoryModel;
class JointTrajectoryPlotDialog;

struct JointTrajectoryWidgetPrivate;
class JointTrajectoryWidget : public QWidget
{
  Q_OBJECT

public:
  explicit JointTrajectoryWidget(QWidget* parent = nullptr, bool add_toolbar = true);
  ~JointTrajectoryWidget();

  void setModel(JointTrajectoryModel* model);

  /**
   * @brief Set the default environment to use if environment was not provided with the trajectory set
   * @param env The default environment to use for trajectory set not containing an environment
   */
  void setDefaultEnvironment(std::shared_ptr<const tesseract_environment::Environment> env);

  /**
   * @brief Get the default environment
   * @return The default environment
   */
  std::shared_ptr<const tesseract_environment::Environment> getDefaultEnvironment() const;

  /**
   * @brief Add joint trajectory set
   * @param trajectory_set The trajectory set associated with the key
   * @return The key associated with added trajectory for removal
   */
  QString addJointTrajectorySet(tesseract_common::JointTrajectorySet trajectory_set);

  /**
   * @brief Remove the joint trajectory set
   * @param key The key associated with the joint trajectory set to be removed
   */
  void removeJointTrajectorySet(const QString& key);

  /**
   * @brief Check a trajectory set with the provided key exists
   * @param key The key associated with the joint trajectory set to find
   * @return True if a trajectory exists under the provided key, otherwise false
   */
  bool hasJointTrajectorySet(const QString& key);

Q_SIGNALS:
  void showJointState(const tesseract_common::JointState& state);
  void configureJointTrajectorySet(const QString& uuid,
                                   const tesseract_common::JointTrajectorySet& joint_trajectory_set);
  void jointTrajectorySetRemoved(const QString& uuid);

public Q_SLOTS:
  virtual void onOpen();
  virtual void onSave();
  virtual void onRemove();
  virtual void onPlot();
  virtual void onEnable();
  virtual void onRender(float dt);

private Q_SLOTS:
  void onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous);
  void onPauseButtonClicked();
  void onPlayButtonClicked();
  void onPlayerTimerTimeout();
  void onSliderValueChanged(int value);
  void onEnablePlayer();
  void onDisablePlayer();

protected:
  std::unique_ptr<Ui::JointTrajectoryWidget> ui_;
  std::unique_ptr<JointTrajectoryWidgetPrivate> data_;
  void createToolBar();

  const QString& getDefaultDirectory() const;
  void setDefaultDirectory(const QString& default_directory);

  bool saveJointTrajectorySet(QString filename, const QString& suffix);
  bool openJointTrajectorySet(const QString& filename, const QString& suffix);
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_JOINT_TRAJECTORY_JOINT_TRAJECTORY_WIDGET_H
