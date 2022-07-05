/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include <tesseract_qt/joint_trajectory/joint_trajectory_plot_dialog.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_set_item.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_widget.h>
#include <tesseract_qt/joint_trajectory/joint_trajectory_model.h>

#include <tesseract_qt/plot/transforms/first_derivative.h>
#include <tesseract_qt/plot/transforms/integral_transform.h>
#include <tesseract_qt/plot/transforms/moving_average_filter.h>
#include <tesseract_qt/plot/transforms/moving_rms.h>
#include <tesseract_qt/plot/transforms/outlier_removal.h>
#include <tesseract_qt/plot/transforms/scale_transform.h>

#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_common/joint_state.h>
#include <tesseract_common/serialization.h>
#include <tesseract_visualization/trajectory_player.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/core/instruction.h>
#include <tesseract_command_language/utils/utils.h>
#include <QTimer>
#include <QToolBar>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <set>

const double SLIDER_RESOLUTION = 0.001;

namespace tesseract_gui
{
struct JointTrajectoryWidgetPrivate
{
  JointTrajectoryModel* model{ nullptr };
  tesseract_environment::Environment::ConstPtr default_env{ nullptr };
  std::unique_ptr<tesseract_visualization::TrajectoryPlayer> player;
  std::unique_ptr<QTimer> player_timer;
  std::unique_ptr<JointTrajectoryPlotDialog> plot_dialog;
  double current_duration{ 0 };
  tesseract_common::JointTrajectoryInfo current_trajectory;

  QString default_directory;

  // Store the selected item
  QStandardItem* selected_item;

  // Toolbar
  QToolBar* toolbar;
  QAction* open_action;
  QAction* save_action;
  QAction* remove_action;
  QAction* plot_action;
};

JointTrajectoryWidget::JointTrajectoryWidget(QWidget* parent, bool add_toolbar)
  : QWidget(parent)
  , ui_(std::make_unique<Ui::JointTrajectoryWidget>())
  , data_(std::make_unique<JointTrajectoryWidgetPrivate>())
{
  ui_->setupUi(this);

  if (add_toolbar)
  {
    createToolBar();
    ui_->verticalLayout->insertWidget(0, data_->toolbar);
  }

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
  connect(
      ui_->trajectoryTreeView, &QTreeView::expanded, [this]() { ui_->trajectoryTreeView->resizeColumnToContents(0); });

  TransformFactory::registerTransform<FirstDerivative>();
  TransformFactory::registerTransform<IntegralTransform>();
  TransformFactory::registerTransform<MovingAverageFilter>();
  TransformFactory::registerTransform<MovingRMS>();
  TransformFactory::registerTransform<OutlierRemovalFilter>();
  TransformFactory::registerTransform<ScaleTransform>();
}

JointTrajectoryWidget::~JointTrajectoryWidget()
{
  QSettings ms;
  ms.beginGroup("JointTrajectoryWidget");
  ms.setValue("default_directory", data_->default_directory);
  ms.endGroup();
}

void JointTrajectoryWidget::createToolBar()
{
  data_->toolbar = new QToolBar;  // NOLINT
  data_->open_action = data_->toolbar->addAction(icons::getImportIcon(), "Open", this, SLOT(onOpen()));
  data_->save_action = data_->toolbar->addAction(icons::getSaveIcon(), "Save", this, SLOT(onSave()));
  data_->remove_action = data_->toolbar->addAction(icons::getTrashIcon(), "Remove", this, SLOT(onRemove()));
  data_->toolbar->addSeparator();
  data_->plot_action = data_->toolbar->addAction(icons::getPlotIcon(), "Plot Joint Trajectory", this, SLOT(onPlot()));

  data_->save_action->setDisabled(true);
  data_->remove_action->setDisabled(true);
  data_->plot_action->setDisabled(true);
}

void JointTrajectoryWidget::setDefaultEnvironment(std::shared_ptr<const tesseract_environment::Environment> env)
{
  data_->default_env = std::move(env);
}

std::shared_ptr<const tesseract_environment::Environment> JointTrajectoryWidget::getDefaultEnvironment() const
{
  return data_->default_env;
}

void JointTrajectoryWidget::onOpen()
{
  QStringList filters;
  filters.append("Joint Trajectory Set XML (*.jtsx)");
  filters.append("Joint Trajectory Set Binary (*.jtsb)");
  filters.append("Composite Instruction XML (*.cpix)");
  filters.append("Composite Instruction Binary (*.cpib)");

  QStringList ext;
  ext.append("jtsx");
  ext.append("jtsb");
  ext.append("cpix");
  ext.append("cpib");

  QFileDialog dialog(this, "Open Joint Trajectory Set", data_->default_directory);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setNameFilters(filters);
  if (dialog.exec() == 1)
  {
    int idx = filters.indexOf(dialog.selectedNameFilter());
    data_->default_directory = QFileInfo(dialog.selectedFiles()[0]).absoluteDir().path();
    openJointTrajectorySet(dialog.selectedFiles()[0], ext[idx]);
  }
}

void JointTrajectoryWidget::onSave()
{
  if (data_->selected_item != nullptr &&
      data_->selected_item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET))
  {
    QList<QString> filters;
    filters.append("Joint Trajectory Set XML (*.jtsx)");
    filters.append("Joint Trajectory Set Binary (*.jtsb)");
    QFileDialog dialog(this, "Save Joint Trajectory Set", data_->default_directory);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilters(filters);
    if (dialog.exec() == 1)
    {
      data_->default_directory = QFileInfo(dialog.selectedFiles()[0]).absoluteDir().path();
      saveJointTrajectorySet(dialog.selectedFiles()[0], (dialog.selectedNameFilter() == filters[0]) ? "jtsx" : "jtsb");
    }
  }
}

