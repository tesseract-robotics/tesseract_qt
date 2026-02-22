/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2024 Levi Armstrong <levi.armstrong@gmail.com>
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

#include <tesseract_qt/planning/widgets/task_composer_widget.h>
#include "ui_task_composer_widget.h"

#include <tesseract_common/profile_dictionary.h>
#include <tesseract_common/resource_locator.h>
#include <tesseract_common/stopwatch.h>
#include <tesseract_common/utils.h>
#include <tesseract_environment/environment.h>
#include <tesseract_motion_planners/core/utils.h>
#include <tesseract_task_composer/core/task_composer_log.h>
#include <tesseract_task_composer/core/task_composer_server.h>
#include <tesseract_task_composer/core/task_composer_future.h>
#include <tesseract_task_composer/core/task_composer_context.h>
#include <tesseract_task_composer/core/task_composer_node.h>
#include <tesseract_task_composer/core/task_composer_node_info.h>
#include <tesseract_task_composer/core/task_composer_data_storage.h>
#include <tesseract_task_composer/core/task_composer_log.h>
#include <tesseract_task_composer/core/task_composer_graph.h>
#include <tesseract_command_language/utils.h>

#include <tesseract_common/cereal_serialization.h>
#include <tesseract_scene_graph/cereal_serialization.h>
#include <tesseract_environment/cereal_serialization.h>
#include <tesseract_command_language/cereal_serialization.h>
#include <tesseract_task_composer/core/cereal_serialization.h>
#include <tesseract_common/serialization.h>

#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/widgets/component_info_dialog.h>
#include <tesseract_qt/common/events/task_composer_events.h>
#include <tesseract_qt/common/events/tool_path_events.h>
#include <tesseract_qt/common/events/joint_trajectory_events.h>
#include <tesseract_qt/common/events/status_log_events.h>
#include <tesseract_qt/common/joint_trajectory_set.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/tool_path.h>
#include <tesseract_qt/common/utils.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>
#include <tesseract_qt/planning/models/task_composer_log_model.h>
#include <tesseract_qt/planning/register_poly_types.h>
#include <tesseract_qt/command_language/models/composite_instruction_standard_item.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <QMenu>

namespace tesseract::gui
{
struct TaskComposerWidget::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;
  tesseract::task_composer::TaskComposerServer task_composer_server;
  tesseract::common::GeneralResourceLocator resource_locator;
  tesseract::common::ProfileDictionary::Ptr profiles;
  tesseract::gui::TaskComposerLogModel log_model;

  ComponentInfoDialog environment_picker;
};

TaskComposerWidget::TaskComposerWidget(QWidget* parent) : TaskComposerWidget(nullptr, parent) {}
TaskComposerWidget::TaskComposerWidget(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::TaskComposerWidget>()), data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);

  ui->log_tree_view->setModel(&data_->log_model);
  ui->log_tree_view->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  ui->ns_combo_box->setModel(&data_->log_model);
  ui->ns_combo_box->setModelColumn(0);
  ui->log_combo_box->setModel(&data_->log_model);
  ui->log_combo_box->setModelColumn(0);

  setComponentInfo(std::move(component_info));

  connect(ui->task_run_push_button, SIGNAL(clicked(bool)), this, SLOT(onRun(bool)));
  connect(ui->log_tree_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onShowContextMenu(QPoint)));
  connect(ui->environment_push_button, SIGNAL(clicked(bool)), this, SLOT(onPickEnvironmentClicked(bool)));

  connect(ui->ns_combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&](int viewIndex) {
    // which column is being shown?
    int column = ui->ns_combo_box->modelColumn();
    // what’s the current root?
    QModelIndex root = ui->ns_combo_box->rootModelIndex();
    // build the model index for that row/column
    QModelIndex mi = data_->log_model.index(viewIndex, column, root);
    // now set the root of the log combo box
    ui->log_combo_box->setRootModelIndex(mi);
    ui->log_combo_box->setCurrentIndex(0);
  });

  // Register Types
  registerCommonAnyPolyTypes();
  registerCommonInstructionPolyTypes();

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

TaskComposerWidget::~TaskComposerWidget() = default;

void TaskComposerWidget::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  data_->component_info = std::move(component_info);
}

std::shared_ptr<const ComponentInfo> TaskComposerWidget::getComponentInfo() const { return data_->component_info; }

