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
#include "ui_joint_trajectory_widget.h"
#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_plot_dialog.h>
#include <tesseract_qt/joint_trajectory/widgets/joint_trajectory_widget.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_set_item.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_model.h>
#include <tesseract_qt/joint_trajectory/models/joint_trajectory_utils.h>

#include <tesseract_qt/plot/transforms/first_derivative.h>
#include <tesseract_qt/plot/transforms/integral_transform.h>
#include <tesseract_qt/plot/transforms/moving_average_filter.h>
#include <tesseract_qt/plot/transforms/moving_rms.h>
#include <tesseract_qt/plot/transforms/outlier_removal.h>
#include <tesseract_qt/plot/transforms/scale_transform.h>

#include <tesseract_qt/common/events/joint_trajectory_events.h>
#include <tesseract_qt/common/events/scene_graph_events.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/joint_trajectory_set.h>

#include <tesseract_common/joint_state.h>
#include <tesseract_visualization/trajectory_player.h>

#include <QTimer>
#include <QFileDialog>
#include <set>

const double SLIDER_RESOLUTION = 0.001;

namespace tesseract_gui
{
struct JointTrajectoryWidget::Implementation
{
  std::shared_ptr<JointTrajectoryModel> model;
  std::unique_ptr<tesseract_visualization::TrajectoryPlayer> player;
  std::unique_ptr<QTimer> player_timer;
  std::unique_ptr<JointTrajectoryPlotDialog> plot_dialog;

  QString default_directory;

  std::unique_ptr<QFileDialog> open_dialog;
  QStringList open_dialog_filters;
  QStringList open_dialog_ext;

  std::unique_ptr<QFileDialog> save_dialog;
  QStringList save_dialog_filters;

  double current_duration{ 0 };
  tesseract_common::JointTrajectoryInfo current_trajectory;
  tesseract_environment::Environment::Ptr current_environment;

