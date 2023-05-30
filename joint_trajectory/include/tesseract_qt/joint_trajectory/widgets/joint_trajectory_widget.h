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
class ComponentInfo;
class JointTrajectoryModel;
class JointTrajectoryPlotDialog;

class JointTrajectoryWidget : public QWidget
{
  Q_OBJECT

public:
  explicit JointTrajectoryWidget(QWidget* parent = nullptr);
  explicit JointTrajectoryWidget(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent = nullptr);
  ~JointTrajectoryWidget();

  void setComponentInfo(std::shared_ptr<const ComponentInfo> component_info);
  std::shared_ptr<const ComponentInfo> getComponentInfo() const;

  void setModel(std::shared_ptr<JointTrajectoryModel> model);
  std::shared_ptr<JointTrajectoryModel> getModel();
  std::shared_ptr<const JointTrajectoryModel> getModel() const;

  QItemSelectionModel& getSelectionModel();
  const QItemSelectionModel& getSelectionModel() const;

  const QString& getDefaultDirectory() const;
  void setDefaultDirectory(const QString& default_directory);

private Q_SLOTS:
  void onSaveFinished(int results);
  void onOpenFinished(int results);
  void onRemove();
  void onPlot();
  void onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous);
  void onPauseButtonClicked();
  void onPlayButtonClicked();
  void onPlayerTimerTimeout();
  void onSliderValueChanged(int value);
  void onEnablePlayer();
  void onDisablePlayer();

protected:
  struct Implementation;
  std::unique_ptr<Ui::JointTrajectoryWidget> ui_;
  std::unique_ptr<Implementation> data_;

  // Documentation inherited
  bool eventFilter(QObject* obj, QEvent* event) override;
};

}  // namespace tesseract_gui

#endif  // TESSERACT_QT_JOINT_TRAJECTORY_JOINT_TRAJECTORY_WIDGET_H
