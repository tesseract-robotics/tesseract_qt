/*
 * @copyright Copyright (C) 2026 Levi Armstrong <leviarmstrong@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 */
#include <tesseract_qt/common/models/property_tree_model.h>
#include <tesseract_qt/common/config_definition.h>
#include <tesseract_qt/common/widgets/config_editor_widget.h>

#include <tesseract/common/property_tree.h>

#include <QApplication>
#include <QByteArray>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QUndoStack>
#include <gtest/gtest.h>

namespace tesseract::gui
{
namespace
{
ConfigDefinition createSimpleDefinition()
{
  using namespace tesseract::common;
  // clang-format off
  return { "simple",
           QStringLiteral("Simple"),
           QStringLiteral("Simple Editor"),
           PropertyTreeBuilder()
               .attribute(property_attribute::TYPE, property_type::CONTAINER)
               .attribute(property_attribute::CONFIG_KEY, "simple_plugins")
               .int32("count").required().minimum(1).done()
               .build(),
           YAML::Load("simple_plugins:\n  count: 1"),
           {},
           {},
           {},
           {} };
  // clang-format on
}

QModelIndex findValueIndex(const PropertyTreeModel& model, const QString& key)
{
  for (int row = 0; row < model.rowCount(); ++row)
  {
    if (model.index(row, 0).data().toString() == key)
      return model.index(row, 1);
  }
  return {};
}
}  // namespace

TEST(ConfigEditorWidgetUnit, ProvidesAndDetectsBuiltInDefinitions)
{
  const auto& definitions = configDefinitions();
  ASSERT_EQ(definitions.size(), 3U);
  EXPECT_EQ(configDefinition("kinematics").configKey(), "kinematic_plugins");
  EXPECT_EQ(configDefinition("collision").configKey(), "contact_manager_plugins");
  EXPECT_EQ(configDefinition("task-composer").configKey(), "task_composer_plugins");

  EXPECT_EQ(detectConfigDefinition(YAML::Load("contact_manager_plugins: {}")).id, "collision");
  EXPECT_THROW(detectConfigDefinition(YAML::Load("unsupported: {}")), std::runtime_error);
  EXPECT_THROW(detectConfigDefinition(YAML::Load("kinematic_plugins: {}\ncontact_manager_plugins: {}")),
               std::runtime_error);
}

TEST(ConfigEditorWidgetUnit, TracksDocumentStateAndSavesAtomically)
{
  ConfigEditorWidget widget;
  const auto definition = createSimpleDefinition();
  widget.newConfiguration(definition);

  ASSERT_TRUE(widget.hasDocument());
  ASSERT_NE(widget.definition(), nullptr);
  EXPECT_EQ(widget.definition()->id, "simple");
  EXPECT_FALSE(widget.isModified());
  EXPECT_TRUE(widget.isValid());
  EXPECT_TRUE(widget.isSaveReady());
  EXPECT_EQ(widget.pluginSchemaState(), PropertyTreeEditorWidget::PluginSchemaState::NotRequired);

  const QModelIndex count = findValueIndex(*widget.editor()->model(), QStringLiteral("count"));
  ASSERT_TRUE(count.isValid());
  ASSERT_TRUE(widget.editor()->model()->setData(count, 2));
  EXPECT_TRUE(widget.isModified());
  widget.editor()->undoStack()->undo();
  EXPECT_FALSE(widget.isModified());
  widget.editor()->undoStack()->redo();
  EXPECT_TRUE(widget.isModified());

  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  const QString path = directory.filePath(QStringLiteral("configuration.yaml"));
  QString error;
  ASSERT_TRUE(widget.save(path, &error)) << error.toStdString();
  EXPECT_TRUE(QFileInfo::exists(path));
  EXPECT_EQ(widget.sourcePath(), path);
  EXPECT_FALSE(widget.isModified());
  EXPECT_EQ(YAML::LoadFile(path.toStdString())["simple_plugins"]["count"].as<int>(), 2);

  ASSERT_TRUE(widget.editor()->model()->setData(findValueIndex(*widget.editor()->model(), QStringLiteral("count")), 0));
  EXPECT_FALSE(widget.isValid());
  EXPECT_FALSE(widget.isSaveReady());
  EXPECT_FALSE(widget.save(path, &error));

  const QString draft_path = directory.filePath(QStringLiteral("draft.yaml"));
  ASSERT_TRUE(widget.exportDraft(draft_path, &error)) << error.toStdString();
  EXPECT_EQ(YAML::LoadFile(draft_path.toStdString())["simple_plugins"]["count"].as<int>(), 0);
}

TEST(ConfigEditorWidgetUnit, KeepsDocumentEditableWhenPluginDiscoveryFails)
{
  using namespace tesseract::common;
  // clang-format off
  ConfigDefinition definition{
    "failing-discovery",
    QStringLiteral("Failing discovery"),
    QStringLiteral("Failing discovery editor"),
    PropertyTreeBuilder()
        .attribute(property_attribute::TYPE, property_type::CONTAINER)
        .attribute(property_attribute::CONFIG_KEY, "example_plugins")
        .customType("search_libraries", property_type::createList(property_type::STRING))
          .attribute(property_attribute::PLUGIN_DISCOVERY_ROLE, "unsupported")
          .done()
        .pluginContainer("plugins", "example::PluginFactory", "ExamplePlugin")
        .build(),
    YAML::Load("example_plugins:\n"
               "  search_libraries: []\n"
               "  plugins: {plugins: {}}"),
    {},
    {},
    {},
    {}
  };
  // clang-format on

  ConfigEditorWidget widget;
  widget.newConfiguration(definition);

  EXPECT_TRUE(widget.hasDocument());
  EXPECT_EQ(widget.pluginSchemaState(), PropertyTreeEditorWidget::PluginSchemaState::Failed);
  EXPECT_FALSE(widget.pluginSchemaError().isEmpty());
  EXPECT_FALSE(widget.isSaveReady());
  EXPECT_TRUE(widget.yaml()["example_plugins"]);
  EXPECT_TRUE(widget.yaml()["example_plugins"]["plugins"]);

  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  QString error;
  EXPECT_TRUE(widget.exportDraft(directory.filePath(QStringLiteral("failed-discovery.yaml")), &error));
}

TEST(ConfigEditorWidgetUnit, LoadsRealKinematicsFixtureWithBuiltInDefaults)
{
  ConfigEditorWidget widget;
  const auto& definition = configDefinition("kinematics");
  const auto document = YAML::LoadFile(TESSERACT_QT_TEST_FIXTURE_DIR "/kinematic_plugins.yaml");

  ASSERT_NO_THROW(widget.loadConfiguration(definition, document));
  EXPECT_EQ(widget.pluginSchemaState(), PropertyTreeEditorWidget::PluginSchemaState::Current);
  EXPECT_TRUE(widget.isValid()) << widget.validationErrors().join('\n').toStdString();
  EXPECT_TRUE(widget.isSaveReady());
}
}  // namespace tesseract::gui

int main(int argc, char** argv)
{
  qputenv("QT_QPA_PLATFORM", QByteArray("offscreen"));
  QApplication application(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}