  // Store the selected item
  QStandardItem* selected_item{ nullptr };
};

JointTrajectoryWidget::JointTrajectoryWidget(QWidget* parent) : JointTrajectoryWidget(ComponentInfo(), parent) {}

JointTrajectoryWidget::JointTrajectoryWidget(ComponentInfo component_info, QWidget* parent)
  : QWidget(parent), ui_(std::make_unique<Ui::JointTrajectoryWidget>()), data_(std::make_unique<Implementation>())
{
  ui_->setupUi(this);

  // Set component information
  setComponentInfo(component_info);

  QSettings ms;
  ms.beginGroup("JointTrajectoryWidget");
  data_->default_directory =
      ms.value("default_directory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]).toString();
  ms.endGroup();

  data_->player = std::make_unique<tesseract_visualization::TrajectoryPlayer>();
  data_->player_timer = std::make_unique<QTimer>(this);
  data_->player_timer->start(10);

  connect(ui_->trajectoryPlayButton, SIGNAL(clicked()), this, SLOT(onPlayButtonClicked()));
  connect(ui_->trajectoryPauseButton, SIGNAL(clicked()), this, SLOT(onPauseButtonClicked()));
  connect(ui_->trajectorySlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
  connect(data_->player_timer.get(), SIGNAL(timeout()), this, SLOT(onPlayerTimerTimeout()));

  TransformFactory::registerTransform<FirstDerivative>();
  TransformFactory::registerTransform<IntegralTransform>();
  TransformFactory::registerTransform<MovingAverageFilter>();
  TransformFactory::registerTransform<MovingRMS>();
  TransformFactory::registerTransform<OutlierRemovalFilter>();
  TransformFactory::registerTransform<ScaleTransform>();

  {  // Setup open dialog
    data_->open_dialog_filters.append("Joint Trajectory Set XML (*.jtsx)");
    data_->open_dialog_filters.append("Joint Trajectory Set Binary (*.jtsb)");
    data_->open_dialog_filters.append("Composite Instruction XML (*.cpix)");
    data_->open_dialog_filters.append("Composite Instruction Binary (*.cpib)");

    data_->open_dialog_ext.append("jtsx");
    data_->open_dialog_ext.append("jtsb");
    data_->open_dialog_ext.append("cpix");
    data_->open_dialog_ext.append("cpib");

    data_->open_dialog = std::make_unique<QFileDialog>(nullptr, "Open Joint Trajectory Set", data_->default_directory);
    data_->open_dialog->setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
    data_->open_dialog->setModal(true);
    data_->open_dialog->setAcceptMode(QFileDialog::AcceptOpen);
    data_->open_dialog->setNameFilters(data_->open_dialog_filters);

    connect(data_->open_dialog.get(), SIGNAL(finished(int)), this, SLOT(onOpenFinished(int)));
  }

  {  // Setup save dialog
    data_->save_dialog_filters.append("Joint Trajectory Set XML (*.jtsx)");
    data_->save_dialog_filters.append("Joint Trajectory Set Binary (*.jtsb)");

    data_->save_dialog = std::make_unique<QFileDialog>(nullptr, "Save Joint Trajectory Set", data_->default_directory);
    data_->save_dialog->setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
    data_->save_dialog->setModal(true);
    data_->save_dialog->setAcceptMode(QFileDialog::AcceptSave);
    data_->save_dialog->setNameFilters(data_->save_dialog_filters);

    connect(data_->save_dialog.get(), SIGNAL(finished(int)), this, SLOT(onSaveFinished(int)));
  }

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

JointTrajectoryWidget::~JointTrajectoryWidget()
{
  QSettings ms;
  ms.beginGroup("JointTrajectoryWidget");
  ms.setValue("default_directory", data_->default_directory);
  ms.endGroup();
}

void JointTrajectoryWidget::setComponentInfo(ComponentInfo component_info)
{
  // Create model
  data_->model = std::make_shared<JointTrajectoryModel>(std::move(component_info));
  ui_->trajectoryTreeView->setModel(data_->model.get());

  connect(ui_->trajectoryTreeView->selectionModel(),
          SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
          this,
          SLOT(onCurrentRowChanged(QModelIndex, QModelIndex)));
}

const ComponentInfo& JointTrajectoryWidget::getComponentInfo() const { return data_->model->getComponentInfo(); }

void JointTrajectoryWidget::setModel(std::shared_ptr<JointTrajectoryModel> model)
{
  data_->model = std::move(model);
  ui_->trajectoryTreeView->setModel(data_->model.get());

  connect(ui_->trajectoryTreeView->selectionModel(),
          SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
          this,
          SLOT(onCurrentRowChanged(QModelIndex, QModelIndex)));
}
std::shared_ptr<JointTrajectoryModel> JointTrajectoryWidget::getModel() { return data_->model; }
std::shared_ptr<const JointTrajectoryModel> JointTrajectoryWidget::getModel() const { return data_->model; }

QItemSelectionModel& JointTrajectoryWidget::getSelectionModel() { return *ui_->trajectoryTreeView->selectionModel(); }
const QItemSelectionModel& JointTrajectoryWidget::getSelectionModel() const
{
  return *ui_->trajectoryTreeView->selectionModel();
}

const QString& JointTrajectoryWidget::getDefaultDirectory() const { return data_->default_directory; }
void JointTrajectoryWidget::setDefaultDirectory(const QString& default_directory)
{
  data_->default_directory = default_directory;
}

void JointTrajectoryWidget::onSaveFinished(int results)
{
  if (results == 1)
  {
    tesseract_common::JointTrajectorySet jts =
        dynamic_cast<JointTrajectorySetItem*>(data_->selected_item)->trajectory_set;
    data_->default_directory = QFileInfo(data_->save_dialog->selectedFiles()[0]).absoluteDir().path();
    saveJointTrajectorySet(jts,
                           data_->save_dialog->selectedFiles()[0],
                           (data_->save_dialog->selectedNameFilter() == data_->save_dialog_filters[0]) ? "jtsx" :
                                                                                                         "jtsb");
  }

  setEnabled(true);
}

void JointTrajectoryWidget::onOpenFinished(int results)
{
  if (results == 1)
  {
    int idx = data_->open_dialog_filters.indexOf(data_->open_dialog->selectedNameFilter());
    data_->default_directory = QFileInfo(data_->open_dialog->selectedFiles()[0]).absoluteDir().path();
    openJointTrajectorySet(
        data_->model->getComponentInfo(), data_->open_dialog->selectedFiles()[0], data_->open_dialog_ext[idx]);
  }
}

void JointTrajectoryWidget::onRemove()
{
  if (data_->selected_item != nullptr &&
      data_->selected_item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET))
  {
    boost::uuids::uuid uuid = dynamic_cast<JointTrajectorySetItem*>(data_->selected_item)->trajectory_set.getUUID();
    QApplication::sendEvent(qApp, new events::JointTrajectoryRemove(data_->model->getComponentInfo(), uuid));
    data_->selected_item = nullptr;
    onDisablePlayer();
  }
}

void JointTrajectoryWidget::onPlot()
{
  if (data_->current_trajectory.second.empty())
    return;

  data_->plot_dialog = nullptr;
  data_->plot_dialog = std::make_unique<JointTrajectoryPlotDialog>(data_->current_trajectory);
  data_->plot_dialog->setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
  data_->plot_dialog->show();
}

void JointTrajectoryWidget::onCurrentRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
  QModelIndex current_index = current;  // This appears to be changing so copy
  data_->selected_item = data_->model->itemFromIndex(current_index);
  onDisablePlayer();
  switch (data_->selected_item->type())
  {
    case static_cast<int>(StandardItemType::COMMON_NAMESPACE):
    {
      auto* event = new events::JointTrajectoryToolbarState(data_->model->getComponentInfo());
      event->save_enabled = false;
      event->remove_enabled = false;
      event->plot_enabled = false;
      QApplication::sendEvent(qApp, event);

      break;
    }
    case static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET_TRAJECTORY):
    {
      auto* event = new events::JointTrajectoryToolbarState(data_->model->getComponentInfo());
      event->save_enabled = false;
      event->remove_enabled = false;
      event->plot_enabled = true;
      QApplication::sendEvent(qApp, event);

      data_->current_trajectory = data_->model->getJointTrajectory(current_index);

      auto jts = data_->model->getJointTrajectorySet(current_index);

      if (jts.getEnvironment() != nullptr && data_->current_environment != jts.getEnvironment() &&
          jts.getEnvironment()->isInitialized())
      {
        data_->current_environment = jts.getEnvironment();

        // If no parent then it using the top most so no need to overwrite existing environment
        if (data_->model->getComponentInfo().hasParent())
        {
          auto env_wrapper =
              std::make_shared<DefaultEnvironmentWrapper>(data_->model->getComponentInfo(), jts.getEnvironment());
          EnvironmentManager::set(env_wrapper);
        }
      }

      data_->player->setTrajectory(data_->current_trajectory.second);

      if (!data_->current_trajectory.second.empty())
        onEnablePlayer();

      break;
    }
    case static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET):
    {
      auto* event = new events::JointTrajectoryToolbarState(data_->model->getComponentInfo());
      event->save_enabled = true;
      event->remove_enabled = true;
      event->plot_enabled = true;
      QApplication::sendEvent(qApp, event);

      auto jts = data_->model->getJointTrajectorySet(current_index);

      if (jts.getEnvironment() != nullptr && data_->current_environment != jts.getEnvironment() &&
          jts.getEnvironment()->isInitialized())
      {
        data_->current_environment = jts.getEnvironment();

        // If no parent then it using the top most so no need to overwrite existing environment
        if (data_->model->getComponentInfo().hasParent())
        {
          auto env_wrapper =
              std::make_shared<DefaultEnvironmentWrapper>(data_->model->getComponentInfo(), jts.getEnvironment());
          EnvironmentManager::set(env_wrapper);
        }
      }

      data_->current_trajectory = tesseract_common::JointTrajectoryInfo();
      data_->current_trajectory.first = jts.getInitialState();
      for (const auto& t : jts.getJointTrajectories())
        data_->current_trajectory.second.insert(
            data_->current_trajectory.second.end(), t.second.begin(), t.second.end());

      data_->player->setTrajectory(data_->current_trajectory.second);

      if (!data_->current_trajectory.second.empty())
        onEnablePlayer();

      break;
    }
    default:
    {
      auto* event = new events::JointTrajectoryToolbarState(data_->model->getComponentInfo());
      event->save_enabled = false;
      event->remove_enabled = false;
      event->plot_enabled = false;
      QApplication::sendEvent(qApp, event);

      const tesseract_common::JointState& state = data_->model->getJointState(current_index);
      auto jts = data_->model->getJointTrajectorySet(current_index);

      if (jts.getEnvironment() != nullptr && jts.getEnvironment()->isInitialized())
      {
        if (data_->model->getComponentInfo().hasParent() && data_->current_environment != jts.getEnvironment())
        {
          auto env_wrapper =
              std::make_shared<DefaultEnvironmentWrapper>(data_->model->getComponentInfo(), jts.getEnvironment());
          EnvironmentManager::set(env_wrapper);
        }

        data_->current_environment = jts.getEnvironment();
        data_->current_environment->setState(state.joint_names, state.position);
      }

      break;
    }
  }
}

