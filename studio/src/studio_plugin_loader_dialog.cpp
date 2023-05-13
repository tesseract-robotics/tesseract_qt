/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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
#include <tesseract_qt/studio/studio_plugin_loader_dialog.h>
#include <tesseract_qt/studio/studio_plugin_factory.h>
#include "ui_studio_plugin_loader_dialog.h"

#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/models/component_info_model.h>

#include <tesseract_common/plugin_loader.h>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_hash.hpp>

#include <QMenu>
#include <QStringListModel>

namespace tesseract_gui
{
struct StudioPluginLoaderDialog::Implementation
{
  // Plugin factory
  std::shared_ptr<tesseract_gui::StudioPluginFactory> plugin_factory;

  // Menus
  QMenu plugin_context_menu;
  QMenu search_path_context_menu;
  QMenu search_library_context_menu;
  QMenu component_info_context_menu;

  // Models
  QStringListModel search_paths_model;
  QStringListModel search_libraries_model;
  QStringListModel component_info_keys;
  ComponentInfoModel component_info_model;
};

StudioPluginLoaderDialog::StudioPluginLoaderDialog(std::shared_ptr<tesseract_gui::StudioPluginFactory> plugin_factory,
                                                   QWidget* parent)
  : QDialog(parent), ui(std::make_unique<Ui::StudioPluginLoaderDialog>()), data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);
  ui->tool_box->removeItem(0);
  ui->search_paths_list_view->setModel(&data_->search_paths_model);
  ui->search_libraries_list_view->setModel(&data_->search_libraries_model);
  ui->component_infos_tree_view->setModel(&data_->component_info_model);

  // Tests remove next line
  data_->component_info_model.add("test", ComponentInfo());

  setWindowIcon(icons::getPluginIcon());
  setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  setModal(true);

  data_->plugin_factory = std::move(plugin_factory);

  data_->plugin_context_menu.addAction(ui->actionRemove_Plugin);
  data_->plugin_context_menu.setWindowModality(Qt::ApplicationModal);

  data_->search_path_context_menu.addAction(ui->actionAdd_Search_Path);
  data_->search_path_context_menu.addSeparator();
  data_->search_path_context_menu.addAction(ui->actionRemove_Search_Path);
  data_->search_path_context_menu.setWindowModality(Qt::ApplicationModal);

  data_->search_library_context_menu.addAction(ui->actionAdd_Search_Library);
  data_->search_library_context_menu.addSeparator();
  data_->search_library_context_menu.addAction(ui->actionRemove_Search_Library);
  data_->search_library_context_menu.setWindowModality(Qt::ApplicationModal);

  data_->component_info_context_menu.addAction(ui->actionAdd_Component_Info);
  data_->component_info_context_menu.addAction(ui->actionCreate_Child_Component_Info);
  data_->component_info_context_menu.addSeparator();
  data_->component_info_context_menu.addAction(ui->actionRemove_Component_Info);
  data_->component_info_context_menu.setWindowModality(Qt::ApplicationModal);

  ui->actionRemove_Plugin->setIcon(icons::getTrashIcon());
  connect(ui->actionRemove_Plugin, &QAction::triggered, [this]() {
    int current_index = ui->tool_box->currentIndex();
    if (current_index >= 0)
      ui->tool_box->removeItem(current_index);
  });

  ui->actionAdd_Search_Path->setIcon(icons::getTextIcon());
  connect(ui->actionAdd_Search_Path, &QAction::triggered, [this]() {
    QStringList list = data_->search_paths_model.stringList();
    list.append("<add_new_search_path>");
    data_->search_paths_model.setStringList(list);
  });

  ui->actionAdd_Search_Library->setIcon(icons::getTextIcon());
  connect(ui->actionAdd_Search_Library, &QAction::triggered, [this]() {
    QStringList list = data_->search_libraries_model.stringList();
    list.append("<add_new_search_library>");
    data_->search_libraries_model.setStringList(list);
  });

  ui->actionRemove_Search_Path->setIcon(icons::getTrashIcon());
  connect(ui->actionRemove_Search_Path, &QAction::triggered, [this]() {
    data_->search_paths_model.removeRow(ui->search_paths_list_view->currentIndex().row());
    refreshSearchPathsAndLibraries();
  });

  ui->actionRemove_Search_Library->setIcon(icons::getTrashIcon());
  connect(ui->actionRemove_Search_Library, &QAction::triggered, [this]() {
    data_->search_libraries_model.removeRow(ui->search_libraries_list_view->currentIndex().row());
    refreshSearchPathsAndLibraries();
  });

  ui->actionAdd_Component_Info->setIcon(icons::getTextIcon());
  connect(ui->actionAdd_Component_Info, &QAction::triggered, [this]() {
    //    QStringList list = data_->search_libraries_model.stringList();
    //    list.append("<add_new_search_library>");
    //    data_->search_libraries_model.setStringList(list);
  });

  ui->actionCreate_Child_Component_Info->setIcon(icons::getTextIcon());
  connect(ui->actionCreate_Child_Component_Info, &QAction::triggered, [this]() {
    //    QStringList list = data_->search_libraries_model.stringList();
    //    list.append("<add_new_search_library>");
    //    data_->search_libraries_model.setStringList(list);
  });

  ui->actionRemove_Component_Info->setIcon(icons::getTrashIcon());
  connect(ui->actionRemove_Component_Info, &QAction::triggered, [this]() {
    //    data_->search_paths_model.removeRow(ui->search_paths_list_view->currentIndex().row());
    //    refreshSearchPathsAndLibraries();
  });

  connect(ui->tool_box,
          SIGNAL(customContextMenuRequested(const QPoint&)),
          this,
          SLOT(showPluginContextMenu(const QPoint&)));
  connect(ui->component_infos_tree_view,
          SIGNAL(customContextMenuRequested(const QPoint&)),
          this,
          SLOT(showComponentInfoContextMenu(const QPoint&)));
  connect(ui->search_paths_list_view,
          SIGNAL(customContextMenuRequested(const QPoint&)),
          this,
          SLOT(showSearchPathContextMenu(const QPoint&)));

  connect(ui->search_paths_list_view->itemDelegate(), &QAbstractItemDelegate::closeEditor, [this]() {
    refreshSearchPathsAndLibraries();
  });
  connect(ui->search_libraries_list_view,
          SIGNAL(customContextMenuRequested(const QPoint&)),
          this,
          SLOT(showSearchLibraryContextMenu(const QPoint&)));
  connect(ui->search_libraries_list_view->itemDelegate(), &QAbstractItemDelegate::closeEditor, [this]() {
    refreshSearchPathsAndLibraries();
  });
  connect(ui->add_push_button, SIGNAL(clicked()), this, SLOT(addPluginWidget()));
}

