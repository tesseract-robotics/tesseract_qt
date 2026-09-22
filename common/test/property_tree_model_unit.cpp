/*
 * @copyright Copyright (c) 2026, Southwest Research Institute
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <tesseract_qt/common/models/property_tree_model.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract/common/schema_registry.h>
#include <tesseract/task_composer/yaml_extensions.h>
#include <QApplication>
#include <QByteArray>
#include <QBrush>
#include <QColor>
#include <QIcon>
#include <gtest/gtest.h>

namespace tesseract::gui
{
namespace
{
tesseract::common::PropertyTree createSchema()
{
  using namespace tesseract::common;
  // clang-format off
  return PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .string("name")
        .required()
        .minimumLength(1)
        .maximumLength(32)
        .label("Name")
        .placeholder("Enter a name")
        .doc("Configuration name")
        .done()
      .boolean("enabled").defaultVal(true).done()
      .int32("count").minimum(1).maximum(5).done()
      .string("mode").enumValues({ "fast", "safe" }).done()
      .string("internal").hidden().done()
      .build();
  // clang-format on
}

QModelIndex findChild(const QAbstractItemModel& model, const QString& name, const QModelIndex& parent = {})
{
  for (int row = 0; row < model.rowCount(parent); ++row)
  {
    const QModelIndex candidate = model.index(row, 0, parent);
    if (candidate.data().toString() == name)
      return candidate;
  }
  return {};
}

void expectIconsForEveryRow(const QAbstractItemModel& model, const QModelIndex& parent = {})
{
  for (int row = 0; row < model.rowCount(parent); ++row)
  {
    const QModelIndex index = model.index(row, 0, parent);
    EXPECT_FALSE(model.data(index, Qt::DecorationRole).value<QIcon>().isNull())
        << index.data(PropertyTreeModel::PathRole).toString().toStdString();
    expectIconsForEveryRow(model, index);
  }
}
}  // namespace

TEST(PropertyTreeModelUnit, LoadsMetadataAndConfiguration)  // NOLINT
{
  PropertyTreeModel model;
  model.load(createSchema(), YAML::Load("name: robot\ncount: 3\nmode: fast"));

  ASSERT_EQ(model.rowCount(), 4);
  const QModelIndex name = model.index(0, 0);
  EXPECT_EQ(model.data(name).toString(), QStringLiteral("Name"));
  EXPECT_EQ(model.data(name, PropertyTreeModel::PathRole).toString(), QStringLiteral("name"));
  EXPECT_TRUE(model.data(name, PropertyTreeModel::RequiredRole).toBool());
  EXPECT_EQ(model.data(name, PropertyTreeModel::DocumentationRole).toString(), QStringLiteral("Configuration name"));
  EXPECT_EQ(model.data(name, PropertyTreeModel::PlaceholderRole).toString(), QStringLiteral("Enter a name"));
  EXPECT_EQ(model.data(name, PropertyTreeModel::MinimumLengthRole).toInt(), 1);
  EXPECT_EQ(model.data(name, PropertyTreeModel::MaximumLengthRole).toInt(), 32);

  const QModelIndex enabled = model.index(1, 1);
  EXPECT_EQ(model.data(enabled, Qt::CheckStateRole).toInt(), Qt::Checked);
  EXPECT_TRUE(model.data(enabled, PropertyTreeModel::DefaultRole).isValid());

  const QModelIndex count = model.index(2, 1);
  EXPECT_EQ(model.data(count).toString(), QStringLiteral("3"));
  EXPECT_EQ(model.data(count, PropertyTreeModel::MinimumRole).toString(), QStringLiteral("1"));
  EXPECT_EQ(model.data(count, PropertyTreeModel::MaximumRole).toString(), QStringLiteral("5"));

  const QModelIndex mode = model.index(3, 1);
  EXPECT_EQ(model.data(mode, PropertyTreeModel::EnumValuesRole).toStringList(),
            (QStringList{ QStringLiteral("fast"), QStringLiteral("safe") }));
  EXPECT_TRUE(model.isValid());
  expectIconsForEveryRow(model);
  EXPECT_EQ(model.data(name, Qt::DecorationRole).value<QIcon>().cacheKey(), icons::getTextIcon().cacheKey());
  EXPECT_EQ(model.data(model.index(1, 0), Qt::DecorationRole).value<QIcon>().cacheKey(),
            icons::getTrueFalseIcon().cacheKey());
  EXPECT_EQ(model.data(model.index(2, 0), Qt::DecorationRole).value<QIcon>().cacheKey(),
            icons::getNumericIcon().cacheKey());
}

TEST(PropertyTreeModelUnit, EditsAndValidatesDraft)  // NOLINT
{
  PropertyTreeModel model;
  model.load(createSchema(), YAML::Load("name: robot\ncount: 3\nmode: fast"));

  const QModelIndex count = model.index(2, 1);
  EXPECT_FALSE(model.setData(count, QStringLiteral("not_an_integer")));
  EXPECT_EQ(model.yaml()["count"].as<int>(), 3);

  EXPECT_TRUE(model.setData(count, 4));
  EXPECT_EQ(model.yaml()["count"].as<int>(), 4);
  EXPECT_TRUE(model.isValid());

  const QModelIndex mode = model.index(3, 1);
  EXPECT_TRUE(model.setData(mode, QStringLiteral("unsupported")));
  EXPECT_FALSE(model.isValid());
  EXPECT_FALSE(model.data(mode, PropertyTreeModel::ValidationErrorsRole).toStringList().empty());

  EXPECT_TRUE(model.setData(mode, QStringLiteral("safe")));
  EXPECT_TRUE(model.isValid());
  EXPECT_EQ(model.yaml()["mode"].as<std::string>(), "safe");

  const QModelIndex name = model.index(0, 1);
  EXPECT_TRUE(model.setData(name, QString()));
  EXPECT_FALSE(model.isValid());
  EXPECT_FALSE(model.data(name, PropertyTreeModel::ValidationErrorsRole).toStringList().empty());
}

TEST(PropertyTreeModelUnit, RejectsStrictUnknownProperties)  // NOLINT
{
  PropertyTreeModel model;
  model.load(createSchema(), YAML::Load("name: robot\nunknown: value"));

  EXPECT_FALSE(model.isValid());
  EXPECT_NE(model.validationErrors().filter(QStringLiteral("unknown")).size(), 0);
}

TEST(PropertyTreeModelUnit, EditsDynamicMapsAndLists)  // NOLINT
{
  using namespace tesseract::common;
  // clang-format off
  const PropertyTree schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .customType("paths", property_type::createList(property_type::STRING)).done()
      .customType("values", property_type::createMap(property_type::STRING, property_type::INT32)).done()
      .build();
  // clang-format on

  PropertyTreeModel model;
  model.load(schema, YAML::Load("paths: [/a, /b]\nvalues: {first: 1}"));

  QModelIndex paths = findChild(model, "paths");
  QModelIndex values = findChild(model, "values");
  ASSERT_TRUE(paths.isValid());
  ASSERT_TRUE(values.isValid());
  EXPECT_EQ(model.data(paths, Qt::DecorationRole).value<QIcon>().cacheKey(), icons::getArrayIcon().cacheKey());
  EXPECT_EQ(model.data(values, Qt::DecorationRole).value<QIcon>().cacheKey(), icons::getKeyValueIcon().cacheKey());
  EXPECT_EQ(model.rowCount(paths), 2);
  EXPECT_EQ(model.rowCount(values), 1);
  EXPECT_EQ(model.index(paths.row(), 1).data().toString(), QStringLiteral("2 items"));
  EXPECT_EQ(model.index(values.row(), 1).data().toString(), QStringLiteral("1 entry"));

  EXPECT_TRUE(model.addListEntry(paths));
  paths = findChild(model, "paths");
  EXPECT_EQ(model.rowCount(paths), 3);
  EXPECT_TRUE(model.setData(model.index(2, 1, paths), QStringLiteral("/c")));
  EXPECT_EQ(model.yaml()["paths"][2].as<std::string>(), "/c");

  values = findChild(model, "values");
  EXPECT_TRUE(model.addMapEntry(values, QStringLiteral("second")));
  values = findChild(model, "values");
  QModelIndex second = findChild(model, "second", values);
  ASSERT_TRUE(second.isValid());
  EXPECT_TRUE(model.setData(second.sibling(second.row(), 1), 2));
  EXPECT_TRUE(model.renameMapEntry(findChild(model, "second", findChild(model, "values")), QStringLiteral("renamed")));
  EXPECT_EQ(model.yaml()["values"]["renamed"].as<int>(), 2);

  values = findChild(model, "values");
  EXPECT_TRUE(model.removeEntry(findChild(model, "first", values)));
  EXPECT_FALSE(model.yaml()["values"]["first"]);
}

TEST(PropertyTreeModelUnit, ExpandsRegisteredCustomContainerTypes)  // NOLINT
{
  using namespace tesseract::common;
  constexpr std::string_view map_type{ "test::EditorNamedMap" };
  constexpr std::string_view list_type{ "test::EditorNamedList" };
  constexpr std::string_view container_type{ "test::EditorNamedContainer" };

  auto registry = SchemaRegistry::instance();
  // clang-format off
  registry->registerSchema(
      std::string(map_type),
      PropertyTreeBuilder().attribute(property_attribute::TYPE, property_type::createMap(property_type::STRING, property_type::INT32)).build());
  registry->registerSchema(
      std::string(list_type),
      PropertyTreeBuilder().attribute(property_attribute::TYPE, property_type::createList(property_type::STRING)).build());
  registry->registerSchema(
      std::string(container_type),
      PropertyTreeBuilder()
          .attribute(property_attribute::TYPE, property_type::CONTAINER)
          .boolean("enabled").defaultVal(true).done()
          .build());
  const PropertyTree schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .customType("values", map_type).done()
      .customType("paths", list_type).done()
      .customType("settings", container_type).required().done()
      .build();
  // clang-format on

  PropertyTreeModel model;
  model.load(schema, YAML::Load("values: {first: 1}\npaths: [/a, /b]\nsettings: {}"));

  QModelIndex values = findChild(model, "values");
  QModelIndex paths = findChild(model, "paths");
  const QModelIndex settings = findChild(model, "settings");
  ASSERT_TRUE(values.isValid());
  ASSERT_TRUE(paths.isValid());
  ASSERT_TRUE(settings.isValid());
  EXPECT_EQ(model.rowCount(values), 1);
  EXPECT_EQ(model.rowCount(paths), 2);
  EXPECT_TRUE(findChild(model, "enabled", settings).isValid());
  EXPECT_EQ(model.data(values.sibling(values.row(), 1)).toString(), QStringLiteral("1 entry"));
  EXPECT_EQ(model.data(paths.sibling(paths.row(), 1)).toString(), QStringLiteral("2 items"));
  EXPECT_TRUE(model.data(values, PropertyTreeModel::DynamicContainerRole).toBool());
  EXPECT_TRUE(model.data(paths, PropertyTreeModel::DynamicContainerRole).toBool());

  EXPECT_TRUE(model.addMapEntry(values, QStringLiteral("second")));
  values = findChild(model, "values");
  QModelIndex second = findChild(model, "second", values);
  ASSERT_TRUE(second.isValid());
  EXPECT_TRUE(model.setData(second.sibling(second.row(), 1), 2));

  paths = findChild(model, "paths");
  EXPECT_TRUE(model.addListEntry(paths));
  paths = findChild(model, "paths");
  EXPECT_TRUE(model.setData(model.index(2, 1, paths), QStringLiteral("/c")));
  EXPECT_EQ(model.yaml()["values"]["second"].as<int>(), 2);
  EXPECT_EQ(model.yaml()["paths"][2].as<std::string>(), "/c");
}

TEST(PropertyTreeModelUnit, DisplaysTaskComposerPortMappingValueTypes)  // NOLINT
{
  using namespace tesseract::common;
  using namespace tesseract::task_composer;

  PropertyTree schema;
  schema.setAttribute(property_attribute::TYPE, property_type::CONTAINER);
  schema["inputs"].setAttribute(property_attribute::TYPE,
                                property_type::createMap(REQUIRED_STRING_OR_STRING_LIST_SCHEMA_KEY));

  PropertyTreeModel model;
  model.load(schema, YAML::Load("inputs: {single: input_data, multiple: [first, second]}"));

  const QModelIndex inputs = findChild(model, "inputs");
  ASSERT_TRUE(inputs.isValid());
  EXPECT_EQ(model.data(inputs, Qt::DecorationRole).value<QIcon>().cacheKey(), icons::getKeyValueIcon().cacheKey());

  const QModelIndex single = findChild(model, "single", inputs);
  ASSERT_TRUE(single.isValid());
  EXPECT_EQ(model.data(single, PropertyTreeModel::TypeRole).toString(), QStringLiteral("string"));
  EXPECT_TRUE(model.data(single, PropertyTreeModel::RequiredRole).toBool());
  EXPECT_EQ(model.data(single, Qt::DecorationRole).value<QIcon>().cacheKey(), icons::getTextIcon().cacheKey());

  const QModelIndex multiple = findChild(model, "multiple", inputs);
  ASSERT_TRUE(multiple.isValid());
  EXPECT_EQ(model.data(multiple, PropertyTreeModel::TypeRole).toString(), QStringLiteral("List[string]"));
  EXPECT_TRUE(model.data(multiple, PropertyTreeModel::RequiredRole).toBool());
  EXPECT_EQ(model.data(multiple, Qt::DecorationRole).value<QIcon>().cacheKey(), icons::getArrayIcon().cacheKey());
  ASSERT_EQ(model.rowCount(multiple), 2);
  EXPECT_EQ(model.data(model.index(0, 0, multiple), PropertyTreeModel::TypeRole).toString(), QStringLiteral("string"));
  EXPECT_EQ(model.data(model.index(0, 0, multiple), Qt::DecorationRole).value<QIcon>().cacheKey(),
            icons::getTextIcon().cacheKey());

  ASSERT_TRUE(model.addMapEntry(inputs, QStringLiteral("new_mapping")));
  QModelIndex updated_inputs = findChild(model, "inputs");
  QModelIndex new_mapping = findChild(model, "new_mapping", updated_inputs);
  ASSERT_TRUE(new_mapping.isValid());
  EXPECT_EQ(model.data(new_mapping, PropertyTreeModel::TypeRole).toString(), QStringLiteral("string"));
  EXPECT_TRUE(model.setData(new_mapping.sibling(new_mapping.row(), 1), QStringLiteral("new_value")));
  updated_inputs = findChild(model, "inputs");
  new_mapping = findChild(model, "new_mapping", updated_inputs);
  EXPECT_EQ(model.data(new_mapping, Qt::DecorationRole).value<QIcon>().cacheKey(), icons::getTextIcon().cacheKey());
}

TEST(PropertyTreeModelUnit, MovesListEntriesToRequestedIndexes)  // NOLINT
{
  using namespace tesseract::common;
  // clang-format off
  const PropertyTree schema = PropertyTreeBuilder()
      .customType("paths", property_type::createList(property_type::STRING)).done()
      .build();
  // clang-format on

  PropertyTreeModel model;
  model.load(schema, YAML::Load("paths: [a, b, c, d]"));

  QModelIndex paths = findChild(model, "paths");
  ASSERT_TRUE(model.moveListEntry(model.index(1, 0, paths), 3));
  EXPECT_EQ(model.yaml()["paths"].as<std::vector<std::string>>(), (std::vector<std::string>{ "a", "c", "d", "b" }));

  paths = findChild(model, "paths");
  ASSERT_TRUE(model.moveListEntry(model.index(3, 0, paths), 0));
  EXPECT_EQ(model.yaml()["paths"].as<std::vector<std::string>>(), (std::vector<std::string>{ "b", "a", "c", "d" }));
}

TEST(PropertyTreeModelUnit, ExpandsAndSwitchesPluginSchemas)  // NOLINT
{
  using namespace tesseract::common;
  constexpr std::string_view base_type{ "test::EditorPluginFactory" };
  constexpr std::string_view plugin_a{ "test::EditorPluginA" };
  constexpr std::string_view plugin_b{ "test::EditorPluginB" };

  auto registry = SchemaRegistry::instance();
  // clang-format off
  const PropertyTree schema_a = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .string("name").required().minimumLength(1).done()
      .build();
  const PropertyTree schema_b = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .int32("iterations").defaultVal(3).done()
      .build();
  const PropertyTree root_schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .customType("plugins", property_type::createMap(property_type::STRING, base_type))
          .acceptsDerivedTypes()
          .done()
      .build();
  // clang-format on
  registry->registerSchema(std::string(plugin_a), schema_a);
  registry->registerSchema(std::string(plugin_b), schema_b);
  registry->registerDerivedType(std::string(base_type), std::string(plugin_a));
  registry->registerDerivedType(std::string(base_type), std::string(plugin_b));

  PluginSchemaCatalog catalog;
  catalog.property_path = "plugins";
  catalog.base_type = base_type;
  catalog.schemas.emplace(plugin_a, schema_a);
  catalog.schemas.emplace(plugin_b, schema_b);

  PropertyTreeModel model;
  model.load(root_schema,
             YAML::Load("plugins:\n  solver:\n    class: test::EditorPluginA\n    config:\n      name: demo"));
  model.setPluginSchemaCatalogs({ catalog });

  QModelIndex plugins = findChild(model, "plugins");
  QModelIndex solver = findChild(model, "solver", plugins);
  QModelIndex plugin_class = findChild(model, "class", solver);
  QModelIndex config = findChild(model, "config", solver);
  ASSERT_TRUE(plugin_class.isValid());
  ASSERT_TRUE(config.isValid());
  EXPECT_EQ(model.data(plugin_class, Qt::DecorationRole).value<QIcon>().cacheKey(), icons::getPluginIcon().cacheKey());
  EXPECT_EQ(model.data(config, Qt::DecorationRole).value<QIcon>().cacheKey(), icons::getSetIcon().cacheKey());
  EXPECT_EQ(model.data(plugin_class, PropertyTreeModel::EnumValuesRole).toStringList().size(), 2);
  QModelIndex name = findChild(model, "name", config);
  ASSERT_TRUE(name.isValid());
  const qint64 valid_icon_key = model.data(name, Qt::DecorationRole).value<QIcon>().cacheKey();
  ASSERT_TRUE(model.setData(name.sibling(name.row(), 1), QString()));
  EXPECT_FALSE(model.isValid());
  EXPECT_EQ(model.data(name, Qt::ForegroundRole).value<QBrush>().color(), QColor(Qt::red));
  EXPECT_EQ(model.data(config, Qt::ForegroundRole).value<QBrush>().color(), QColor(Qt::red));
  EXPECT_FALSE(model.data(plugin_class, Qt::ForegroundRole).isValid());
  EXPECT_NE(model.data(name, Qt::DecorationRole).value<QIcon>().cacheKey(), valid_icon_key);
  expectIconsForEveryRow(model);

  EXPECT_TRUE(model.setData(plugin_class.sibling(plugin_class.row(), 1),
                            QString::fromUtf8(plugin_b.data(), static_cast<int>(plugin_b.size()))));
  EXPECT_EQ(model.yaml()["plugins"]["solver"]["class"].as<std::string>(), plugin_b);
  EXPECT_EQ(model.yaml()["plugins"]["solver"]["config"]["iterations"].as<int>(), 3);

  plugins = findChild(model, "plugins");
  solver = findChild(model, "solver", plugins);
  config = findChild(model, "config", solver);
  EXPECT_TRUE(findChild(model, "iterations", config).isValid());
  EXPECT_FALSE(findChild(model, "name", config).isValid());
}

TEST(PropertyTreeModelUnit, SelectsPluginCatalogByPropertyPath)  // NOLINT
{
  using namespace tesseract::common;
  constexpr std::string_view base_type{ "test::SharedFactory" };

  // clang-format off
  const PropertyTree schema_a = PropertyTreeBuilder()
      .string("first_value").done()
      .build();
  const PropertyTree schema_b = PropertyTreeBuilder()
      .string("second_value").done()
      .build();
  const PropertyTree root_schema = PropertyTreeBuilder()
      .pluginContainerMap("first_plugins", base_type, "FirstSection")
      .pluginContainerMap("second_plugins", base_type, "SecondSection")
      .build();
  // clang-format on

  PluginSchemaCatalog first_catalog;
  first_catalog.property_path = "first_plugins";
  first_catalog.base_type = base_type;
  first_catalog.schemas.emplace("FirstPlugin", schema_a);

  PluginSchemaCatalog second_catalog;
  second_catalog.property_path = "second_plugins";
  second_catalog.base_type = base_type;
  second_catalog.schemas.emplace("SecondPlugin", schema_b);

  PropertyTreeModel model;
  model.load(root_schema,
             YAML::Load("first_plugins:\n"
                        "  first_group:\n"
                        "    plugins:\n"
                        "      first:\n"
                        "        class: FirstPlugin\n"
                        "        config: {first_value: one}\n"
                        "second_plugins:\n"
                        "  second_group:\n"
                        "    plugins:\n"
                        "      second:\n"
                        "        class: SecondPlugin\n"
                        "        config: {second_value: two}"));
  model.setPluginSchemaCatalogs({ first_catalog, second_catalog });

  const QModelIndex first_group = findChild(model, "first_group", findChild(model, "first_plugins"));
  const QModelIndex first = findChild(model, "first", findChild(model, "plugins", first_group));
  const QModelIndex first_class = findChild(model, "class", first);
  EXPECT_EQ(model.data(first_class, PropertyTreeModel::EnumValuesRole).toStringList(),
            (QStringList{ QStringLiteral("FirstPlugin") }));
  EXPECT_TRUE(findChild(model, "first_value", findChild(model, "config", first)).isValid());

  const QModelIndex second_group = findChild(model, "second_group", findChild(model, "second_plugins"));
  const QModelIndex second = findChild(model, "second", findChild(model, "plugins", second_group));
  const QModelIndex second_class = findChild(model, "class", second);
  EXPECT_EQ(model.data(second_class, PropertyTreeModel::EnumValuesRole).toStringList(),
            (QStringList{ QStringLiteral("SecondPlugin") }));
  EXPECT_TRUE(findChild(model, "second_value", findChild(model, "config", second)).isValid());
}
}  // namespace tesseract::gui

int main(int argc, char** argv)
{
  qputenv("QT_QPA_PLATFORM", QByteArray("offscreen"));
  QApplication application(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
