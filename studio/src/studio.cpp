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
#include <tesseract_qt/studio/studio.h>
#include "ui_studio.h"

#include <tesseract_common/plugin_loader.hpp>
#include <tesseract_common/yaml_utils.h>

#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>

#include <tesseract_qt/studio/studio_plugin_loader_dialog.h>
#include <tesseract_qt/studio/studio_dock_widget_factory.h>

#include <DockManager.h>
#include <DockAreaWidget.h>

#include <QSettings>
#include <QWidgetAction>
#include <QComboBox>
#include <QInputDialog>
#include <QDialog>
#include <QFileDialog>
#include <QStandardPaths>

static const std::string TESSERACT_STUDIO_PLUGIN_DIRECTORIES_ENV = "TESSERACT_STUDIO_PLUGIN_DIRECTORIES";
static const std::string TESSERACT_STUDIO_PLUGINS_ENV = "TESSERACT_STUDIO_PLUGINS";
static const std::string STUDIO_CONFIG_KEY{ "studio_config" };
static const std::string SEARCH_PATHS_KEY{ "search_paths" };
static const std::string SEARCH_LIBRARIES_KEY{ "search_libraries" };
static const std::string COMPONENT_INFOS_KEY{ "component_infos" };
static const std::string DOCK_WIDGETS_KEY{ "dock_widgets" };

namespace tesseract_gui
{
struct Studio::Implementation
{
  Implementation(Studio* app);
  ~Implementation();

  Studio* app;
  ads::CDockManager* dock_manager{ nullptr };
  QWidgetAction* perspective_list_action{ nullptr };
  QComboBox* perspective_comboBox{ nullptr };
  QString default_directory;

  std::map<std::string, StudioDockWidgetFactory::Ptr> factories;
  tesseract_common::PluginLoader plugin_loader;
  StudioPluginLoaderDialog plugin_loader_dialog;

  std::map<ads::DockWidgetArea, ads::CDockAreaWidget*> dock_areas;
  std::vector<StudioDockWidget*> dock_widgets;

  /** @brief Load a config */
  void loadConfig();

  /** @brief Save the current state to config */
  void saveConfig();

  /** @brief Saves the dock manager state and the main window geometry */
  void saveState();

  /** @brief Create a perspective */
  void createPerspective();

  /** @brief Save the list of perspectives */
  void savePerspectives();

  /** @brief Restores the dock manager state */
  void restoreState();

  /** @brief Restore the perspective list of the dock manager */
  void restorePerspectives();