TaskComposerLogModel& TaskComposerWidget::getModel() { return data_->log_model; }
const TaskComposerLogModel& TaskComposerWidget::getModel() const { return data_->log_model; }

void TaskComposerWidget::createContextMenu(QMenu& log_menu)
{
  ComponentInfoManager::removeUnused();
  auto component_infos = ComponentInfoManager::get();

  QMenu* vtp_menu = log_menu.addMenu(icons::getToolPathIcon(), "View Tool Path");
  for (const auto& component_info : component_infos)
  {
    vtp_menu->addAction(
        icons::getModelIcon(), QString::fromStdString(component_info->getName()), [this, component_info]() {
          QModelIndex row = ui->log_tree_view->selectionModel()->currentIndex();
          if (row.isValid())
          {
            QStandardItem* item = static_cast<QStandardItemModel*>(ui->log_tree_view->model())->itemFromIndex(row);
            if (item->type() == static_cast<int>(StandardItemType::CL_COMPOSITE_INSTRUCTION))
            {
              const auto& ci = dynamic_cast<CompositeInstructionStandardItem*>(item)->getCompositeInstruction();
              const auto& log = data_->log_model.get(row);
              auto env_any = log.initial_data.getData("environment");
              auto env = env_any.as<std::shared_ptr<const tesseract::environment::Environment>>();
              // The toolpath is in world coordinate system
              tesseract::common::Toolpath toolpath = tesseract::motion_planners::toToolpath(ci, *env);
              events::ToolPathAdd event(component_info, toolpath);
              QApplication::sendEvent(qApp, &event);
            }
          }
        });
  }

  QMenu* vjt_menu = log_menu.addMenu(icons::getTrajectoryIcon(), "View Joint Trajectory");
  for (const auto& component_info : component_infos)
  {
    vjt_menu->addAction(
        icons::getModelIcon(), QString::fromStdString(component_info->getName()), [this, component_info]() {
          const QModelIndex row = ui->log_tree_view->selectionModel()->currentIndex();
          if (row.isValid())
          {
            QStandardItem* item = static_cast<TaskComposerLogModel*>(ui->log_tree_view->model())->itemFromIndex(row);
            if (item->type() == static_cast<int>(StandardItemType::CL_COMPOSITE_INSTRUCTION))
            {
              const auto& ci = dynamic_cast<CompositeInstructionStandardItem*>(item)->getCompositeInstruction();
              const auto& log = data_->log_model.get(row);
              auto env_any = log.initial_data.getData("environment");
              auto env = env_any.as<std::shared_ptr<const tesseract::environment::Environment>>();

              bool trajectory_sent{ false };
              if (!ci.empty() && ci.front().isCompositeInstruction())
              {
                tesseract::common::JointTrajectorySet jset(env->clone(), log.description);
                jset.setNamespace(ui->ns_line_edit->text().toStdString());
                for (const auto& sub_ci : ci)
                {
                  if (!sub_ci.isCompositeInstruction())
                    break;

                  jset.appendJointTrajectory(tesseract::command_language::toJointTrajectory(sub_ci));
                }
                events::JointTrajectoryAdd event(component_info, jset);
                QApplication::sendEvent(qApp, &event);
                trajectory_sent = true;
              }

              if (!trajectory_sent)
              {
                tesseract::common::JointTrajectorySet jset(env->clone(), log.description);
                jset.setNamespace(ui->ns_line_edit->text().toStdString());
                jset.appendJointTrajectory(tesseract::command_language::toJointTrajectory(ci));
                events::JointTrajectoryAdd event(component_info, jset);
                QApplication::sendEvent(qApp, &event);
              }
            }
          }
        });
  }
  log_menu.setWindowModality(Qt::ApplicationModal);
}

void TaskComposerWidget::viewDotgraph(const std::string& dotgraph)
{
  if (dotgraph.empty())
    return;

  const std::string dotgraph_location =
      tesseract::common::getTempPath() + tesseract::common::getTimestampString() + ".dot";
  {  // Save to temp under unique name
    std::ofstream os{};
    os.open(dotgraph_location);
    os << dotgraph;
    os.close();
  }

  // Open Dotgraph
  std::system(std::string("xdot " + dotgraph_location + " &").c_str());
}

