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

#include <tesseract_common/yaml_utils.h>
#include <tesseract_common/yaml_extenstions.h>
#include <boost_plugin_loader/plugin_loader.hpp>

#include <filesystem>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>

#include <yaml-cpp/yaml.h>

#include <tesseract_qt/common/events/log_events.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>
#include <tesseract_qt/common/environment_manager.h>

#include <tesseract_qt/studio/studio_plugin_loader_dialog.h>
#include <tesseract_qt/studio/studio_dock_widget_factory.h>

#if __has_include(<qtadvanceddocking/DockManager.h>)
#include <qtadvanceddocking/DockAreaWidget.h>
#include <qtadvanceddocking/DockManager.h>
#else
#include <DockAreaWidget.h>
#include <DockManager.h>
#endif

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
  boost_plugin_loader::PluginLoader plugin_loader;
  StudioPluginLoaderDialog plugin_loader_dialog;

  std::string central_widget;
  std::vector<StudioDockWidget*> dock_widgets;

  std::filesystem::path config_filepath;
  std::filesystem::path settings_filepath;

  /** @brief Load a config */
  void loadConfig();

  /** @brief Load a config */
  void loadConfigAs();

  /** @brief Save the current state to config */
  void saveConfig();

  /** @brief Save the current state to config */
  void saveConfigAs();

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

  /** @brief Removes all plugins, component infos, and environments */
  void clear();

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
  QStringList std_locations = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
  default_directory = ms.value("default_directory", std_locations[0]).toString();
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

void Studio::Implementation::clear()
{
  perspective_comboBox->clear();

  disconnect(dock_manager, &ads::CDockManager::dockWidgetRemoved, app, nullptr);

  for (auto* dock_widget : dock_widgets)
  {
    dock_manager->removeDockWidget(dock_widget);
    delete dock_widget;
  }

  delete dock_manager;
  dock_manager = new ads::CDockManager(app);
  dock_manager->setStyleSheet("");

  central_widget.clear();
  dock_widgets.clear();

  config_filepath.clear();
  settings_filepath.clear();

  ComponentInfoManager::clear();
  EnvironmentManager::clear();

  connect(dock_manager, &ads::CDockManager::dockWidgetRemoved, [this](ads::CDockWidget* w) { dockWidgetRemoved(w); });
}

void Studio::Implementation::loadConfig()
{
  YAML::Node config = YAML::LoadFile(config_filepath.string());
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

    if (const YAML::Node& dock_widgets_node = config_node[DOCK_WIDGETS_KEY])
    {
      if (const YAML::Node& n = dock_widgets_node["central_widget"])
        central_widget = n.as<std::string>();

      if (const YAML::Node& plugins = dock_widgets_node["plugins"])
      {
        if (!plugins.IsMap())
          throw std::runtime_error(DOCK_WIDGETS_KEY + "::plugins, should contain a map of studio plugin names to "
                                                      "plugins!");

        try
        {
          auto plugin_infos = plugins.as<tesseract_common::PluginInfoMap>();

          // If central widget it must be added first
          if (!central_widget.empty())
          {
            auto it = plugin_infos.find(central_widget);
            if (it == plugin_infos.end())
              throw std::runtime_error("Studio::loadConfig: failed find central widget with name '" + central_widget +
                                       "'");

            QString name = QString::fromStdString(it->first);
            StudioDockWidget* dock_widget = app->createDockWidget(name, it->second);
            if (dock_widget == nullptr)
            {
              tesseract_gui::events::LogErrorEvent event(
                  QString("Studio::loadConfig: Failed to load dock widget '%1' from config.").arg(it->first.c_str()));
              QApplication::sendEvent(qApp, &event);
            }
            else
            {
              app->addDockWidget(dock_widget);
            }
          }

          for (const auto& plugin_info : plugin_infos)
          {
            if (!central_widget.empty() && plugin_info.first == central_widget)
              continue;

            QString name = QString::fromStdString(plugin_info.first);
            StudioDockWidget* dock_widget = app->createDockWidget(name, plugin_info.second);
            if (dock_widget == nullptr)
            {
              tesseract_gui::events::LogErrorEvent event(QString("Studio::loadConfig: Failed to load dock widget '%1' "
                                                                 "from config.")
                                                             .arg(plugin_info.first.c_str()));
              QApplication::sendEvent(qApp, &event);
              continue;
            }

            app->addDockWidget(dock_widget);
          }
        }
        catch (const std::exception& e)
        {
          throw std::runtime_error("Studio::loadConfig: Constructor failed to cast '" + DOCK_WIDGETS_KEY +
                                   "::plugins' to tesseract_common::PluginInfoMap! Details: " + e.what());
        }
      }
    }
  }

  // Load settings
  restoreState();
  restorePerspectives();
}

