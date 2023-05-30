#include <tesseract_qt/common/widgets/component_info_manager_widget.h>

#include "ui_component_info_manager_widget.h"

#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/component_info_standard_item.h>
#include <tesseract_qt/common/models/component_info_manager_model.h>
#include <tesseract_qt/common/widgets/create_component_info_dialog.h>
#include <tesseract_qt/common/widgets/create_child_component_info_dialog.h>

#include <QMenu>

namespace tesseract_gui
{
struct ComponentInfoManagerWidget::Implementation
{
  // Model
  ComponentInfoManagerModel model;

  // Editable
  bool editable{ false };

  // Menus
  QMenu context_menu;

  // Dialogs
  CreateComponentInfoDialog create_dialog;
  CreateChildComponentInfoDialog create_child_dialog;
};

ComponentInfoManagerWidget::ComponentInfoManagerWidget(QWidget* parent)
  : QWidget(parent), ui(std::make_unique<Ui::ComponentInfoManagerWidget>()), data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);

  // Set model
  ui->tree_view->setModel(&(data_->model));
  data_->model.refresh();

  // Create context menu
  data_->context_menu.addAction(ui->actionCreate);
  data_->context_menu.addAction(ui->actionCreate_Child);
  data_->context_menu.addSeparator();
  data_->context_menu.addAction(ui->actionRemove);
  data_->context_menu.setWindowModality(Qt::ApplicationModal);

  // Connect Actions
  ui->actionCreate->setIcon(icons::getTextIcon());
  connect(ui->actionCreate, &QAction::triggered, [this]() {
    data_->create_dialog.reset();
    if (data_->create_dialog.exec() == 1)
    {
      data_->create_dialog.getComponentInfo();
      refresh();
    }
  });

  ui->actionCreate_Child->setIcon(icons::getTextIcon());
  connect(ui->actionCreate_Child, &QAction::triggered, [this]() {
    QModelIndex cmi = ui->tree_view->currentIndex();
    QStandardItem* item = data_->model.itemFromIndex(cmi);

    if (item->type() == static_cast<int>(StandardItemType::COMMON_COMPONENT_INFO))
    {
      auto* cci_type = static_cast<ComponentInfoStandardItem*>(item);
      data_->create_child_dialog.reset(cci_type->component_info);
      if (data_->create_child_dialog.exec() == 1)
      {
        data_->create_child_dialog.getComponentInfo();
        refresh();
      }
    }
  });

  ui->actionRemove->setIcon(icons::getTrashIcon());
  connect(ui->actionRemove, &QAction::triggered, [this]() {
    QModelIndex cmi = ui->tree_view->currentIndex();
    QStandardItem* item = data_->model.itemFromIndex(cmi);

    //    if (item->type() == static_cast<int>(StandardItemType::COMMON_COMPONENT_INFO))
    //    {
    //      // Make copy of namespace
    //      boost::uuids::uuid ns = static_cast<ComponentInfoStandardItem*>(item)->component_info->getNamespace();
    //      data_->model.remove(ns);
    //    }
  });

  // Connect context menu
  connect(ui->tree_view, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

ComponentInfoManagerWidget::~ComponentInfoManagerWidget() = default;

void ComponentInfoManagerWidget::enableEditMode(bool enable)
{
  data_->editable = enable;
  if (!data_->editable)
    ui->tree_view->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
  else
    ui->tree_view->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
}

void ComponentInfoManagerWidget::refresh()
{
  data_->model.refresh();
  Q_EMIT refreshed();
}

void ComponentInfoManagerWidget::showContextMenu(const QPoint& pos)
{
  QPoint global_pos = ui->tree_view->mapToGlobal(pos);
  QModelIndex cmi = ui->tree_view->selectionModel()->currentIndex();
  QStandardItem* item = data_->model.itemFromIndex(cmi);
  if (item != nullptr)
  {
    const bool enabled{ item->type() == static_cast<int>(StandardItemType::COMMON_COMPONENT_INFO) };
    ui->actionCreate_Child->setEnabled(enabled);
    ui->actionRemove->setEnabled(enabled);
  }

  data_->context_menu.exec(global_pos);
}

}  // namespace tesseract_gui
