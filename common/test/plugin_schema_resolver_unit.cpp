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
#include <tesseract_qt/common/models/property_tree_model.h>
#include <tesseract_qt/common/config_definition.h>

#include <tesseract/common/plugin_info.h>
#include <tesseract/common/property_tree.h>
#include <tesseract/common/schema_registry.h>
#include <tesseract/common/yaml_extensions.h>
#include <gtest/gtest.h>

namespace tesseract::gui
{
namespace
{
tesseract::common::PropertyTree createPluginSchema()
{
  using namespace tesseract::common;
  // clang-format off
  return PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .attribute(property_attribute::CONFIG_KEY, "example_plugins")
      .customType("paths", property_type::createList(property_type::STRING))
          .attribute(property_attribute::PLUGIN_DISCOVERY_ROLE, plugin_discovery_role::SEARCH_PATHS)
          .done()
      .customType("libraries", property_type::createList(property_type::STRING))
          .attribute(property_attribute::PLUGIN_DISCOVERY_ROLE, plugin_discovery_role::SEARCH_LIBRARIES)
          .done()
      .pluginContainerMap("solvers", "example::SolverFactory", "ExampleSolver")
      .build();
  // clang-format on
}

QModelIndex findByPath(const QAbstractItemModel& model, const QString& path, const QModelIndex& parent = {})
{
  for (int row = 0; row < model.rowCount(parent); ++row)
  {
    const QModelIndex index = model.index(row, 0, parent);
    if (model.data(index, PropertyTreeModel::PathRole).toString() == path)
      return index;

    const QModelIndex nested = findByPath(model, path, index);
    if (nested.isValid())
      return nested;
  }
  return {};
}
}  // namespace

TEST(PluginSchemaResolverUnit, ReadsWrappedDiscoveryConfiguration)
{
  PluginSchemaResolver resolver;
  resolver.setSearchPaths({ "/default/path", "/duplicate" });
  resolver.setSearchLibraries({});
  resolver.refresh(createPluginSchema(),
                   YAML::Load("example_plugins:\n  paths: [/duplicate, /configured/path]\n  libraries: []"));

  EXPECT_EQ(resolver.effectiveSearchPaths(),
            (std::vector<std::string>{ "/default/path", "/duplicate", "/configured/path" }));
  EXPECT_TRUE(resolver.effectiveSearchLibraries().empty());

  ASSERT_EQ(resolver.catalogs().size(), 1U);
  const auto& catalog = resolver.catalogs().front();
  EXPECT_EQ(catalog.property_path, "solvers");
  EXPECT_EQ(catalog.section, "ExampleSolver");
  EXPECT_EQ(catalog.base_type, "example::SolverFactory");
  EXPECT_TRUE(catalog.schemas.empty());
  EXPECT_EQ(resolver.findCatalog("solvers"), &catalog);
  EXPECT_EQ(resolver.findCatalog("missing"), nullptr);
}

TEST(PluginSchemaResolverUnit, PreservesCatalogOnMetadataFailure)
{
  using namespace tesseract::common;

  PluginSchemaResolver resolver;
  resolver.refresh(createPluginSchema());
  ASSERT_EQ(resolver.catalogs().size(), 1U);

  // clang-format off
  const PropertyTree invalid_schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .container("plugins")
          .attribute(property_attribute::PLUGIN_SECTION, "Incomplete")
          .done()
      .build();
  // clang-format on

  EXPECT_THROW(resolver.refresh(invalid_schema), std::runtime_error);
  ASSERT_EQ(resolver.catalogs().size(), 1U);
  EXPECT_EQ(resolver.catalogs().front().section, "ExampleSolver");
}

TEST(PluginSchemaResolverUnit, ClearRetainsConfiguredSearchInputs)
{
  PluginSchemaResolver resolver;
  resolver.setSearchPaths({ "/configured/path" });
  resolver.setSearchLibraries({});
  resolver.refresh(createPluginSchema());
  ASSERT_EQ(resolver.catalogs().size(), 1U);

  resolver.clear();
  EXPECT_TRUE(resolver.catalogs().empty());
  EXPECT_TRUE(resolver.effectiveSearchPaths().empty());

  resolver.refresh(createPluginSchema());
  EXPECT_EQ(resolver.effectiveSearchPaths(), (std::vector<std::string>{ "/configured/path" }));
}

TEST(PluginSchemaResolverUnit, ResolvesContactManagerSchemaMetadata)
{
  PluginSchemaResolver resolver;
  resolver.refresh(YAML::convert<tesseract::common::ContactManagersPluginInfo>::schema(),
                   YAML::Load("contact_manager_plugins:\n"
                              "  search_paths: [/collision/plugins]\n"
                              "  search_libraries: []\n"
                              "  discrete_plugins: {plugins: {}}\n"
                              "  continuous_plugins: {plugins: {}}"));

  EXPECT_EQ(resolver.effectiveSearchPaths(), (std::vector<std::string>{ "/collision/plugins" }));
  ASSERT_EQ(resolver.catalogs().size(), 2U);
  EXPECT_EQ(resolver.catalogs()[0].property_path, "discrete_plugins");
  EXPECT_EQ(resolver.catalogs()[0].section, "DiscColl");
  EXPECT_EQ(resolver.catalogs()[0].base_type, "tesseract::collision::DiscreteContactManagerFactory");
  EXPECT_EQ(resolver.catalogs()[1].property_path, "continuous_plugins");
  EXPECT_EQ(resolver.catalogs()[1].section, "ContColl");
  EXPECT_EQ(resolver.catalogs()[1].base_type, "tesseract::collision::ContinuousContactManagerFactory");
}

TEST(PluginSchemaResolverUnit, ResolvesTaskComposerSchemaMetadata)
{
  PluginSchemaResolver resolver;
  resolver.refresh(YAML::convert<tesseract::common::TaskComposerPluginInfo>::schema(),
                   YAML::Load("task_composer_plugins:\n"
                              "  search_paths: [/task/plugins]\n"
                              "  search_libraries: []\n"
                              "  executors: {plugins: {}}\n"
                              "  tasks: {plugins: {}}"));

  EXPECT_EQ(resolver.effectiveSearchPaths(), (std::vector<std::string>{ "/task/plugins" }));
  ASSERT_EQ(resolver.catalogs().size(), 2U);
  EXPECT_EQ(resolver.catalogs()[0].property_path, "executors");
  EXPECT_EQ(resolver.catalogs()[0].section, "TaskExec");
  EXPECT_EQ(resolver.catalogs()[0].base_type, "tesseract::task_composer::TaskComposerExecutorFactory");
  EXPECT_EQ(resolver.catalogs()[1].property_path, "tasks");
  EXPECT_EQ(resolver.catalogs()[1].section, "TaskNode");
  EXPECT_EQ(resolver.catalogs()[1].base_type, "tesseract::task_composer::TaskComposerNodeFactory");
}

TEST(PluginSchemaResolverUnit, DiscoversCollisionPluginSchemas)
{
  PluginSchemaResolver resolver;
  resolver.setSearchPaths({ TESSERACT_PLUGIN_LIBRARY_DIR });
  resolver.setSearchLibraries({ "tesseract_collision_bullet_factories", "tesseract_collision_fcl_factories" });
  resolver.refresh(YAML::convert<tesseract::common::ContactManagersPluginInfo>::schema());

  const auto* discrete = resolver.findCatalog("discrete_plugins");
  ASSERT_NE(discrete, nullptr);
  EXPECT_EQ(discrete->schemas.size(), 3U);
  EXPECT_EQ(discrete->schemas.count("BulletDiscreteBVHManagerFactory"), 1U);
  EXPECT_EQ(discrete->schemas.count("BulletDiscreteSimpleManagerFactory"), 1U);
  EXPECT_EQ(discrete->schemas.count("FCLDiscreteBVHManagerFactory"), 1U);

  const auto* continuous = resolver.findCatalog("continuous_plugins");
  ASSERT_NE(continuous, nullptr);
  EXPECT_EQ(continuous->schemas.size(), 2U);
  EXPECT_EQ(continuous->schemas.count("BulletCastBVHManagerFactory"), 1U);
  EXPECT_EQ(continuous->schemas.count("BulletCastSimpleManagerFactory"), 1U);
}

TEST(PluginSchemaResolverUnit, DiscoversTaskComposerPluginSchemas)
{
  PluginSchemaResolver resolver;
  resolver.setSearchPaths({ TESSERACT_PLANNING_PLUGIN_LIBRARY_DIR });
  resolver.setSearchLibraries({ "tesseract_task_composer_factories",
                                "tesseract_task_composer_planning_factories",
                                "tesseract_task_composer_taskflow_factories" });
  resolver.refresh(YAML::convert<tesseract::common::TaskComposerPluginInfo>::schema());

  const auto* executors = resolver.findCatalog("executors");
  ASSERT_NE(executors, nullptr);
  EXPECT_EQ(executors->schemas.count("TaskflowTaskComposerExecutorFactory"), 1U);

  const auto* tasks = resolver.findCatalog("tasks");
  ASSERT_NE(tasks, nullptr);
  EXPECT_EQ(tasks->schemas.count("DoneTaskFactory"), 1U);
  EXPECT_EQ(tasks->schemas.count("GraphTaskFactory"), 1U);
  EXPECT_EQ(tasks->schemas.count("PipelineTaskFactory"), 1U);
  EXPECT_EQ(tasks->schemas.count("OMPLMotionPlannerTaskFactory"), 1U);

  const auto& done_schema = tasks->schemas.at("DoneTaskFactory");
  EXPECT_EQ(done_schema.find("inputs"), nullptr);
  EXPECT_EQ(done_schema.find("outputs"), nullptr);

  const auto& motion_planner_schema = tasks->schemas.at("OMPLMotionPlannerTaskFactory");
  ASSERT_TRUE(motion_planner_schema.at("inputs").isRequired());
  EXPECT_TRUE(motion_planner_schema.at("inputs").at("program").isRequired());
  EXPECT_TRUE(motion_planner_schema.at("inputs").at("environment").isRequired());
  EXPECT_TRUE(motion_planner_schema.at("inputs").at("profiles").isRequired());
  ASSERT_TRUE(motion_planner_schema.at("outputs").isRequired());
  EXPECT_TRUE(motion_planner_schema.at("outputs").at("program").isRequired());

  const auto& definition = configDefinition("task-composer");
  const YAML::Node document = YAML::Load(R"(
task_composer_plugins:
  search_paths: []
  search_libraries: []
  executors: {plugins: {}}
  tasks:
    plugins:
      Planner:
        class: OMPLMotionPlannerTaskFactory
        config:
          inputs:
            program: input_program
            environment: environment
            profiles: profiles
          outputs:
            program: output_program
)");

  PropertyTreeModel model;
  model.load(definition.schema, document[definition.configKey()]);
  model.setPluginSchemaCatalogs(resolver.catalogs());
  EXPECT_TRUE(model.isValid()) << model.validationErrors().join('\n').toStdString();

  YAML::Node edited = YAML::Clone(document[definition.configKey()]);
  ASSERT_TRUE(edited["tasks"]["plugins"]["Planner"]["config"]["inputs"].remove("environment"));
  model.replaceYaml(edited);
  EXPECT_FALSE(model.isValid());

  const YAML::Node pipeline_document = YAML::Load(R"(
task_composer_plugins:
  search_paths: []
  search_libraries: []
  executors: {plugins: {}}
  tasks:
    plugins:
      Pipeline:
        class: PipelineTaskFactory
        config:
          inputs: {program: input_data}
          outputs: {program: output_data}
          nodes:
            DoneTask:
              class: DoneTaskFactory
              config: {}
          edges: []
          terminals: [DoneTask]
)");

  model.load(definition.schema, pipeline_document[definition.configKey()]);
  model.setPluginSchemaCatalogs(resolver.catalogs());
  const QModelIndex done_config =
      findByPath(model, QStringLiteral("tasks.plugins.Pipeline.config.nodes.DoneTask.config"));
  ASSERT_TRUE(done_config.isValid());
  EXPECT_FALSE(
      findByPath(model, QStringLiteral("tasks.plugins.Pipeline.config.nodes.DoneTask.config.inputs"), done_config)
          .isValid());
  EXPECT_FALSE(
      findByPath(model, QStringLiteral("tasks.plugins.Pipeline.config.nodes.DoneTask.config.outputs"), done_config)
          .isValid());
}

TEST(PluginSchemaResolverUnit, DiscoveredKinematicsSchemasPassEditorValidation)
{
  using tesseract::common::SchemaRegistry;

  const auto& definition = configDefinition("kinematics");
  const auto& schema = definition.schema;
  const auto document = YAML::LoadFile(TESSERACT_QT_TEST_FIXTURE_DIR "/kinematic_plugins.yaml");

  PluginSchemaResolver resolver;
  resolver.setSearchPaths(definition.default_search_paths);
  resolver.setSearchLibraries(definition.default_search_libraries);
  resolver.setSearchEnvironment(definition.search_paths_environment, definition.search_libraries_environment);
  ASSERT_NO_THROW(resolver.refresh(schema, document));

  const auto* inverse = resolver.findCatalog("inv_kin_plugins");
  ASSERT_NE(inverse, nullptr);
  const std::vector<std::string> aliases{
    "KDLInvKinChainLMAFactory", "OPWInvKinFactory", "URInvKinFactory", "ROPInvKinFactory", "REPInvKinFactory"
  };
  const auto registry = SchemaRegistry::instance();
  for (const auto& alias : aliases)
  {
    EXPECT_EQ(inverse->schemas.count(alias), 1U) << alias;
    EXPECT_TRUE(registry->isDerivedFrom("tesseract::kinematics::InvKinFactory", alias)) << alias;
  }

  PropertyTreeModel model;
  model.load(schema, document["kinematic_plugins"]);
  model.setPluginSchemaCatalogs(resolver.catalogs());
  EXPECT_TRUE(model.isValid()) << model.validationErrors().join('\n').toStdString();
}
}  // namespace tesseract::gui