void Studio::Implementation::loadConfigAs()
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

  QStringList selected_files = dialog.selectedFiles();
  QString filepath = selected_files.first();
  if (QFileInfo::exists(filepath))
    default_directory = QFileInfo(filepath).absoluteDir().path();

  // Clear the current state to a blank slate
  clear();

  config_filepath = filepath.toStdString();
  settings_filepath = config_filepath.string() + ".ini";

  loadConfig();
}

void Studio::Implementation::saveConfig()
{
  if (config_filepath.empty() || settings_filepath.empty())
    return;

  YAML::Node studio_plugins;
  if (!plugin_loader.search_paths.empty())
    studio_plugins[SEARCH_PATHS_KEY] = plugin_loader.search_paths;

  if (!plugin_loader.search_libraries.empty())
    studio_plugins[SEARCH_LIBRARIES_KEY] = plugin_loader.search_libraries;

  if (!tesseract_gui::ComponentInfoManager::empty())
    studio_plugins[COMPONENT_INFOS_KEY] = tesseract_gui::ComponentInfoManager::getConfig();

  if (!central_widget.empty())
    studio_plugins[DOCK_WIDGETS_KEY]["central_widget"] = central_widget;

  tesseract_common::PluginInfoMap plugins;
  for (const auto& dock_widget : dock_widgets)
  {
    tesseract_common::PluginInfo plugin_info;
    plugin_info.class_name = dock_widget->getFactoryClassName();
    plugin_info.config = dock_widget->getConfig();
    plugins[dock_widget->getName().toStdString()] = plugin_info;
  }

  if (!plugins.empty())
    studio_plugins[DOCK_WIDGETS_KEY]["plugins"] = plugins;

  YAML::Node config;
  config[STUDIO_CONFIG_KEY] = studio_plugins;

  // Save config
  std::ofstream fout(config_filepath);
  fout << config;

  // Save settings
  saveState();
  savePerspectives();
}

void Studio::Implementation::saveConfigAs()
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

  QStringList selected_files = dialog.selectedFiles();
  QString filepath = selected_files.first();
  if (QFileInfo::exists(filepath))
    default_directory = QFileInfo(filepath).absoluteDir().path();

  config_filepath = filepath.toStdString();
  settings_filepath = config_filepath.string() + ".ini";

  saveConfig();
}

void Studio::Implementation::saveState()
{
  if (settings_filepath.empty())
    return;

  QSettings Settings(QString::fromStdString(settings_filepath.string()), QSettings::IniFormat);
  Settings.setValue("TesseractStudio/Geometry", app->saveGeometry());
  Settings.setValue("TesseractStudio/State", app->saveState());
  Settings.setValue("TesseractStudio/DockingState", dock_manager->saveState());
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
  if (settings_filepath.empty())
    return;

  QSettings settings(QString::fromStdString(settings_filepath.string()), QSettings::IniFormat);
  dock_manager->savePerspectives(settings);
}

void Studio::Implementation::restoreState()
{
  if (settings_filepath.empty())
    return;

  QSettings settings(QString::fromStdString(settings_filepath.string()), QSettings::IniFormat);
  app->restoreGeometry(settings.value("TesseractStudio/Geometry").toByteArray());
  app->restoreState(settings.value("TesseractStudio/State").toByteArray());
  dock_manager->restoreState(settings.value("TesseractStudio/DockingState").toByteArray());
}

