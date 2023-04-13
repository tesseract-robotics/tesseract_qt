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
#include <tesseract_qt/tool_path/models/tool_path_model.h>
#include <tesseract_qt/tool_path/models/tool_path_selection_model.h>
#include <tesseract_qt/tool_path/models/tool_path_utils.h>
#include <tesseract_qt/common/tool_path_standard_item.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/tree_view.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/events/tool_path_events.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QStandardItemModel>

namespace tesseract_gui
{
struct ToolPathWidget::Implementation
{
  std::shared_ptr<ToolPathModel> model;
  std::shared_ptr<ToolPathSelectionModel> selection_model;
  QVBoxLayout* layout;
  TreeView* tree_view;

  QString default_directory;

  std::unique_ptr<QFileDialog> open_dialog;
  QStringList open_dialog_filters;
  QStringList open_dialog_ext;

  std::unique_ptr<QFileDialog> save_dialog;
  QStringList save_dialog_filters;

  // Store the selected item
  QStandardItem* selected_item{ nullptr };
};

ToolPathWidget::ToolPathWidget(QWidget* parent) : ToolPathWidget(ComponentInfo(), parent) {}

ToolPathWidget::ToolPathWidget(ComponentInfo component_info, QWidget* parent)
  : QWidget(parent), data_(std::make_unique<Implementation>())
{
  // Create model
  data_->model = std::make_shared<ToolPathModel>(component_info);
  data_->selection_model = std::make_shared<ToolPathSelectionModel>(data_->model.get(), component_info);

  // Load Qt Settings
  QSettings ms;
  ms.beginGroup("ToolPathWidget");
  data_->default_directory =
      ms.value("default_directory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]).toString();
  ms.endGroup();

  // Create tree widget
  data_->tree_view = new TreeView();
  data_->tree_view->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
  data_->tree_view->setModel(data_->model.get());

  // Create layout
  data_->layout = new QVBoxLayout();
  data_->layout->setMargin(0);
  data_->layout->setSpacing(0);
  data_->layout->addWidget(data_->tree_view);

  // Set layout
  setLayout(data_->layout);

  {  // Setup open dialog
    data_->open_dialog_filters.append("Tool Path XML (*.tpx)");
    data_->open_dialog_filters.append("Tool Path Binary (*.tpb)");
    data_->open_dialog_filters.append("Tool Path YAML (*.yaml)");
    data_->open_dialog_filters.append("Tool Path Json (*.json)");

    data_->open_dialog_ext.append("tpx");
    data_->open_dialog_ext.append("tpb");
    data_->open_dialog_ext.append("yaml");
    data_->open_dialog_ext.append("json");

    data_->open_dialog = std::make_unique<QFileDialog>(nullptr, "Open Tool Path", data_->default_directory);
    data_->open_dialog->setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
    data_->open_dialog->setModal(true);
    data_->open_dialog->setAcceptMode(QFileDialog::AcceptOpen);
    data_->open_dialog->setNameFilters(data_->open_dialog_filters);

    connect(data_->open_dialog.get(), SIGNAL(finished(int)), this, SLOT(onOpenFinished(int)));
  }

  {  // Setup save dialog
    data_->save_dialog_filters.append("Tool Path XML (*.tpx)");
    data_->save_dialog_filters.append("Tool Path Binary (*.tpb)");
    data_->save_dialog_filters.append("Tool Path YAML (*.yaml)");

    data_->save_dialog = std::make_unique<QFileDialog>(nullptr, "Save Tool Path", data_->default_directory);
    data_->save_dialog->setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
    data_->save_dialog->setModal(true);
    data_->save_dialog->setAcceptMode(QFileDialog::AcceptSave);
    data_->save_dialog->setNameFilters(data_->save_dialog_filters);

    connect(data_->save_dialog.get(), SIGNAL(finished(int)), this, SLOT(onSaveFinished(int)));
  }

  connect(data_->tree_view->selectionModel(),
          SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
          this,
          SLOT(onCurrentRowChanged(QModelIndex, QModelIndex)));

  // Install event filter for interactive view controller
  qGuiApp->installEventFilter(this);
}
ToolPathWidget::~ToolPathWidget()
{
  QSettings ms;
  ms.beginGroup("ToolPathWidget");
  ms.setValue("default_directory", data_->default_directory);
  ms.endGroup();
}

void ToolPathWidget::setComponentInfo(ComponentInfo component_info)
{
  // Create model
  data_->model = std::make_shared<ToolPathModel>(std::move(component_info));
  data_->selection_model = std::make_shared<ToolPathSelectionModel>(data_->model.get(), component_info);

  data_->tree_view->setModel(data_->model.get());
  data_->tree_view->setSelectionModel(data_->selection_model.get());
}

const ComponentInfo& ToolPathWidget::getComponentInfo() const { return data_->model->getComponentInfo(); }

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
  if (event->type() == events::ToolPathOpen::kType)
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
  else if (event->type() == events::ToolPathSave::kType)
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
    tesseract_common::Toolpath tp = dynamic_cast<ToolPathStandardItem*>(data_->selected_item)->getCommonToolPath();
    data_->default_directory = QFileInfo(data_->save_dialog->selectedFiles()[0]).absoluteDir().path();
    QString suffix;
    if (data_->save_dialog->selectedNameFilter() == data_->save_dialog_filters[0])
      suffix = "tpx";
    else if (data_->save_dialog->selectedNameFilter() == data_->save_dialog_filters[1])
      suffix = "tpb";
    else if (data_->save_dialog->selectedNameFilter() == data_->save_dialog_filters[2])
      suffix = "yaml";
    else
      throw std::runtime_error("ToolPathWidget, save has invalid extension!");

    saveToolPath(tp, data_->save_dialog->selectedFiles()[0], suffix);
  }

  setEnabled(true);
}

void ToolPathWidget::onOpenFinished(int results)
{
  if (results == 1)
  {
    int idx = data_->open_dialog_filters.indexOf(data_->open_dialog->selectedNameFilter());
    data_->default_directory = QFileInfo(data_->open_dialog->selectedFiles()[0]).absoluteDir().path();
    openToolPath(data_->model->getComponentInfo(), data_->open_dialog->selectedFiles()[0], data_->open_dialog_ext[idx]);
  }
}

void ToolPathWidget::onCurrentRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
  QModelIndex current_index = current;  // This appears to be changing so copy
  data_->selected_item = data_->model->itemFromIndex(current_index);
}
}  // namespace tesseract_gui