void TaskComposerWidget::onShowContextMenu(const QPoint& pos)
{
  QModelIndex index = ui->log_tree_view->indexAt(pos);
  if (index.isValid())
  {
    QStandardItem* item = static_cast<QStandardItemModel*>(ui->log_tree_view->model())->itemFromIndex(index);

    if (item->type() == static_cast<int>(StandardItemType::CL_COMPOSITE_INSTRUCTION))
    {
      QPoint global_pos = ui->log_tree_view->viewport()->mapToGlobal(pos);
      QMenu menu;
      createContextMenu(menu);
      menu.exec(global_pos);
    }
  }
}

QModelIndex TaskComposerWidget::getSelectedLog() const
{
  // which column is being shown?
  int column = ui->log_combo_box->modelColumn();
  // what’s the current root?
  QModelIndex root = ui->log_combo_box->rootModelIndex();
  // build the model index for that row/column
  return data_->log_model.index(ui->log_combo_box->currentIndex(), column, root);
}

void TaskComposerWidget::onRun(bool /*checked*/)
{
  tesseract::task_composer::TaskComposerLog nlog(ui->desc_line_edit->text().toStdString());

  // Get selected log
  QModelIndex current_index = getSelectedLog();
  if (!current_index.isValid())
  {
    tesseract::gui::events::StatusLogError e("TaskComposerWidget, No log selected!");
    QApplication::sendEvent(qApp, &e);
    return;
  }

  const auto& log = data_->log_model.get(current_index);
  auto data_storage = std::make_shared<tesseract::task_composer::TaskComposerDataStorage>(log.initial_data);
  data_storage->setData("profiles", data_->profiles);

  // Load environment
  const QString current_environment_txt = ui->environment_line_edit->text();
  if (!current_environment_txt.isEmpty())
  {
    QStringList list = current_environment_txt.split("::");
    if (list.size() == 2)
    {
      auto uuid = boost::lexical_cast<boost::uuids::uuid>(list[1].toStdString());
      std::shared_ptr<ComponentInfo> component_info = ComponentInfoManager::get(uuid);
      if (component_info != nullptr)
      {
        auto env = EnvironmentManager::get(component_info);
        if (env != nullptr)
          data_storage->setData("environment",
                                std::shared_ptr<const tesseract::environment::Environment>(env->environment().clone()));
      }
    }
  }

  // Log initial data
  nlog.initial_data = *data_storage;

  const std::string task_name = ui->task_combo_box->currentText().toStdString();
  const std::string executor_name = ui->executor_combo_box->currentText().toStdString();
  const bool dotgraph = ui->dotgraph_check_box->isChecked();

  tesseract::common::Stopwatch stopwatch;
  stopwatch.start();
  auto future = data_->task_composer_server.run(task_name, data_storage, dotgraph, executor_name);
  future->wait();
  stopwatch.stop();

  // Check for failure if running pipeline or graph
  // This is useful if you are testing a sub task which should not abort in normal
  const tesseract::task_composer::TaskComposerNode& task = data_->task_composer_server.getTask(task_name);
  if (task.getType() == tesseract::task_composer::TaskComposerNodeType::GRAPH ||
      task.getType() == tesseract::task_composer::TaskComposerNodeType::PIPELINE)
  {
    const auto& graph_task = dynamic_cast<const tesseract::task_composer::TaskComposerGraph&>(task);
    std::vector<boost::uuids::uuid> terminals = graph_task.getTerminals();
    std::optional<tesseract::task_composer::TaskComposerNodeInfo> task_info =
        future->context->task_infos->getInfo(terminals.front());
    if (task_info.has_value())
      future->context->abort(terminals.front());
  }

  // Log Context
  nlog.context = future->context;

  // Generate dot graph if requested
  if (dotgraph)
    nlog.dotgraph = task.getDotgraph(nlog.context->task_infos->getInfoMap());

  // Send status
  const QString ps = (future->context->isSuccessful()) ? "Successful" : "Failed";
  const QString msg =
      QString("TaskComposerWidget, Planning %1, elapsed time %2 seconds").arg(ps).arg(stopwatch.elapsedSeconds());
  tesseract::gui::events::StatusLogInfo e(msg);
  QApplication::sendEvent(qApp, &e);

  // Add llog
  data_->log_model.add(nlog, ui->ns_line_edit->text().toStdString());

  // Show Dotgraph
  if (dotgraph)
    TaskComposerWidget::viewDotgraph(nlog.dotgraph);
}