StudioPluginLoaderDialog::~StudioPluginLoaderDialog() = default;

void StudioPluginLoaderDialog::showEvent(QShowEvent* event)
{
  // Load search paths into widget
  std::set<std::string> search_paths = data_->plugin_factory->getSearchPaths();
  QStringList qlist_search_paths;
  for (const auto& search_path : search_paths)
    qlist_search_paths.append(search_path.c_str());

  data_->search_paths_model.setStringList(qlist_search_paths);

  // Load search libraries into widget
  std::set<std::string> search_libraries = data_->plugin_factory->getSearchLibraries();
  QStringList qlist_search_libraries;
  for (const auto& search_library : search_libraries)
    qlist_search_libraries.append(search_library.c_str());

  data_->search_libraries_model.setStringList(qlist_search_libraries);

  std::vector<std::string> plugins =
      data_->plugin_factory->getPluginLoader().getAvailablePlugins(StudioConfigWidgetFactory::getSectionName());
  ui->combo_box->clear();
  for (const auto& plugin : plugins)
    ui->combo_box->addItem(QString::fromStdString(plugin));

  ui->add_push_button->setEnabled(!plugins.empty());

  QDialog::showEvent(event);
}

void StudioPluginLoaderDialog::showPluginContextMenu(const QPoint& pos)
{
  QPoint global_pos = ui->tool_box->mapToGlobal(pos);
  if (ui->tool_box->currentWidget() != nullptr)
  {
    QPoint local_pos = ui->tool_box->currentWidget()->mapFromGlobal(global_pos);
    QRect rect = ui->tool_box->currentWidget()->rect();
    rect.setTop(-30);  // Account for page header
    if (rect.contains(local_pos))
      data_->plugin_context_menu.exec(global_pos);
  }
}

void StudioPluginLoaderDialog::showComponentInfoContextMenu(const QPoint& pos)
{
  QPoint global_pos = ui->component_infos_tree_view->mapToGlobal(pos);
  QModelIndex cmi = ui->component_infos_tree_view->selectionModel()->currentIndex();
  QStandardItem* item = data_->component_info_model.itemFromIndex(cmi);
  if (item != nullptr)
  {
    const bool enabled{ item->type() == static_cast<int>(StandardItemType::COMMON_COMPONENT_INFO) };
    ui->actionCreate_Child_Component_Info->setEnabled(enabled);
    ui->actionRemove_Component_Info->setEnabled(enabled);
  }

  data_->component_info_context_menu.exec(global_pos);
}

void StudioPluginLoaderDialog::showSearchPathContextMenu(const QPoint& pos)
{
  QPoint global_pos = ui->search_paths_list_view->mapToGlobal(pos);
  data_->search_path_context_menu.exec(global_pos);
}

void StudioPluginLoaderDialog::showSearchLibraryContextMenu(const QPoint& pos)
{
  QPoint global_pos = ui->search_libraries_list_view->mapToGlobal(pos);
  data_->search_library_context_menu.exec(global_pos);
}

void StudioPluginLoaderDialog::addPluginWidget()
{
  std::string name = "StudioPlugin" + std::to_string(boost::uuids::hash_value(boost::uuids::random_generator()()));
  tesseract_common::PluginInfo plugin_info;
  plugin_info.class_name = ui->combo_box->currentText().toStdString();

  StudioPluginConfigWidget* widget = data_->plugin_factory->createStudioConfigWidget(name, plugin_info);

  if (widget == nullptr)
    return;

  QString title = (plugin_info.class_name + "(" + name + ")").c_str();
  ui->tool_box->addItem(widget, title);
}

void StudioPluginLoaderDialog::refreshSearchPathsAndLibraries()
{
  QStringList qlist_search_paths = data_->search_paths_model.stringList();
  std::set<std::string> search_paths;
  for (const auto& entry : qlist_search_paths)
    search_paths.insert(entry.toStdString());

  data_->plugin_factory->setSearchPaths(search_paths);

  QStringList qlist_search_libraries = data_->search_libraries_model.stringList();
  std::set<std::string> search_libraries;
  for (const auto& entry : qlist_search_libraries)
    search_libraries.insert(entry.toStdString());

  data_->plugin_factory->setSearchLibraries(search_libraries);

  std::vector<std::string> plugins =
      data_->plugin_factory->getPluginLoader().getAvailablePlugins(StudioConfigWidgetFactory::getSectionName());
  ui->combo_box->clear();
  for (const auto& plugin : plugins)
    ui->combo_box->addItem(QString::fromStdString(plugin));

  ui->add_push_button->setEnabled(!plugins.empty());
}

}  // namespace tesseract_gui
