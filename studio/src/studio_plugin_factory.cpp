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

#include <tesseract_common/plugin_loader.hpp>
#include <tesseract_common/yaml_utils.h>
#include <tesseract_qt/studio/studio_plugin_factory.h>
#include <tesseract_qt/common/plugin_infos.h>
#include <tesseract_qt/common/yaml_utils.h>

static const std::string TESSERACT_STUDIO_PLUGIN_DIRECTORIES_ENV = "TESSERACT_STUDIO_PLUGIN_DIRECTORIES";
static const std::string TESSERACT_STUDIO_PLUGINS_ENV = "TESSERACT_STUDIO_PLUGINS";

namespace tesseract_gui
{
// const std::string StudioConfigWidgetFactory::SECTION_NAME = "Studio";

// const std::string& StudioConfigWidgetFactory::getSectionName() { return StudioConfigWidgetFactory::SECTION_NAME; }

const std::string StudioDockWidgetFactory::SECTION_NAME = "Studio";

const std::string& StudioDockWidgetFactory::getSectionName() { return StudioDockWidgetFactory::SECTION_NAME; }

StudioPluginFactory::StudioPluginFactory()
{
  plugin_loader_.search_libraries_env = TESSERACT_STUDIO_PLUGINS_ENV;
  plugin_loader_.search_paths_env = TESSERACT_STUDIO_PLUGIN_DIRECTORIES_ENV;
  plugin_loader_.search_paths.insert(TESSERACT_STUDIO_PLUGIN_PATH);
  boost::split(
      plugin_loader_.search_libraries, TESSERACT_STUDIO_PLUGINS, boost::is_any_of(":"), boost::token_compress_on);
}

StudioPluginFactory::StudioPluginFactory(const YAML::Node& config) : StudioPluginFactory() { loadConfig(config); }

StudioPluginFactory::StudioPluginFactory(const tesseract_common::fs::path& config) : StudioPluginFactory()
{
  loadConfig(config);
}

StudioPluginFactory::StudioPluginFactory(const std::string& config) : StudioPluginFactory() { loadConfig(config); }

// This prevents it from being defined inline.
// If not the forward declare of PluginLoader cause compiler error.
StudioPluginFactory::~StudioPluginFactory() = default;

void StudioPluginFactory::loadConfig(const YAML::Node& config)
{
  if (const YAML::Node& plugin_info = config[tesseract_gui::StudioPluginInfo::CONFIG_KEY])
  {
    auto studio_plugin_info = plugin_info.as<tesseract_gui::StudioPluginInfo>();
    plugin_loader_.search_paths.insert(studio_plugin_info.search_paths.begin(), studio_plugin_info.search_paths.end());
    plugin_loader_.search_libraries.insert(studio_plugin_info.search_libraries.begin(),
                                           studio_plugin_info.search_libraries.end());
    plugin_info_ = studio_plugin_info.plugin_infos;
  }
}

void StudioPluginFactory::loadConfig(const tesseract_common::fs::path& config)
{
  loadConfig(YAML::LoadFile(config.string()));
}

void StudioPluginFactory::loadConfig(const std::string& config) { loadConfig(YAML::Load(config)); }

void StudioPluginFactory::addSearchPath(const std::string& path) { plugin_loader_.search_paths.insert(path); }

std::set<std::string> StudioPluginFactory::getSearchPaths() const { return plugin_loader_.search_paths; }

void StudioPluginFactory::setSearchPaths(const std::set<std::string>& paths) { plugin_loader_.search_paths = paths; }

void StudioPluginFactory::clearSearchPaths() { plugin_loader_.search_paths.clear(); }

void StudioPluginFactory::addSearchLibrary(const std::string& library_name)
{
  plugin_loader_.search_libraries.insert(library_name);
}

std::set<std::string> StudioPluginFactory::getSearchLibraries() const { return plugin_loader_.search_libraries; }

void StudioPluginFactory::setSearchLibraries(const std::set<std::string>& library_names)
{
  plugin_loader_.search_libraries = library_names;
}

void StudioPluginFactory::clearSearchLibraries() { plugin_loader_.search_libraries.clear(); }

void StudioPluginFactory::addStudioPlugin(const std::string& name, tesseract_common::PluginInfo plugin_info)
{
  plugin_info_.plugins[name] = std::move(plugin_info);
}

bool StudioPluginFactory::hasStudioPlugins() const { return !plugin_info_.plugins.empty(); }

tesseract_common::PluginInfoMap StudioPluginFactory::getStudioPlugins() const { return plugin_info_.plugins; }

void StudioPluginFactory::removeStudioPlugin(const std::string& name)
{
  auto cm_it = plugin_info_.plugins.find(name);
  if (cm_it == plugin_info_.plugins.end())
    throw std::runtime_error("StudioPluginFactory, tried to remove studio plugin '" + name + "' that does not exist!");

  plugin_info_.plugins.erase(cm_it);

  if (plugin_info_.default_plugin == name)
    plugin_info_.default_plugin.clear();
}

// StudioPluginConfigWidget* StudioPluginFactory::createStudioConfigWidget(const std::string& name) const
//{
//  auto cm_it = plugin_info_.plugins.find(name);
//  if (cm_it == plugin_info_.plugins.end())
//  {
//    CONSOLE_BRIDGE_logWarn("StudioPluginFactory, tried to get studio config widget '%s' that does not "
//                           "exist!",
//                           name.c_str());
//    return nullptr;
//  }

//  return createStudioConfigWidget(name, cm_it->second);
//}

// StudioPluginConfigWidget*
// StudioPluginFactory::createStudioConfigWidget(const std::string& name,
//                                              const tesseract_common::PluginInfo& plugin_info) const
//{
//  try
//  {
//    auto it = factories_.find(plugin_info.class_name);
//    if (it != factories_.end())
//      return it->second->create(name, plugin_info.config);

//    auto plugin = plugin_loader_.instantiate<StudioConfigWidgetFactory>(plugin_info.class_name);
//    if (plugin == nullptr)
//    {
//      CONSOLE_BRIDGE_logWarn("Failed to load symbol '%s'", plugin_info.class_name.c_str());
//      return nullptr;
//    }
//    factories_[plugin_info.class_name] = plugin;
//    return plugin->create(name, plugin_info.config);
//  }
//  catch (const std::exception& e)
//  {
//    CONSOLE_BRIDGE_logWarn("Failed to load symbol '%s', Details: %s", plugin_info.class_name.c_str(), e.what());
//    return nullptr;
//  }
//}

// StudioDockWidget* StudioPluginFactory::createStudioDockWidget(const std::string& name) const
//{
//  auto cm_it = plugin_info_.plugins.find(name);
//  if (cm_it == plugin_info_.plugins.end())
//  {
//    CONSOLE_BRIDGE_logWarn("StudioPluginFactory, tried to get studio dock widget '%s' that does not "
//                           "exist!",
//                           name.c_str());
//    return nullptr;
//  }

//  return createStudioDockWidget(name, cm_it->second);
//}

StudioDockWidget* StudioPluginFactory::createStudioDockWidget(const QString& name,
                                                              const tesseract_common::PluginInfo& plugin_info) const
{
  try
  {
    auto it = factories_.find(plugin_info.class_name);
    if (it != factories_.end())
    {
      StudioDockWidget* dock_widget = it->second->create(name);
      if (dock_widget != nullptr)
      {
        if (!plugin_info.config.IsNull())
          dock_widget->loadConfig(plugin_info.config);
      }
      return dock_widget;
    }

    auto plugin = plugin_loader_.instantiate<StudioDockWidgetFactory>(plugin_info.class_name);
    if (plugin == nullptr)
    {
      CONSOLE_BRIDGE_logWarn("Failed to load symbol '%s'", plugin_info.class_name.c_str());
      return nullptr;
    }
    factories_[plugin_info.class_name] = plugin;

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

void StudioPluginFactory::saveConfig(const tesseract_common::fs::path& file_path) const
{
  YAML::Node config = getConfig();
  std::ofstream fout(file_path.string());
  fout << config;
}

YAML::Node StudioPluginFactory::getConfig() const
{
  tesseract_gui::StudioPluginInfo tc_plugins;
  tc_plugins.search_paths = plugin_loader_.search_paths;
  tc_plugins.search_libraries = plugin_loader_.search_libraries;
  tc_plugins.plugin_infos = plugin_info_;

  YAML::Node config;
  config[tesseract_common::TaskComposerPluginInfo::CONFIG_KEY] = tc_plugins;

  return config;
}

tesseract_common::PluginLoader& StudioPluginFactory::getPluginLoader() { return plugin_loader_; }
const tesseract_common::PluginLoader& StudioPluginFactory::getPluginLoader() const { return plugin_loader_; }

}  // namespace tesseract_gui
