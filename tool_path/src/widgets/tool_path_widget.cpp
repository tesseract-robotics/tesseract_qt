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

#include <tesseract_qt/tool_path/widgets/tool_path_widget.h>
#include <tesseract_qt/tool_path/widgets/tool_path_file_dialog.h>
#include <tesseract_qt/tool_path/models/tool_path_model.h>
#include <tesseract_qt/tool_path/models/tool_path_selection_model.h>
#include <tesseract_qt/tool_path/models/tool_path_utils.h>
#include <tesseract_qt/common/models/tool_path_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/widgets/tree_view.h>
#include <tesseract_qt/common/events/tool_path_events.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_scene_graph/scene_state.h>
#include <tesseract_environment/environment.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QStandardItemModel>

namespace tesseract_gui
{
struct ToolPathWidget::Implementation
{
  std::shared_ptr<ToolPathModel> model;
  std::shared_ptr<ToolPathSelectionModel> selection_model;
  QVBoxLayout* layout;
  TreeView* tree_view;

  std::unique_ptr<ToolPathFileDialog> open_dialog;
  std::unique_ptr<ToolPathFileDialog> save_dialog;

  // Store the selected item
  QStandardItem* selected_item{ nullptr };
};

ToolPathWidget::ToolPathWidget(QWidget* parent) : ToolPathWidget(nullptr, parent) {}

ToolPathWidget::ToolPathWidget(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent)
  : QWidget(parent), data_(std::make_unique<Implementation>())
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // Create model
  data_->model = std::make_shared<ToolPathModel>(component_info);
  data_->selection_model = std::make_shared<ToolPathSelectionModel>(data_->model.get(), component_info);

  // Create tree widget
  data_->tree_view = new TreeView();
  data_->tree_view->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
  data_->tree_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  data_->tree_view->setModel(data_->model.get());

  // Create layout
  data_->layout = new QVBoxLayout();
  data_->layout->setMargin(0);
  data_->layout->setSpacing(0);
  data_->layout->addWidget(data_->tree_view);

  // Set layout
  setLayout(data_->layout);

  // Setup open dialog
  data_->open_dialog = std::make_unique<ToolPathFileDialog>(component_info, QFileDialog::AcceptOpen);
  data_->open_dialog->setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  data_->open_dialog->setModal(true);
  data_->open_dialog->setAcceptMode(QFileDialog::AcceptOpen);
  connect(data_->open_dialog.get(), SIGNAL(finished(int)), this, SLOT(onOpenFinished(int)));

  // Setup save dialog
  data_->save_dialog = std::make_unique<ToolPathFileDialog>(component_info, QFileDialog::AcceptSave);
  data_->save_dialog->setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  data_->save_dialog->setModal(true);
  connect(data_->save_dialog.get(), SIGNAL(finished(int)), this, SLOT(onSaveFinished(int)));

  connect(data_->tree_view->selectionModel(),
          SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
          this,
          SLOT(onCurrentRowChanged(QModelIndex, QModelIndex)));

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}

ToolPathWidget::~ToolPathWidget() = default;

void ToolPathWidget::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  // Create model
  data_->model = std::make_shared<ToolPathModel>(component_info);
  data_->selection_model = std::make_shared<ToolPathSelectionModel>(data_->model.get(), component_info);

  data_->tree_view->setModel(data_->model.get());
  data_->tree_view->setSelectionModel(data_->selection_model.get());

  data_->open_dialog->setComponentInfo(component_info);
  data_->save_dialog->setComponentInfo(component_info);
}

std::shared_ptr<const ComponentInfo> ToolPathWidget::getComponentInfo() const
{
  return data_->model->getComponentInfo();
}

void ToolPathWidget::setModel(std::shared_ptr<ToolPathModel> model)
{
  data_->model = std::move(model);
  data_->tree_view->setModel(data_->model.get());
}
std::shared_ptr<ToolPathModel> ToolPathWidget::getModel() { return data_->model; }
std::shared_ptr<const ToolPathModel> ToolPathWidget::getModel() const { return data_->model; }

QItemSelectionModel& ToolPathWidget::getSelectionModel() { return *data_->tree_view->selectionModel(); }
const QItemSelectionModel& ToolPathWidget::getSelectionModel() const { return *data_->tree_view->selectionModel(); }

// Documentation inherited
bool ToolPathWidget::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == events::EventType::TOOL_PATH_OPEN)
  {
    assert(dynamic_cast<events::ToolPathOpen*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathOpen*>(event);
    if (e->getComponentInfo() == data_->model->getComponentInfo())
    {
      data_->open_dialog->show();
      data_->open_dialog->raise();
      data_->open_dialog->activateWindow();
    }
  }
  else if (event->type() == events::EventType::TOOL_PATH_SAVE)
  {
    assert(dynamic_cast<events::ToolPathSave*>(event) != nullptr);
    auto* e = static_cast<events::ToolPathSave*>(event);
    if (e->getComponentInfo() == data_->model->getComponentInfo())
    {
      if (data_->selected_item != nullptr &&
          data_->selected_item->type() == static_cast<int>(StandardItemType::COMMON_TOOL_PATH))
      {
        setDisabled(true);
        data_->save_dialog->show();
        data_->save_dialog->raise();
        data_->save_dialog->activateWindow();
      }
    }
  }

  // Standard event processing
  return QObject::eventFilter(obj, event);
}

void ToolPathWidget::onSaveFinished(int results)
{
  if (results == 1)
  {
    auto* item = dynamic_cast<ToolPathStandardItem*>(data_->selected_item);
    const std::string& working_frame = item->getWorkingFrame();
    std::string link_name = data_->save_dialog->getLinkName().toStdString();
    tesseract_common::Toolpath tp = item->getCommonToolPath();
    if (!working_frame.empty() && working_frame != link_name)
    {
      auto env_wrapper = EnvironmentManager::get(data_->model->getComponentInfo());
      if (env_wrapper != nullptr && env_wrapper->getEnvironment()->isInitialized())
      {
        tesseract_scene_graph::SceneState state = env_wrapper->getEnvironment()->getState();
        auto it1 = state.link_transforms.find(working_frame);
        auto it2 = state.link_transforms.find(link_name);
        if (it1 != state.link_transforms.end() && it2 != state.link_transforms.end())
        {
          Eigen::Isometry3d tf = it2->second.inverse() * it1->second;
          for (auto& seg : tp)
            for (auto& p : seg)
              p = tf * p;
        }
      }
    }
    QString filepath = data_->save_dialog->getFilePath();
    saveToolPath(tp, filepath);
  }

  setEnabled(true);
}

void ToolPathWidget::onOpenFinished(int results)
{
  if (results == 1)
  {
    QString link_name = data_->open_dialog->getLinkName();
    QString filepath = data_->open_dialog->getFilePath();
    openToolPath(data_->model->getComponentInfo(), filepath, link_name);
  }
}

void ToolPathWidget::onCurrentRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
  QModelIndex current_index = current;  // This appears to be changing so copy
  data_->selected_item = data_->model->itemFromIndex(current_index);
}
}  // namespace tesseract_gui
