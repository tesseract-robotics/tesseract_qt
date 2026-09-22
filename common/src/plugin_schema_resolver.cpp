/*
 * @copyright Copyright (C) 2026 Levi Armstrong <levi.armstrong@gmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 */
#include <tesseract_qt/common/plugin_schema_resolver.h>

#include <tesseract/common/schema_registry.h>
#include <boost_plugin_loader/plugin_loader.hpp>

#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace tesseract::gui
{
namespace
{
using tesseract::common::PropertyTree;
using namespace tesseract::common;

struct PluginDescriptor
{
  std::string property_path;
  std::string section;
  std::string base_type;
};

void appendUnique(std::vector<std::string>& destination, const std::vector<std::string>& values)
{
  std::unordered_set<std::string> existing(destination.begin(), destination.end());
  for (const auto& value : values)
  {
    if (existing.insert(value).second)
      destination.push_back(value);
  }
}

std::vector<std::string> readStringList(const YAML::Node& node, const std::string& property_path)
{
  if (!node || node.IsNull())
    return {};

  try
  {
    return node.as<std::vector<std::string>>();
  }
  catch (const std::exception& exception)
  {
    throw std::runtime_error("PluginSchemaResolver: property '" + property_path +
                             "' must be a list of strings: " + exception.what());
  }
}

void inspectSchema(const PropertyTree& schema,
                   const YAML::Node& config,
                   const std::string& path,
                   std::vector<std::string>& search_paths,
                   std::vector<std::string>& search_libraries,
                   std::vector<PluginDescriptor>& descriptors)
{
  const auto discovery_role = schema.getAttribute(property_attribute::PLUGIN_DISCOVERY_ROLE);
  if (discovery_role.has_value())
  {
    const auto role = discovery_role->as<std::string>();
    if (role == plugin_discovery_role::SEARCH_PATHS)
      appendUnique(search_paths, readStringList(config, path));
    else if (role == plugin_discovery_role::SEARCH_LIBRARIES)
      appendUnique(search_libraries, readStringList(config, path));
    else
      throw std::runtime_error("PluginSchemaResolver: unsupported discovery role '" + role + "' at '" + path + "'");
  }

  const auto section = schema.getAttribute(property_attribute::PLUGIN_SECTION);
  const auto base_type = schema.getAttribute(property_attribute::PLUGIN_BASE_TYPE);
  if (section.has_value() != base_type.has_value())
    throw std::runtime_error("PluginSchemaResolver: incomplete plugin metadata at '" + path + "'");

  if (section.has_value())
    descriptors.push_back({ path, section->as<std::string>(), base_type->as<std::string>() });

  for (const auto& key : schema.keys())
  {
    std::string child_path = path;
    if (!child_path.empty())
      child_path += ".";
    child_path += key;
    const YAML::Node child_config = (config && config.IsMap()) ? config[key] : YAML::Node();
    inspectSchema(schema.at(key), child_config, child_path, search_paths, search_libraries, descriptors);
  }
}

YAML::Node unwrapConfig(const PropertyTree& schema, const YAML::Node& config)
{
  const auto config_key = schema.getAttribute(property_attribute::CONFIG_KEY);
  if (!config_key.has_value() || !config || !config.IsMap())
    return config;

  const auto key = config_key->as<std::string>();
  return config[key] ? config[key] : config;
}
}  // namespace

struct PluginSchemaResolver::Implementation
{
  std::vector<std::string> search_paths;
  std::vector<std::string> search_libraries;
  std::string search_paths_environment;
  std::string search_libraries_environment;
  boost_plugin_loader::PluginLoader loader;
  std::vector<PluginSchemaCatalog> catalogs;
};

PluginSchemaResolver::PluginSchemaResolver() : data_(std::make_unique<Implementation>()) {}

PluginSchemaResolver::~PluginSchemaResolver() = default;
PluginSchemaResolver::PluginSchemaResolver(PluginSchemaResolver&&) noexcept = default;
PluginSchemaResolver& PluginSchemaResolver::operator=(PluginSchemaResolver&&) noexcept = default;

void PluginSchemaResolver::setSearchPaths(std::vector<std::string> paths) { data_->search_paths = std::move(paths); }

void PluginSchemaResolver::setSearchLibraries(std::vector<std::string> libraries)
{
  data_->search_libraries = std::move(libraries);
}

void PluginSchemaResolver::setSearchEnvironment(std::string paths_environment, std::string libraries_environment)
{
  data_->search_paths_environment = std::move(paths_environment);
  data_->search_libraries_environment = std::move(libraries_environment);
}

void PluginSchemaResolver::clear()
{
  data_->loader = {};
  data_->catalogs.clear();
}

void PluginSchemaResolver::refresh(const PropertyTree& schema, const YAML::Node& config)
{
  boost_plugin_loader::PluginLoader candidate_loader;
  candidate_loader.search_paths = data_->search_paths;
  candidate_loader.search_libraries = data_->search_libraries;
  candidate_loader.search_paths_env = data_->search_paths_environment;
  candidate_loader.search_libraries_env = data_->search_libraries_environment;

  std::vector<PluginDescriptor> descriptors;
  inspectSchema(schema,
                unwrapConfig(schema, config),
                "",
                candidate_loader.search_paths,
                candidate_loader.search_libraries,
                descriptors);

  auto registry = SchemaRegistry::instance();
  const char* environment_libraries = candidate_loader.search_libraries_env.empty() ?
                                          nullptr :
                                          std::getenv(candidate_loader.search_libraries_env.c_str());
  const bool has_plugin_libraries = !candidate_loader.search_libraries.empty() ||
                                    (environment_libraries != nullptr && environment_libraries[0] != '\0');
  if (has_plugin_libraries)
    registry->loadAndRetainPluginLibraries(candidate_loader);

  std::vector<PluginSchemaCatalog> candidate_catalogs;
  candidate_catalogs.reserve(descriptors.size());
  for (const auto& descriptor : descriptors)
  {
    PluginSchemaCatalog catalog{ descriptor.property_path, descriptor.section, descriptor.base_type, {} };
    std::vector<std::string> aliases;
    if (has_plugin_libraries)
      aliases = candidate_loader.getAvailablePlugins(descriptor.section);
    std::sort(aliases.begin(), aliases.end());
    aliases.erase(std::unique(aliases.begin(), aliases.end()), aliases.end());

    for (const auto& alias : aliases)
    {
      if (!registry->isDerivedFrom(descriptor.base_type, alias))
      {
        throw std::runtime_error("PluginSchemaResolver: plugin '" + alias + "' from section '" + descriptor.section +
                                 "' is not registered as a derived type of '" + descriptor.base_type + "'");
      }
      if (!registry->contains(alias))
        throw std::runtime_error("PluginSchemaResolver: plugin '" + alias + "' does not have a registered schema");

      catalog.schemas.emplace(alias, registry->get(alias));
    }

    candidate_catalogs.push_back(std::move(catalog));
  }

  data_->loader = std::move(candidate_loader);
  data_->catalogs = std::move(candidate_catalogs);
}

const std::vector<PluginSchemaCatalog>& PluginSchemaResolver::catalogs() const { return data_->catalogs; }

const PluginSchemaCatalog* PluginSchemaResolver::findCatalog(const std::string& property_path) const
{
  const auto it = std::find_if(data_->catalogs.begin(), data_->catalogs.end(), [&property_path](const auto& catalog) {
    return catalog.property_path == property_path;
  });
  return (it == data_->catalogs.end()) ? nullptr : &(*it);
}

const std::vector<std::string>& PluginSchemaResolver::effectiveSearchPaths() const
{
  return data_->loader.search_paths;
}

const std::vector<std::string>& PluginSchemaResolver::effectiveSearchLibraries() const
{
  return data_->loader.search_libraries;
}
}  // namespace tesseract::gui
