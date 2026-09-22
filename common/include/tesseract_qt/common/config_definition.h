/*
 * @copyright Copyright (C) 2026 Levi Armstrong <leviarmstrong@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 */
#ifndef TESSERACT_QT_COMMON_CONFIG_DEFINITION_H
#define TESSERACT_QT_COMMON_CONFIG_DEFINITION_H

#include <tesseract/common/property_tree.h>
#include <yaml-cpp/yaml.h>

#include <QString>

#include <string>
#include <string_view>
#include <vector>

namespace tesseract::gui
{
/** @brief Declarative description of one supported configuration document. */
struct ConfigDefinition
{
  std::string id;
  QString label;
  QString title;
  tesseract::common::PropertyTree schema;
  YAML::Node empty_document;
  std::vector<std::string> default_search_paths;
  std::vector<std::string> default_search_libraries;
  std::string search_paths_environment;
  std::string search_libraries_environment;

  /** @brief Return the schema's outer YAML configuration key. */
  std::string configKey() const;
};

/** @brief Return the built-in configuration definitions. */
const std::vector<ConfigDefinition>& configDefinitions();

/**
 * @brief Find a built-in configuration definition by identifier.
 * @throws std::runtime_error if the identifier is unknown.
 */
const ConfigDefinition& configDefinition(std::string_view id);

/**
 * @brief Detect a built-in configuration definition from its outer YAML key.
 * @throws std::runtime_error if no supported key or more than one supported key is present.
 */
const ConfigDefinition& detectConfigDefinition(const YAML::Node& document);
}  // namespace tesseract::gui

#endif  // TESSERACT_QT_COMMON_CONFIG_DEFINITION_H