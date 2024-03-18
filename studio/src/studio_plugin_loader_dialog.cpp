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
#include <tesseract_qt/studio/studio_dock_widget_factory.h>
#include <tesseract_qt/studio/studio.h>

#include "ui_studio_plugin_loader_dialog.h"

#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/component_info_standard_item.h>
#include <tesseract_qt/common/models/component_info_manager_model.h>
#include <tesseract_qt/common/widgets/create_component_info_dialog.h>
#include <tesseract_qt/common/widgets/create_child_component_info_dialog.h>

#include <tesseract_common/plugin_loader.h>
#include <tesseract_common/plugin_info.h>
#include <tesseract_common/types.h>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_hash.hpp>

#include <QMenu>
#include <QStringListModel>

namespace tesseract_gui
{
struct StudioPluginLoaderDialog::Implementation
{
  // Application
  Studio* app;

  // Menus
  QMenu search_path_context_menu;
  QMenu search_library_context_menu;

  // Models
  QStringListModel search_paths_model;
  QStringListModel search_libraries_model;
};

StudioPluginLoaderDialog::StudioPluginLoaderDialog(Studio* app)
  : QDialog(app), ui(std::make_unique<Ui::StudioPluginLoaderDialog>()), data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);
  ui->search_paths_list_view->setModel(&data_->search_paths_model);
  ui->search_libraries_list_view->setModel(&data_->search_libraries_model);

  setWindowIcon(icons::getPluginIcon());
  setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  setModal(true);

  data_->app = app;

  data_->search_path_context_menu.addAction(ui->actionAdd_Search_Path);
  data_->search_path_context_menu.addSeparator();
  data_->search_path_context_menu.addAction(ui->actionRemove_Search_Path);
  data_->search_path_context_menu.setWindowModality(Qt::ApplicationModal);

  data_->search_library_context_menu.addAction(ui->actionAdd_Search_Library);
  data_->search_library_context_menu.addSeparator();
  data_->search_library_context_menu.addAction(ui->actionRemove_Search_Library);
  data_->search_library_context_menu.setWindowModality(Qt::ApplicationModal);

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
  connect(ui->plugin_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(generateUniqueName()));
}

StudioPluginLoaderDialog::~StudioPluginLoaderDialog() = default;

void StudioPluginLoaderDialog::showEvent(QShowEvent* event)
{
  // Load search paths into widget
  std::set<std::string> search_paths = data_->app->getPluginLoader().search_paths;
  QStringList qlist_search_paths;
  for (const auto& search_path : search_paths)
    qlist_search_paths.append(search_path.c_str());

  data_->search_paths_model.setStringList(qlist_search_paths);

  // Load search libraries into widget
  std::set<std::string> search_libraries = data_->app->getPluginLoader().search_libraries;
  QStringList qlist_search_libraries;
  for (const auto& search_library : search_libraries)
    qlist_search_libraries.append(search_library.c_str());

  data_->search_libraries_model.setStringList(qlist_search_libraries);

  std::vector<std::string> plugins =
      data_->app->getPluginLoader().getAvailablePlugins(StudioDockWidgetFactory::getSectionName());
  ui->plugin_combo_box->clear();
  for (const auto& plugin : plugins)
    ui->plugin_combo_box->addItem(QString::fromStdString(plugin));

  ui->add_push_button->setEnabled(!plugins.empty());

  // Generate new unique name
  generateUniqueName();

  QDialog::showEvent(event);
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
  QString name = ui->plugin_name_line_edit->text();
  tesseract_common::PluginInfo plugin_info;
  plugin_info.class_name = ui->plugin_combo_box->currentText().toStdString();

  StudioDockWidget* dock_widget = data_->app->createDockWidget(name, plugin_info);

  if (dock_widget == nullptr)
    return;

  data_->app->addDockWidget(dock_widget);

  generateUniqueName();
}

void StudioPluginLoaderDialog::refreshSearchPathsAndLibraries()
{
  QStringList qlist_search_paths = data_->search_paths_model.stringList();
  std::set<std::string> search_paths;
  for (const auto& entry : qlist_search_paths)
    search_paths.insert(entry.toStdString());

  data_->app->getPluginLoader().search_paths = search_paths;

  QStringList qlist_search_libraries = data_->search_libraries_model.stringList();
  std::set<std::string> search_libraries;
  for (const auto& entry : qlist_search_libraries)
    search_libraries.insert(entry.toStdString());

  data_->app->getPluginLoader().search_libraries = search_libraries;

  std::vector<std::string> plugins =
      data_->app->getPluginLoader().getAvailablePlugins(StudioDockWidgetFactory::getSectionName());
  ui->plugin_combo_box->clear();
  for (const auto& plugin : plugins)
    ui->plugin_combo_box->addItem(QString::fromStdString(plugin));

  ui->add_push_button->setEnabled(!plugins.empty());
}

void StudioPluginLoaderDialog::generateUniqueName()
{
  std::string name = "StudioPlugin" + std::to_string(boost::uuids::hash_value(boost::uuids::random_generator()()));
  ui->plugin_name_line_edit->setText(QString::fromStdString(name));
}

}  // namespace tesseract_gui