  /**
   * @brief This function is called when a dock widget is removed from the application.
   * @param dock_widget The dock widget removed
   */
  void dockWidgetRemoved(ads::CDockWidget* dock_widget);
};

Studio::Implementation::Implementation(Studio* app) : app(app), plugin_loader_dialog(app)
{
  // Config plugin loader
  plugin_loader.search_libraries_env = TESSERACT_STUDIO_PLUGINS_ENV;
  plugin_loader.search_paths_env = TESSERACT_STUDIO_PLUGIN_DIRECTORIES_ENV;
  plugin_loader.search_paths.insert(TESSERACT_STUDIO_PLUGIN_PATH);
  boost::split(
      plugin_loader.search_libraries, TESSERACT_STUDIO_PLUGINS, boost::is_any_of(":"), boost::token_compress_on);

  // Load settings
  QSettings ms;
  ms.beginGroup("TesseractQtStudio");
  default_directory =
      ms.value("default_directory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]).toString();
  ms.endGroup();
}

Studio::Implementation::~Implementation()
{
  // Save settings
  QSettings ms;
  ms.beginGroup("TesseractQtStudio");
  ms.setValue("default_directory", default_directory);
  ms.endGroup();
}

void Studio::Implementation::loadConfig()
{
  QStringList filters;
  filters.append("Tesseract Studio Config (*.studio)");

  QFileDialog dialog(app, "Open Tesseract Studio Config", default_directory);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  dialog.setModal(true);
  dialog.setNameFilters(filters);
  if (dialog.exec() != 1)
    return;

  QString config_filepath = dialog.selectedFiles().first();
  if (QFileInfo(config_filepath).exists())
    default_directory = QFileInfo(config_filepath).absoluteDir().path();

  YAML::Node config = YAML::LoadFile(config_filepath.toStdString());
  if (const YAML::Node& config_node = config[STUDIO_CONFIG_KEY])
  {
    if (const YAML::Node& search_paths = config_node[SEARCH_PATHS_KEY])
    {
      std::set<std::string> sp;
      try
      {
        sp = search_paths.as<std::set<std::string>>();
      }
      catch (const std::exception& e)
      {
        throw std::runtime_error("Studio::loadConfig: Constructor failed to cast '" + SEARCH_PATHS_KEY +
                                 "' to std::set<std::string>! "
                                 "Details: " +
                                 e.what());
      }
      plugin_loader.search_paths.insert(sp.begin(), sp.end());
    }

    if (const YAML::Node& search_libraries = config_node[SEARCH_LIBRARIES_KEY])
    {
      std::set<std::string> sl;
      try
      {
        sl = search_libraries.as<std::set<std::string>>();
      }
      catch (const std::exception& e)
      {
        throw std::runtime_error("Studio::loadConfig: Constructor failed to cast '" + SEARCH_LIBRARIES_KEY +
                                 "' to std::set<std::string>! "
                                 "Details: " +
                                 e.what());
      }
      plugin_loader.search_libraries.insert(sl.begin(), sl.end());
    }

    if (const YAML::Node& component_infos = config_node[COMPONENT_INFOS_KEY])
    {
      if (!component_infos.IsMap())
        throw std::runtime_error(COMPONENT_INFOS_KEY + ", should contain a map of component info names to component "
                                                       "info!");

      try
      {
        tesseract_gui::ComponentInfoManager::loadConfig(component_infos);
      }
      catch (const std::exception& e)
      {
        throw std::runtime_error(
            "Studio::loadConfig: Constructor failed to cast '" + COMPONENT_INFOS_KEY +
            "' to std::unordered_map<std::string, tesseract_gui::ComponentInfo>! Details: " + e.what());
      }
    }

    if (const YAML::Node& plugins = config_node[DOCK_WIDGETS_KEY])
    {
      if (!plugins.IsMap())
        throw std::runtime_error(DOCK_WIDGETS_KEY + ", should contain a map of studio plugin names to "
                                                    "plugins!");

      try
      {
        auto plugin_infos = plugins.as<tesseract_common::PluginInfoMap>();

        for (const auto& plugin_info : plugin_infos)
        {
          QString name = QString::fromStdString(plugin_info.first);
          StudioDockWidget* dock_widget = app->createDockWidget(name, plugin_info.second);
          if (dock_widget == nullptr)
          {
            CONSOLE_BRIDGE_logError("Studio::loadConfig: Failed to load dock widget '%s' from config.",
                                    plugin_info.first.c_str());
            continue;
          }

          app->addDockWidget(dock_widget);
        }
      }
      catch (const std::exception& e)
      {
        throw std::runtime_error("Studio::loadConfig: Constructor failed to cast '" + DOCK_WIDGETS_KEY +
                                 "' to tesseract_common::PluginInfoContainer! Details: " + e.what());
      }
    }
  }
}

void Studio::Implementation::saveConfig()
{
  QStringList filters;
  filters.append("Tesseract Studio Config (*.studio)");

  QFileDialog dialog(app, "Save Tesseract Studio Config", default_directory);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  dialog.setModal(true);
  dialog.setNameFilters(filters);
  dialog.setDefaultSuffix(".studio");
  if (dialog.exec() != 1)
    return;

  QString config_filepath = dialog.selectedFiles().first();
  if (QFileInfo(config_filepath).exists())
    default_directory = QFileInfo(config_filepath).absoluteDir().path();

  YAML::Node studio_plugins;
  if (!plugin_loader.search_paths.empty())
    studio_plugins[SEARCH_PATHS_KEY] = plugin_loader.search_paths;

  if (!plugin_loader.search_libraries.empty())
    studio_plugins[SEARCH_LIBRARIES_KEY] = plugin_loader.search_libraries;

  if (!tesseract_gui::ComponentInfoManager::empty())
    studio_plugins[COMPONENT_INFOS_KEY] = tesseract_gui::ComponentInfoManager::getConfig();

  tesseract_common::PluginInfoMap plugins;
  for (const auto& dock_widget : dock_widgets)
  {
    tesseract_common::PluginInfo plugin_info;
    plugin_info.class_name = dock_widget->getFactoryClassName();
    plugin_info.config = dock_widget->getConfig();
    plugins[dock_widget->getName().toStdString()] = plugin_info;
  }

  if (!plugins.empty())
    studio_plugins[DOCK_WIDGETS_KEY] = plugins;

  YAML::Node config;
  config[STUDIO_CONFIG_KEY] = studio_plugins;

  std::ofstream fout(config_filepath.toStdString());
  fout << config;
}

void Studio::Implementation::saveState()
{
  QSettings Settings("TesseractQtStudioSettings.ini", QSettings::IniFormat);
  Settings.setValue("TesseractQtStudio/Geometry", app->saveGeometry());
  Settings.setValue("TesseractQtStudio/State", app->saveState());
  Settings.setValue("TesseractQtStudio/DockingState", dock_manager->saveState());
}

void Studio::Implementation::createPerspective()
{
  QString PerspectiveName = QInputDialog::getText(app, "Create Perspective", "Enter unique name:");
  if (PerspectiveName.isEmpty())
    return;

  dock_manager->addPerspective(PerspectiveName);
  QSignalBlocker Blocker(perspective_comboBox);
  perspective_comboBox->clear();
  perspective_comboBox->addItems(dock_manager->perspectiveNames());
  perspective_comboBox->setCurrentText(PerspectiveName);

  savePerspectives();
}

void Studio::Implementation::savePerspectives()
{
  QSettings Settings("TesseractQtStudioSettings.ini", QSettings::IniFormat);
  dock_manager->savePerspectives(Settings);
}

void Studio::Implementation::restoreState()
{
  QSettings Settings("TesseractQtStudioSettings.ini", QSettings::IniFormat);
  app->restoreGeometry(Settings.value("TesseractQtStudio/Geometry").toByteArray());
  app->restoreState(Settings.value("TesseractQtStudio/State").toByteArray());
  dock_manager->restoreState(Settings.value("TesseractQtStudio/DockingState").toByteArray());
}

void Studio::Implementation::restorePerspectives()
{
  QSettings Settings("TesseractQtStudioSettings.ini", QSettings::IniFormat);
  dock_manager->loadPerspectives(Settings);
  perspective_comboBox->clear();
  perspective_comboBox->addItems(dock_manager->perspectiveNames());
}

void Studio::Implementation::dockWidgetRemoved(ads::CDockWidget* dock_widget)
{
  dock_widgets.erase(std::remove(dock_widgets.begin(), dock_widgets.end(), dock_widget), dock_widgets.end());
}

Studio::Studio(QWidget* parent)
  : QMainWindow(parent), ui(std::make_unique<Ui::Studio>()), data_(std::make_unique<Implementation>(this))
{
  ui->setupUi(this);
  setWindowTitle(QApplication::applicationName());

  data_->dock_manager = new ads::CDockManager(this);
  data_->dock_manager->setStyleSheet("");

  // Setup actions
  ui->actionLoad_Config->setIcon(icons::getOpenIcon());
  ui->actionSave_Config->setIcon(icons::getSaveIcon());
  ui->actionSave_State->setIcon(icons::getSaveIcon());
  ui->actionRestore_State->setIcon(icons::getRestoreIcon());
  ui->actionCreate_Perspective->setIcon(icons::getLayoutIcon());
  ui->actionLoad_Plugins->setIcon(icons::getPluginIcon());
  connect(ui->actionLoad_Config, &QAction::triggered, [this]() { data_->loadConfig(); });
  connect(ui->actionSave_Config, &QAction::triggered, [this]() { data_->saveConfig(); });
  connect(ui->actionSave_State, &QAction::triggered, [this]() { data_->saveState(); });
  connect(ui->actionRestore_State, &QAction::triggered, [this]() { data_->restoreState(); });
  connect(ui->actionCreate_Perspective, &QAction::triggered, [this]() { data_->createPerspective(); });
  connect(ui->actionLoad_Plugins, &QAction::triggered, [this]() { data_->plugin_loader_dialog.show(); });
  data_->perspective_list_action = new QWidgetAction(this);
  data_->perspective_comboBox = new QComboBox(this);
  data_->perspective_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  data_->perspective_comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  data_->perspective_list_action->setDefaultWidget(data_->perspective_comboBox);
  ui->toolBar->addAction(data_->perspective_list_action);
  connect(data_->perspective_comboBox,
          SIGNAL(activated(const QString&)),
          data_->dock_manager,
          SLOT(openPerspective(const QString&)));

  connect(data_->dock_manager, &ads::CDockManager::dockWidgetRemoved, [this](ads::CDockWidget* w) {
    data_->dockWidgetRemoved(w);
  });

  //  auto render_widget = new tesseract_gui::RenderWidget(data_->component_info->getSceneName());
  //  render_widget->setSkyEnabled(true);
  //  auto* render_dock_widget = new ads::CDockWidget("Scene");
  //  render_dock_widget->setWidget(render_widget);
  //  render_dock_widget->setFeature(ads::CDockWidget::DockWidgetFocusable, true);
  //  auto* central_area = data_->dock_manager->setCentralWidget(render_dock_widget);
  //  central_area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

  //  auto* env_dock_widget = new ads::CDockWidget("Environment");
  //  env_dock_widget->setWidget(new tesseract_gui::EnvironmentWidget(data_->component_info));
  //  env_dock_widget->setToolBar(new tesseract_gui::SceneGraphToolBar(data_->component_info));
  //  env_dock_widget->toolBar()->setIconSize(QSize(25, 25));
  //  data_->dock_manager->addDockWidget(ads::LeftDockWidgetArea, env_dock_widget, central_area);
  //  ui->menuView->addAction(env_dock_widget->toggleViewAction());

  //  auto* tool_path_dock_widget = new ads::CDockWidget("Tool Paths");
  //  tool_path_dock_widget->setWidget(new tesseract_gui::ToolPathWidget(data_->component_info));
  //  tool_path_dock_widget->setToolBar(new tesseract_gui::ToolPathToolBar(data_->component_info));
  //  tool_path_dock_widget->toolBar()->setIconSize(QSize(25, 25));
  //  auto* right_area = data_->dock_manager->addDockWidget(ads::RightDockWidgetArea, tool_path_dock_widget,
  //  central_area); ui->menuView->addAction(tool_path_dock_widget->toggleViewAction());

  //  auto* joint_traj_dock_widget = new ads::CDockWidget("Joint Trajectories");
  //  joint_traj_dock_widget->setWidget(new tesseract_gui::JointTrajectoryWidget(data_->jt_component_info));
  //  joint_traj_dock_widget->setToolBar(new tesseract_gui::JointTrajectoryToolBar(data_->jt_component_info));
  //  joint_traj_dock_widget->toolBar()->setIconSize(QSize(25, 25));
  //  data_->dock_manager->addDockWidgetTabToArea(joint_traj_dock_widget, right_area);
  //  ui->menuView->addAction(joint_traj_dock_widget->toggleViewAction());

  //  auto* manipulation_dock_widget = new ads::CDockWidget("Manipulation");
  //  manipulation_dock_widget->setWidget(new tesseract_gui::ManipulationWidget(data_->component_info, true));
  //  manipulation_dock_widget->setToolBar(new tesseract_gui::ManipulationToolBar(data_->component_info));
  //  manipulation_dock_widget->toolBar()->setIconSize(QSize(25, 25));
  //  data_->dock_manager->addDockWidgetTabToArea(manipulation_dock_widget, right_area);
  //  ui->menuView->addAction(manipulation_dock_widget->toggleViewAction());

  //  data_->restoreState(); If this is enabled widgets do not show up if name change
  data_->restorePerspectives();
}

Studio::~Studio() = default;

tesseract_common::PluginLoader& Studio::getPluginLoader() { return data_->plugin_loader; }
const tesseract_common::PluginLoader& Studio::getPluginLoader() const { return data_->plugin_loader; }

StudioDockWidget* Studio::createDockWidget(const QString& name, const tesseract_common::PluginInfo& plugin_info)
{
  try
  {
    auto it = data_->factories.find(plugin_info.class_name);
    if (it != data_->factories.end())
    {
      StudioDockWidget* dock_widget = it->second->create(name);
      if (dock_widget != nullptr)
      {
        if (!plugin_info.config.IsNull())
          dock_widget->loadConfig(plugin_info.config);
      }
      return dock_widget;
    }

    auto plugin = data_->plugin_loader.instantiate<StudioDockWidgetFactory>(plugin_info.class_name);
    if (plugin == nullptr)
    {
      CONSOLE_BRIDGE_logWarn("Failed to load symbol '%s'", plugin_info.class_name.c_str());
      return nullptr;
    }
    data_->factories[plugin_info.class_name] = plugin;

    StudioDockWidget* dock_widget = plugin->create(name);
    if (dock_widget != nullptr)
    {
      if (!plugin_info.config.IsNull())
        dock_widget->loadConfig(plugin_info.config);
    }
    return dock_widget;
  }
  catch (const std::exception& e)
  {
    CONSOLE_BRIDGE_logWarn("Failed to load symbol '%s', Details: %s", plugin_info.class_name.c_str(), e.what());
    return nullptr;
  }
}

void Studio::addDockWidget(StudioDockWidget* dock_widget)
{
  if (!dock_widget->isInitialized())
  {
    dock_widget->onInitialize();
    if (!dock_widget->isInitialized())
    {
      delete dock_widget;
      return;
    }
  }

  if (dock_widget->assignAsCentralWidget())
  {
    auto it = data_->dock_areas.find(ads::DockWidgetArea::CenterDockWidgetArea);
    if (it != data_->dock_areas.end())
      throw std::runtime_error("Only able to have one central widget!");

    auto* central_area = data_->dock_manager->setCentralWidget(dock_widget);
    central_area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);
    data_->dock_areas[ads::DockWidgetArea::CenterDockWidgetArea] = central_area;
  }
  else
  {
    auto it = data_->dock_areas.find(dock_widget->getDesiredDockArea());
    if (it != data_->dock_areas.end())
    {
      data_->dock_manager->addDockWidgetTabToArea(dock_widget, it->second);
    }
    else
    {
      ads::CDockAreaWidget* dock_area{ nullptr };
      auto it = data_->dock_areas.find(ads::DockWidgetArea::CenterDockWidgetArea);
      if (it != data_->dock_areas.end())
        dock_area = data_->dock_manager->addDockWidget(dock_widget->getDesiredDockArea(), dock_widget, it->second);
      else
        dock_area = data_->dock_manager->addDockWidget(dock_widget->getDesiredDockArea(), dock_widget);

      data_->dock_areas[dock_widget->getDesiredDockArea()] = dock_area;
    }
    ui->menuView->addAction(dock_widget->toggleViewAction());
  }

  data_->dock_widgets.push_back(dock_widget);
}
}  // namespace tesseract_gui
