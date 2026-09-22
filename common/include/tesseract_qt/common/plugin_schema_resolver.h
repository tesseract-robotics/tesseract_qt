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
#ifndef TESSERACT_QT_COMMON_PLUGIN_SCHEMA_RESOLVER_H
#define TESSERACT_QT_COMMON_PLUGIN_SCHEMA_RESOLVER_H

#include <tesseract/common/property_tree.h>
#include <yaml-cpp/yaml.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace tesseract::gui
{
/** @brief Schemas discovered for one plugin-container property. */
struct PluginSchemaCatalog
{
  std::string property_path;
  std::string section;
  std::string base_type;
  std::map<std::string, tesseract::common::PropertyTree> schemas;
};

/** @brief Resolves plugin schemas using only declarative PropertyTree metadata. */
class PluginSchemaResolver
{
public:
  PluginSchemaResolver();
  ~PluginSchemaResolver();

  PluginSchemaResolver(const PluginSchemaResolver&) = delete;
  PluginSchemaResolver& operator=(const PluginSchemaResolver&) = delete;
  PluginSchemaResolver(PluginSchemaResolver&&) noexcept;
  PluginSchemaResolver& operator=(PluginSchemaResolver&&) noexcept;

  /** @brief Set application-provided search paths prepended to values found in the YAML draft. */
  void setSearchPaths(std::vector<std::string> paths);

  /** @brief Set application-provided search libraries prepended to values found in the YAML draft. */
  void setSearchLibraries(std::vector<std::string> libraries);

  /** @brief Set optional environment variables used by boost_plugin_loader. */
  void setSearchEnvironment(std::string paths_environment, std::string libraries_environment);

  /** @brief Clear resolved catalogs while retaining application-provided search settings. */
  void clear();

  /**
   * @brief Discover and validate schemas for all plugin-container nodes in a schema.
   * @details Results are committed transactionally only after every discovered alias has a compatible schema.
   * Search paths and libraries declared by plugin_discovery_role metadata are read from config.
   * @param schema Declarative configuration schema.
   * @param config Current YAML draft, either wrapped by the schema config_key or already unwrapped.
   */
  void refresh(const tesseract::common::PropertyTree& schema, const YAML::Node& config = YAML::Node());

  /** @brief Return the most recently committed plugin schema catalogs. */
  const std::vector<PluginSchemaCatalog>& catalogs() const;

  /** @brief Find a catalog by its dot-separated property path. */
  const PluginSchemaCatalog* findCatalog(const std::string& property_path) const;

  /** @brief Return the effective search paths from the most recent successful refresh. */
  const std::vector<std::string>& effectiveSearchPaths() const;

  /** @brief Return the effective search libraries from the most recent successful refresh. */
  const std::vector<std::string>& effectiveSearchLibraries() const;

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};
}  // namespace tesseract::gui

#endif  // TESSERACT_QT_COMMON_PLUGIN_SCHEMA_RESOLVER_H