void JointTrajectoryWidget::onPauseButtonClicked()
{
  ui_->trajectoryPlayButton->setEnabled(true);
  ui_->trajectorySlider->setEnabled(true);
  ui_->trajectoryPauseButton->setEnabled(false);
}
void JointTrajectoryWidget::onPlayButtonClicked()
{
  ui_->trajectoryPlayButton->setEnabled(false);
  ui_->trajectorySlider->setEnabled(false);
  ui_->trajectoryPauseButton->setEnabled(true);
  data_->player->setCurrentDuration(data_->current_duration);
}

void JointTrajectoryWidget::onPlayerTimerTimeout()
{
  if (ui_->trajectoryPlayerFrame->isEnabled() && ui_->trajectoryPauseButton->isEnabled())
  {
    data_->player->getNext();
    ui_->trajectorySlider->setSliderPosition(data_->player->currentDuration() / SLIDER_RESOLUTION);
    if (data_->player->isFinished())
      onPauseButtonClicked();
  }
}

void JointTrajectoryWidget::onSliderValueChanged(int value)
{
  data_->current_duration = value * SLIDER_RESOLUTION;
  tesseract_common::JointState state = data_->player->setCurrentDuration(data_->current_duration);
  ui_->trajectoryCurrentDurationLabel->setText(QString().sprintf("%0.3f", data_->current_duration));
  data_->current_environment->setState(state.joint_names, state.position);
}

