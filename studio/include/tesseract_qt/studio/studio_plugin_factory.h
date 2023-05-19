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
#ifndef TESSERACT_QT_STUDIO_STUDIO_PLUGIN_FACTORY_H
#define TESSERACT_QT_STUDIO_STUDIO_PLUGIN_FACTORY_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <string>
#include <map>
#include <yaml-cpp/yaml.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <ads_globals.h>
#include <tesseract_common/types.h>
#include <tesseract_common/plugin_loader.h>
#include <tesseract_qt/studio/studio_plugin_config_widget.h>

// clang-format off
#define TESSERACT_ADD_STUDIO_CONFIG_PLUGIN(DERIVED_CLASS, ALIAS)                                           \
  TESSERACT_ADD_PLUGIN_SECTIONED(DERIVED_CLASS, ALIAS, Studio)
// clang-format on

class QToolBar;
class QMenu;

namespace ads
{
class CDockWidget;
}

namespace tesseract_gui
{
class StudioPluginFactory;
class ComponentInfoModel;

/** @brief Studio Factory class used by the Tesseract Studio for loading objects */
class StudioConfigWidgetFactory
{
public:
  using Ptr = std::shared_ptr<StudioConfigWidgetFactory>;
  using ConstPtr = std::shared_ptr<const StudioConfigWidgetFactory>;

  virtual ~StudioConfigWidgetFactory() = default;

  virtual StudioPluginConfigWidget* create(const std::string& name,
                                           const YAML::Node& config,
                                           std::shared_ptr<const ComponentInfoModel> component_info_model) const = 0;

  static const std::string& getSectionName();

protected:
  static const std::string SECTION_NAME;
  friend class PluginLoader;
};

struct StudioFactoryResult
{
  using UPtr = std::unique_ptr<StudioFactoryResult>;

  ads::CDockWidget* dock_widget{ nullptr };
  ads::DockWidgetArea dock_area{ ads::LeftDockWidgetArea };
  QToolBar* toolbar{ nullptr };
  QMenu* menu{ nullptr };
};

class StudioPluginFactory
{
public:
  StudioPluginFactory();
  ~StudioPluginFactory();
  StudioPluginFactory(const StudioPluginFactory&) = default;
  StudioPluginFactory& operator=(const StudioPluginFactory&) = default;
  StudioPluginFactory(StudioPluginFactory&&) = default;
  StudioPluginFactory& operator=(StudioPluginFactory&&) = default;

  /**
   * @brief Load plugins from yaml node
   * @param config The config node
   */
  StudioPluginFactory(const YAML::Node& config);

  /**
   * @brief Load plugins from file path
   * @param config The config file path
   */
  StudioPluginFactory(const tesseract_common::fs::path& config);

  /**
   * @brief Load plugins from string
   * @param config The config string
   */
  StudioPluginFactory(const std::string& config);

  /**
   * @brief Load plugins from yaml node
   * @param config The config node
   */
  void loadConfig(const YAML::Node& config);

  /**
   * @brief Load plugins from file path
   * @param config The config file path
   */
  void loadConfig(const tesseract_common::fs::path& config);

  /**
   * @brief Load plugins from string
   * @param config The config string
   */
  void loadConfig(const std::string& config);

  /**
   * @brief Add location for the plugin loader to search
   * @param path The full path to the directory
   */
  void addSearchPath(const std::string& path);

  /**
   * @brief Get the plugin search paths
   * @return The search paths
   */
  std::set<std::string> getSearchPaths() const;

  /**
   * @brief Set locations for the plugin loader to search
   * @param paths A set of full path to the directories
   */
  void setSearchPaths(const std::set<std::string>& paths);

  /** @brief Clear the search paths */
  void clearSearchPaths();

  /**
   * @brief Add a library to search for plugin name
   * @param library_name The library name without the prefix or suffix
   */
  void addSearchLibrary(const std::string& library_name);

  /**
   * @brief Get the plugin search libraries
   * @return The search libraries
   */
  std::set<std::string> getSearchLibraries() const;

  /**
   * @brief Add a libraries to search for plugin name
   * @param library_names The library names without the prefix or suffix
   */
  void setSearchLibraries(const std::set<std::string>& library_names);

  /** @brief Clean the search libraries */
  void clearSearchLibraries();

  /**
   * @brief Add a studio plugin
   * @param name The name
   * @param plugin_info The plugin information
   */
  void addStudioPlugin(const std::string& name, tesseract_common::PluginInfo plugin_info);

  /**
   * @brief Check if it has studio plugins
   * @return True if studio PluginInfoMap is not empty, otherwise fale
   */
  bool hasStudioPlugins() const;

  /**
   * @brief Get the map of studio plugins
   * @return A map of plugins
   */
  tesseract_common::PluginInfoMap getStudioPlugins() const;

  /**
   * @brief Remove studio plugin
   * @param name The name of the plugin to remove
   */
  void removeStudioPlugin(const std::string& name);

  /**
   * @brief Get studio config widget given name
   * @details This looks for studio plugin info. If not found nullptr is returned.
   * @param name The name
   */
  StudioPluginConfigWidget* createStudioConfigWidget(const std::string& name) const;

  /**
   * @brief Get studio config widget given name and plugin info
   * @details This looks for studio plugin info. If not found nullptr is returned.
   * @param name The name
   * @param plugin_info The plugin information to create task composer executor object
   */
  StudioPluginConfigWidget* createStudioConfigWidget(const std::string& name,
                                                     const tesseract_common::PluginInfo& plugin_info) const;

  /**
   * @brief Save the plugin information to a yaml config file
   * @param file_path The file path
   */
  void saveConfig(const tesseract_common::fs::path& file_path) const;

  /**
   * @brief Get the plugin information config as a yaml node
   * @return The plugin information config yaml node/
   */
  YAML::Node getConfig() const;

  /**
   * @brief Get the component info model
   * @return The component info model
   */
  std::shared_ptr<ComponentInfoModel> getComponentInfoModel();
  std::shared_ptr<const ComponentInfoModel> getComponentInfoModel() const;

  /** @brief Get the internal plugin loader */
  tesseract_common::PluginLoader& getPluginLoader();
  const tesseract_common::PluginLoader& getPluginLoader() const;

private:
  mutable std::map<std::string, StudioConfigWidgetFactory::Ptr> factories_;
  std::shared_ptr<ComponentInfoModel> component_info_model_;
  tesseract_common::PluginInfoContainer plugin_info_;
  tesseract_common::PluginLoader plugin_loader_;
};
}  // namespace tesseract_gui

#endif  // TESSERACT_QT_STUDIO_STUDIO_PLUGIN_FACTORY_H