void Studio::Implementation::restorePerspectives()
{
  if (settings_filepath.empty())
    return;

  QSettings settings(QString::fromStdString(settings_filepath.string()), QSettings::IniFormat);
  dock_manager->loadPerspectives(settings);
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
  ui->actionSave_Config_As->setIcon(icons::getSaveIcon());
  ui->actionSave_State->setIcon(icons::getSaveIcon());
  ui->actionRestore_State->setIcon(icons::getRestoreIcon());
  ui->actionCreate_Perspective->setIcon(icons::getLayoutIcon());
  ui->actionLoad_Plugins->setIcon(icons::getPluginIcon());
  connect(ui->actionLoad_Config, &QAction::triggered, [this]() {
    ui->actionSave_Config->setEnabled(false);
    data_->loadConfigAs();
    if (!data_->config_filepath.empty() && !data_->settings_filepath.empty())
      ui->actionSave_Config->setEnabled(true);
  });
  connect(ui->actionSave_Config, &QAction::triggered, [this]() { data_->saveConfig(); });
  connect(ui->actionSave_Config_As, &QAction::triggered, [this]() {
    data_->saveConfigAs();
    if (!data_->config_filepath.empty() && !data_->settings_filepath.empty())
      ui->actionSave_Config->setEnabled(true);
  });
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
}

Studio::~Studio() = default;

bool Studio::init(int argc, char** argv)
{
  namespace po = boost::program_options;
  std::string config;
  po::options_description desc("Options");
  desc.add_options()("help,h",
                     "Print help messages")("config,c", po::value<std::string>(&config), "File path to config.");

  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(argc, argv, desc), vm);  // can throw

    /** --help option */
    if (vm.count("help") != 0U)
    {
      std::stringstream ss;
      ss << "Basic Command Line Parameter App" << std::endl << desc << std::endl;
      tesseract_gui::events::LogInfoEvent event(ss.str().c_str());
      QApplication::sendEvent(qApp, &event);
      return false;
    }

    // throws on error, so do after help in case there are any problems
    po::notify(vm);
  }
  catch (po::error& e)
  {
    std::stringstream ss;
    ss << "ERROR: " << e.what() << std::endl << std::endl;
    ss << desc << std::endl;
    tesseract_gui::events::LogErrorEvent event(ss.str().c_str());
    QApplication::sendEvent(qApp, &event);
    return false;
  }

  if (!config.empty())
    loadConfig(QString::fromStdString(config));

  return true;
}

void Studio::loadConfig(const QString& filepath)
{
  if (QFileInfo::exists(filepath))
    data_->default_directory = QFileInfo(filepath).absoluteDir().path();

  // Clear the current state to a blank slate
  data_->clear();

  data_->config_filepath = filepath.toStdString();
  data_->settings_filepath = data_->config_filepath.string() + ".ini";

  data_->loadConfig();
}

void Studio::closeEvent(QCloseEvent* event)
{
  // Delete dock manager here to delete all floating widgets. This ensures
  // that all top level windows of the dock manager are properly closed
  data_->dock_manager->deleteLater();
  QMainWindow::closeEvent(event);
}

boost_plugin_loader::PluginLoader& Studio::getPluginLoader() { return data_->plugin_loader; }
const boost_plugin_loader::PluginLoader& Studio::getPluginLoader() const { return data_->plugin_loader; }

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

    auto plugin = data_->plugin_loader.createInstance<StudioDockWidgetFactory>(plugin_info.class_name);
    if (plugin == nullptr)
    {
      tesseract_gui::events::LogWarnEvent event(
          QString("Failed to load symbol '%1'").arg(plugin_info.class_name.c_str()));
      QApplication::sendEvent(qApp, &event);
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
    tesseract_gui::events::LogWarnEvent event(
        QString("Failed to load symbol '%1', Details: %2").arg(plugin_info.class_name.c_str(), e.what()));
    QApplication::sendEvent(qApp, &event);
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
    if (data_->dock_manager->centralWidget() != nullptr)
      throw std::runtime_error("Only able to have one central widget!");

    auto* central_area = data_->dock_manager->setCentralWidget(dock_widget);
    central_area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);
    data_->central_widget = dock_widget->getName().toStdString();
  }
  else
  {
    data_->dock_manager->addDockWidget(ads::DockWidgetArea::LeftDockWidgetArea, dock_widget);
    ui->menuView->addAction(dock_widget->toggleViewAction());
  }

  data_->dock_widgets.push_back(dock_widget);
}
}  // namespace tesseract_gui