void TaskComposerWidget::onPickEnvironmentClicked(bool /*checked*/)
{
  if (data_->environment_picker.exec() == 1)
  {
    auto component_info = data_->environment_picker.getComponentInfo();
    if (component_info != nullptr)
    {
      const QString text = QString("%1::%2").arg(component_info->getName().c_str(),
                                                 boost::uuids::to_string(component_info->getNamespace()).c_str());
      ui->environment_line_edit->setText(text);
      return;
    }
  }
}

bool TaskComposerWidget::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::EventType::TASK_COMPOSER_LOAD_CONFIG)
  {
    assert(dynamic_cast<events::TaskComposerLoadConfig*>(event) != nullptr);
    auto* e = static_cast<events::TaskComposerLoadConfig*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      auto resource = data_->resource_locator.locateResource(e->getResourcePath());
      if (resource != nullptr)
      {
        data_->task_composer_server.loadConfig(std::filesystem::path(resource->getFilePath()), *resource);

        ui->executor_combo_box->clear();
        for (const auto& executor : data_->task_composer_server.getAvailableExecutors())
          ui->executor_combo_box->addItem(QString::fromStdString(executor));

        ui->task_combo_box->clear();
        for (const auto& task : data_->task_composer_server.getAvailableTasks())
          ui->task_combo_box->addItem(QString::fromStdString(task));
      }
    }
  }

  if (event->type() == events::EventType::TASK_COMPOSER_LOAD_LOG)
  {
    assert(dynamic_cast<events::TaskComposerLoadLog*>(event) != nullptr);
    auto* e = static_cast<events::TaskComposerLoadLog*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      auto resource = data_->resource_locator.locateResource(e->getResourcePath());
      if (resource != nullptr)
      {
        auto log = tesseract::common::Serialization::fromArchiveFile<tesseract::task_composer::TaskComposerLog>(
            resource->getFilePath());
        if (e->getNamespace().empty())
          data_->log_model.add(std::move(log));
        else
          data_->log_model.add(std::move(log), e->getNamespace());
      }
    }
  }

  if (event->type() == events::EventType::TASK_COMPOSER_ADD_LOG)
  {
    assert(dynamic_cast<events::TaskComposerAddLog*>(event) != nullptr);
    auto* e = static_cast<events::TaskComposerAddLog*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      if (e->getNamespace().empty())
        data_->log_model.add(e->getLog());
      else
        data_->log_model.add(e->getLog(), e->getNamespace());
    }
  }

  if (event->type() == events::EventType::TASK_COMPOSER_SAVE_LOG)
  {
    assert(dynamic_cast<events::TaskComposerSaveLog*>(event) != nullptr);
    auto* e = static_cast<events::TaskComposerSaveLog*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      QModelIndex current_index = getSelectedLog();
      if (current_index.isValid())
      {
        const auto& log = data_->log_model.get(current_index);
        tesseract::common::Serialization::toArchiveFile<tesseract::task_composer::TaskComposerLog>(log,
                                                                                                   e->getSavePath());
      }
    }
  }

  if (event->type() == events::EventType::TASK_COMPOSER_PLOT_DOTGRAPH)
  {
    assert(dynamic_cast<events::TaskComposerPlotDotgraph*>(event) != nullptr);
    auto* e = static_cast<events::TaskComposerPlotDotgraph*>(event);
    if (e->getComponentInfo() == data_->component_info)
    {
      QModelIndex current_index = getSelectedLog();
      if (current_index.isValid())
      {
        const auto& log = data_->log_model.get(current_index);
        if (!log.dotgraph.empty())
        {
          TaskComposerWidget::viewDotgraph(log.dotgraph);
        }
        else if (log.context != nullptr && data_->task_composer_server.hasTask(log.context->name))
        {
          const auto& task = data_->task_composer_server.getTask(log.context->name);
          const std::string dotgraph = task.getDotgraph(log.context->task_infos->getInfoMap());
          TaskComposerWidget::viewDotgraph(dotgraph);
        }
        else
        {
          tesseract::gui::events::StatusLogWarn e("TaskComposerWidget, No dotgraph found!");
          QApplication::sendEvent(qApp, &e);
        }
      }
    }
  }

  if (event->type() == events::EventType::TASK_COMPOSER_SET_PROFILES)
  {
    assert(dynamic_cast<events::TaskComposerSetProfiles*>(event) != nullptr);
    auto* e = static_cast<events::TaskComposerSetProfiles*>(event);
    if (e->getComponentInfo() == data_->component_info)
      data_->profiles = e->getProfiles();
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}
}  // namespace tesseract::gui