void JointTrajectoryWidget::onEnablePlayer()
{
  ui_->trajectoryPlayerFrame->setEnabled(true);
  ui_->trajectoryPlayButton->setEnabled(true);
  ui_->trajectoryPauseButton->setEnabled(false);
  ui_->trajectorySlider->setMinimum(0);
  ui_->trajectorySlider->setMaximum(data_->player->trajectoryDuration() / SLIDER_RESOLUTION);
  ui_->trajectorySlider->setSliderPosition(0);
  ui_->trajectoryCurrentDurationLabel->setText(QString().sprintf("%0.3f", data_->player->currentDuration()));
  ui_->trajectoryDurationLabel->setText(QString().sprintf("%0.3f", data_->player->trajectoryDuration()));
  data_->current_duration = 0;
}

void JointTrajectoryWidget::onDisablePlayer() { ui_->trajectoryPlayerFrame->setEnabled(false); }

// Documentation inherited
bool JointTrajectoryWidget::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::JointTrajectoryOpen::kType)
  {
    assert(dynamic_cast<events::JointTrajectoryOpen*>(event) != nullptr);
    auto* e = static_cast<events::JointTrajectoryOpen*>(event);
    if (e->getComponentInfo() == data_->model->getComponentInfo())
    {
      data_->open_dialog->show();
      data_->open_dialog->raise();
      data_->open_dialog->activateWindow();
    }
  }
  else if (event->type() == events::JointTrajectorySave::kType)
  {
    assert(dynamic_cast<events::JointTrajectorySave*>(event) != nullptr);
    auto* e = static_cast<events::JointTrajectorySave*>(event);
    if (e->getComponentInfo() == data_->model->getComponentInfo())
    {
      if (data_->selected_item != nullptr &&
          data_->selected_item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET))
      {
        setDisabled(true);
        data_->save_dialog->show();
        data_->save_dialog->raise();
        data_->save_dialog->activateWindow();
      }
    }
  }
  else if (event->type() == events::JointTrajectoryPlot::kType)
  {
    assert(dynamic_cast<events::JointTrajectoryPlot*>(event) != nullptr);
    auto* e = static_cast<events::JointTrajectoryPlot*>(event);
    if (e->getComponentInfo() == data_->model->getComponentInfo())
      onPlot();
  }
  else if (event->type() == events::JointTrajectoryRemoveSelected::kType)
  {
    assert(dynamic_cast<events::JointTrajectoryRemoveSelected*>(event) != nullptr);
    auto* e = static_cast<events::JointTrajectoryRemoveSelected*>(event);
    if (e->getComponentInfo() == data_->model->getComponentInfo())
      onRemove();
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

}  // namespace tesseract_gui