bool JointTrajectoryWidget::saveJointTrajectorySet(QString filename, const QString& suffix)
{
  auto* item = dynamic_cast<JointTrajectorySetItem*>(data_->selected_item);
  QFileInfo file_info(filename);
  if (suffix == "jtsx")
  {
    if (file_info.suffix() != "jtsx")
      filename = file_info.absolutePath() + QDir::separator() + file_info.baseName() + ".jtsx";

    tesseract_common::Serialization::toArchiveFileXML<tesseract_common::JointTrajectorySet>(item->trajectory_set,
                                                                                            filename.toStdString());
    return true;
  }

  if (suffix == "jtsb")
  {
    if (file_info.suffix() != "jtsb")
      filename = file_info.absolutePath() + QDir::separator() + file_info.baseName() + ".jtsb";

    tesseract_common::Serialization::toArchiveFileBinary<tesseract_common::JointTrajectorySet>(item->trajectory_set,
                                                                                               filename.toStdString());
    return true;
  }

  return false;
}

bool JointTrajectoryWidget::openJointTrajectorySet(const QString& filename, const QString& suffix)
{
  QFileInfo file_info(filename);
  tesseract_common::JointTrajectorySet joint_trajectory_set;
  if (suffix == "jtsx" && file_info.suffix() == "jtsx")
  {
    addJointTrajectorySet(tesseract_common::Serialization::fromArchiveFileXML<tesseract_common::JointTrajectorySet>(
        filename.toStdString()));
    return true;
  }

  if (suffix == "jtsb" && file_info.suffix() == "jtsb")
  {
    addJointTrajectorySet(tesseract_common::Serialization::fromArchiveFileBinary<tesseract_common::JointTrajectorySet>(
        filename.toStdString()));
    return true;
  }

  if (suffix == "cpix" && file_info.suffix() == "cpix")
  {
    auto cpi =
        tesseract_common::Serialization::fromArchiveFileXML<tesseract_planning::Instruction>(filename.toStdString())
            .as<tesseract_planning::CompositeInstruction>();
    tesseract_common::JointTrajectory jt = tesseract_planning::toJointTrajectory(cpi);
    if (jt.empty())
      return false;

    std::unordered_map<std::string, double> initial_state;
    for (std::size_t i = 0; i < jt.states.front().joint_names.size(); ++i)
      initial_state[jt.states.front().joint_names[i]] = jt.states.front().position[i];

    tesseract_common::JointTrajectorySet jts(initial_state, jt.description);
    jts.appendJointTrajectory(jt);
    addJointTrajectorySet(jts);
    return true;
  }

  if (suffix == "cpib" && file_info.suffix() == "cpib")
  {
    auto cpi =
        tesseract_common::Serialization::fromArchiveFileBinary<tesseract_planning::Instruction>(filename.toStdString())
            .as<tesseract_planning::CompositeInstruction>();
    tesseract_common::JointTrajectory jt = tesseract_planning::toJointTrajectory(cpi);
    if (jt.empty())
      return false;

    std::unordered_map<std::string, double> initial_state;
    for (std::size_t i = 0; i < jt.states.front().joint_names.size(); ++i)
      initial_state[jt.states.front().joint_names[i]] = jt.states.front().position[i];

    tesseract_common::JointTrajectorySet jts(initial_state, jt.description);
    addJointTrajectorySet(jts);
    return true;
  }

  return false;
}

