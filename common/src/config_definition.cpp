/*
 * @copyright Copyright (C) 2026 Levi Armstrong <leviarmstrong@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 */
#include <tesseract_qt/common/config_definition.h>

#include <tesseract/common/plugin_info.h>
#include <tesseract/common/yaml_extensions.h>
#include <tesseract/collision/contact_managers_plugin_factory.h>
#include <tesseract/kinematics/kinematics_plugin_factory.h>
#include <tesseract/task_composer/task_composer_plugin_factory.h>

#include <QObject>

#include <algorithm>
#include <stdexcept>

namespace tesseract::gui
{
std::string ConfigDefinition::configKey() const
{
  const auto config_key = schema.getAttribute(tesseract::common::property_attribute::CONFIG_KEY);
  if (!config_key.has_value())
    throw std::runtime_error("Configuration definition '" + id + "' does not declare a config_key");
  return config_key->as<std::string>();
}

const std::vector<ConfigDefinition>& configDefinitions()
{
  static const std::vector<ConfigDefinition> definitions = []() {
    const tesseract::kinematics::KinematicsPluginFactory kinematics_factory;
    const tesseract::collision::ContactManagersPluginFactory collision_factory;
    const tesseract::task_composer::TaskComposerPluginFactory task_composer_factory;
    return std::vector<ConfigDefinition>{ { "kinematics",
                                            QObject::tr("Kinematics"),
                                            QObject::tr("Tesseract Kinematics Configuration Editor"),
                                            YAML::convert<tesseract::common::KinematicsPluginInfo>::schema(),
                                            YAML::Load("kinematic_plugins:\n"
                                                       "  search_paths: []\n"
                                                       "  search_libraries: []\n"
                                                       "  fwd_kin_plugins: {}\n"
                                                       "  inv_kin_plugins: {}"),
                                            kinematics_factory.getSearchPaths(),
                                            kinematics_factory.getSearchLibraries(),
                                            "TESSERACT_KINEMATICS_PLUGIN_DIRECTORIES",
                                            "TESSERACT_KINEMATICS_PLUGINS" },
                                          { "collision",
                                            QObject::tr("Contact Managers"),
                                            QObject::tr("Tesseract Contact Manager Configuration Editor"),
                                            YAML::convert<tesseract::common::ContactManagersPluginInfo>::schema(),
                                            YAML::Load("contact_manager_plugins:\n"
                                                       "  search_paths: []\n"
                                                       "  search_libraries: []\n"
                                                       "  discrete_plugins: {plugins: {}}\n"
                                                       "  continuous_plugins: {plugins: {}}"),
                                            collision_factory.getSearchPaths(),
                                            collision_factory.getSearchLibraries(),
                                            "TESSERACT_CONTACT_MANAGERS_PLUGIN_DIRECTORIES",
                                            "TESSERACT_CONTACT_MANAGERS_PLUGINS" },
                                          { "task-composer",
                                            QObject::tr("Task Composer"),
                                            QObject::tr("Tesseract Task Composer Configuration Editor"),
                                            YAML::convert<tesseract::common::TaskComposerPluginInfo>::schema(),
                                            YAML::Load("task_composer_plugins:\n"
                                                       "  search_paths: []\n"
                                                       "  search_libraries: []\n"
                                                       "  executors: {plugins: {}}\n"
                                                       "  tasks: {plugins: {}}"),
                                            task_composer_factory.getSearchPaths(),
                                            task_composer_factory.getSearchLibraries(),
                                            "TESSERACT_TASK_COMPOSER_PLUGIN_DIRECTORIES",
                                            "TESSERACT_TASK_COMPOSER_PLUGINS" } };
  }();
  return definitions;
}

const ConfigDefinition& configDefinition(std::string_view id)
{
  const auto& definitions = configDefinitions();
  const auto definition =
      std::find_if(definitions.begin(), definitions.end(), [id](const auto& candidate) { return candidate.id == id; });
  if (definition == definitions.end())
    throw std::runtime_error("Unknown configuration type '" + std::string(id) + "'");
  return *definition;
}

const ConfigDefinition& detectConfigDefinition(const YAML::Node& document)
{
  if (!document || !document.IsMap())
    throw std::runtime_error("Configuration document must be a YAML map");

  const ConfigDefinition* match{ nullptr };
  for (const auto& definition : configDefinitions())
  {
    if (!document[definition.configKey()])
      continue;
    if (match != nullptr)
      throw std::runtime_error("Configuration document contains multiple supported top-level keys");
    match = &definition;
  }

  if (match == nullptr)
    throw std::runtime_error("Configuration document does not contain a supported top-level key");
  return *match;
}
}  // namespace tesseract::gui