void JointTrajectoryWidget::onRemove()
{
  if (data_->selected_item != nullptr &&
      data_->selected_item->type() == static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET))
  {
    QString uuid = dynamic_cast<JointTrajectorySetItem*>(data_->selected_item)->uuid;
    removeJointTrajectorySet(uuid);
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

void JointTrajectoryWidget::setModel(JointTrajectoryModel* model)
{
  ui_->trajectoryTreeView->setModel(model);
  ui_->trajectoryTreeView->setColumnWidth(0, 175);
  data_->model = model;

  connect(ui_->trajectoryTreeView->selectionModel(),
          SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
          this,
          SLOT(onCurrentRowChanged(QModelIndex, QModelIndex)));
}

QString JointTrajectoryWidget::addJointTrajectorySet(tesseract_common::JointTrajectorySet trajectory_set)
{
  if (trajectory_set.getEnvironment() != nullptr)
    return data_->model->addJointTrajectorySet(trajectory_set);

  if (data_->default_env != nullptr)
    trajectory_set.applyEnvironment(data_->default_env->clone());

  return data_->model->addJointTrajectorySet(trajectory_set);
}

void JointTrajectoryWidget::removeJointTrajectorySet(const QString& key)
{
  data_->model->removeJointTrajectorySet(key);
  emit jointTrajectorySetRemoved(key);
}

bool JointTrajectoryWidget::hasJointTrajectorySet(const QString& key)
{
  return data_->model->hasJointTrajectorySet(key);
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
      data_->save_action->setDisabled(true);
      data_->remove_action->setDisabled(true);
      data_->plot_action->setDisabled(true);
      break;
    }
    case static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET_TRAJECTORY):
    {
      data_->save_action->setDisabled(true);
      data_->remove_action->setDisabled(true);
      data_->plot_action->setDisabled(false);

      data_->current_trajectory = data_->model->getJointTrajectory(current_index);

      auto details = data_->model->getJointTrajectorySetDetails(current_index);
      emit configureJointTrajectorySet(details.first, details.second);

      data_->player->setTrajectory(data_->current_trajectory.second);

      if (!data_->current_trajectory.second.empty())
        onEnablePlayer();

      break;
    }
    case static_cast<int>(StandardItemType::JOINT_TRAJECTORY_SET):
    {
      data_->save_action->setDisabled(false);
      data_->remove_action->setDisabled(false);
      data_->plot_action->setDisabled(false);

      auto details = data_->model->getJointTrajectorySetDetails(current_index);
      const tesseract_common::JointTrajectorySet& traj_set = details.second;

      emit configureJointTrajectorySet(details.first, details.second);

      data_->current_trajectory = tesseract_common::JointTrajectoryInfo();
      data_->current_trajectory.first = traj_set.getInitialState();
      for (const auto& t : traj_set.getJointTrajectories())
        data_->current_trajectory.second.insert(
            data_->current_trajectory.second.end(), t.second.begin(), t.second.end());

      data_->player->setTrajectory(data_->current_trajectory.second);

      if (!data_->current_trajectory.second.empty())
        onEnablePlayer();

      break;
    }
    default:
    {
      data_->save_action->setDisabled(true);
      data_->remove_action->setDisabled(true);
      data_->plot_action->setDisabled(true);

      const tesseract_common::JointState& state = data_->model->getJointState(current_index);
      auto details = data_->model->getJointTrajectorySetDetails(current_index);
      emit configureJointTrajectorySet(details.first, details.second);
      emit showJointState(state);
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
  emit showJointState(state);
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

void JointTrajectoryWidget::onEnable() {}

void JointTrajectoryWidget::onRender(float /*dt*/) {}

const QString& JointTrajectoryWidget::getDefaultDirectory() const { return data_->default_directory; }
void JointTrajectoryWidget::setDefaultDirectory(const QString& default_directory)
{
  data_->default_directory = default_directory;
}

}  // namespace tesseract_gui
