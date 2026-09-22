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
#include <tesseract_qt/common/models/property_tree_model.h>
#include <tesseract_qt/common/widgets/property_tree_editor_widget.h>
#include <tesseract_qt/common/widgets/property_tree_item_delegate.h>

#include <tesseract/common/property_tree.h>

#include <QApplication>
#include <QByteArray>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QToolButton>
#include <QTreeView>
#include <QUndoStack>
#include <gtest/gtest.h>

#include <algorithm>

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
      .string("name").required().minimumLength(1).maximumLength(16).placeholder("Enter a name").done()
      .boolean("enabled").defaultVal(true).done()
      .int32("count").minimum(1).maximum(10).done()
      .float64("ratio").minimum(0.25).maximum(4.0).done()
      .string("mode").enumValues({ "fast", "safe" }).done()
      .customType("payload", "example::Payload").done()
      .build();
  // clang-format on
}

QModelIndex valueIndex(const PropertyTreeModel& model, const QString& name)
{
  for (int row = 0; row < model.rowCount(); ++row)
  {
    if (model.index(row, 0).data().toString() == name)
      return model.index(row, 1);
  }

  return {};
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
}  // namespace

TEST(PropertyTreeEditorWidgetUnit, CreatesSchemaAwareEditors)
{
  PropertyTreeModel model;
  model.load(createSchema(), YAML::Load("name: demo\nenabled: true\ncount: 3\nratio: 1.5\nmode: safe\npayload: raw"));

  QWidget parent;
  PropertyTreeItemDelegate delegate;
  QStyleOptionViewItem option;

  std::unique_ptr<QWidget> name_editor(delegate.createEditor(&parent, option, valueIndex(model, "name")));
  auto* name_line_edit = qobject_cast<QLineEdit*>(name_editor.get());
  ASSERT_NE(name_line_edit, nullptr);
  EXPECT_EQ(name_line_edit->placeholderText(), QStringLiteral("Enter a name"));
  EXPECT_EQ(name_line_edit->maxLength(), 16);

  EXPECT_EQ(delegate.createEditor(&parent, option, valueIndex(model, "enabled")), nullptr);

  std::unique_ptr<QWidget> count_editor(delegate.createEditor(&parent, option, valueIndex(model, "count")));
  auto* count_spin_box = qobject_cast<QSpinBox*>(count_editor.get());
  ASSERT_NE(count_spin_box, nullptr);
  EXPECT_EQ(count_spin_box->minimum(), 1);
  EXPECT_EQ(count_spin_box->maximum(), 10);

  std::unique_ptr<QWidget> ratio_editor(delegate.createEditor(&parent, option, valueIndex(model, "ratio")));
  auto* ratio_spin_box = qobject_cast<QDoubleSpinBox*>(ratio_editor.get());
  ASSERT_NE(ratio_spin_box, nullptr);
  EXPECT_DOUBLE_EQ(ratio_spin_box->minimum(), 0.25);
  EXPECT_DOUBLE_EQ(ratio_spin_box->maximum(), 4.0);

  std::unique_ptr<QWidget> mode_editor(delegate.createEditor(&parent, option, valueIndex(model, "mode")));
  auto* mode_combo_box = qobject_cast<QComboBox*>(mode_editor.get());
  ASSERT_NE(mode_combo_box, nullptr);
  EXPECT_EQ(mode_combo_box->count(), 2);

  std::unique_ptr<QWidget> payload_editor(delegate.createEditor(&parent, option, valueIndex(model, "payload")));
  EXPECT_NE(qobject_cast<QPlainTextEdit*>(payload_editor.get()), nullptr);
}

TEST(PropertyTreeEditorWidgetUnit, ForwardsChangesAndDisplaysValidation)
{
  PropertyTreeEditorWidget editor;
  int configuration_changed_count{ 0 };
  int validation_changed_count{ 0 };
  QObject::connect(&editor, &PropertyTreeEditorWidget::configurationChanged, [&configuration_changed_count]() {
    ++configuration_changed_count;
  });
  QObject::connect(&editor,
                   &PropertyTreeEditorWidget::validationChanged,
                   [&validation_changed_count](bool, const QStringList&) { ++validation_changed_count; });

  editor.load(createSchema(), YAML::Load("name: demo\ncount: 3\nratio: 1.5\nmode: safe\npayload: raw"));
  EXPECT_TRUE(editor.isValid());
  EXPECT_EQ(validation_changed_count, 1);

  ASSERT_TRUE(editor.model()->setData(valueIndex(*editor.model(), "count"), 0, Qt::EditRole));
  EXPECT_EQ(configuration_changed_count, 1);
  EXPECT_EQ(validation_changed_count, 2);
  EXPECT_FALSE(editor.isValid());

  const auto error_panels = editor.findChildren<QListWidget*>();
  ASSERT_EQ(error_panels.size(), 1);
  EXPECT_FALSE(error_panels.front()->isHidden());
  EXPECT_GT(error_panels.front()->count(), 0);
  EXPECT_TRUE(error_panels.front()->wordWrap());
  EXPECT_EQ(error_panels.front()->horizontalScrollBarPolicy(), Qt::ScrollBarAlwaysOff);

  const auto* diagnostics = editor.findChild<QFrame*>(QStringLiteral("validationDiagnostics"));
  const auto* diagnostics_header = editor.findChild<QLabel*>(QStringLiteral("validationDiagnosticsHeader"));
  ASSERT_NE(diagnostics, nullptr);
  ASSERT_NE(diagnostics_header, nullptr);
  EXPECT_FALSE(diagnostics->isHidden());
  EXPECT_LE(diagnostics->maximumHeight(), 200);
  EXPECT_EQ(diagnostics_header->sizePolicy().verticalPolicy(), QSizePolicy::Fixed);
  EXPECT_TRUE(diagnostics_header->text().contains(QStringLiteral("1 validation issue")));
  EXPECT_TRUE(error_panels.front()->item(0)->text().contains('\n'));
  EXPECT_FALSE(error_panels.front()->item(0)->toolTip().isEmpty());
}

TEST(PropertyTreeEditorWidgetUnit, SupportsUndoRedoForScalarAndStructuralEdits)
{
  using namespace tesseract::common;
  // clang-format off
  const PropertyTree schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .int32("count").done()
      .customType("paths", property_type::createList(property_type::STRING)).done()
      .build();
  // clang-format on

  PropertyTreeEditorWidget editor;
  editor.load(schema, YAML::Load("count: 1\npaths: [/a]"));
  auto* undo_button = editor.findChild<QToolButton*>(QStringLiteral("undoButton"));
  auto* redo_button = editor.findChild<QToolButton*>(QStringLiteral("redoButton"));
  ASSERT_NE(undo_button, nullptr);
  ASSERT_NE(redo_button, nullptr);
  ASSERT_TRUE(editor.model()->setData(valueIndex(*editor.model(), "count"), 2));
  ASSERT_TRUE(editor.undoStack()->canUndo());
  EXPECT_EQ(editor.yaml()["count"].as<int>(), 2);
  EXPECT_EQ(undo_button->text(), QStringLiteral("Undo"));
  EXPECT_TRUE(undo_button->toolTip().contains(QStringLiteral("Edit count")));

  editor.undoStack()->undo();
  EXPECT_EQ(editor.yaml()["count"].as<int>(), 1);
  EXPECT_EQ(redo_button->text(), QStringLiteral("Redo"));
  EXPECT_TRUE(redo_button->toolTip().contains(QStringLiteral("Edit count")));
  editor.undoStack()->redo();
  EXPECT_EQ(editor.yaml()["count"].as<int>(), 2);

  const QModelIndex paths = findChild(*editor.model(), QStringLiteral("paths"));
  ASSERT_TRUE(editor.model()->addListEntry(paths));
  EXPECT_EQ(editor.yaml()["paths"].size(), 2U);
  editor.undoStack()->undo();
  EXPECT_EQ(editor.yaml()["paths"].size(), 1U);
  editor.undoStack()->redo();
  EXPECT_EQ(editor.yaml()["paths"].size(), 2U);
}

TEST(PropertyTreeEditorWidgetUnit, ShowsSchemaDetailsAndNormalizedYamlPreview)
{
  using namespace tesseract::common;
  // clang-format off
  const PropertyTree schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .attribute(property_attribute::CONFIG_KEY, "example")
      .string("name")
        .label("Display Name")
        .group("General")
        .doc("Name used by the example.")
        .required()
        .done()
      .build();
  // clang-format on

  PropertyTreeEditorWidget editor;
  editor.load(schema, YAML::Load("example: {name: initial}"));
  ASSERT_TRUE(editor.focusPath(QStringLiteral("name")));

  const auto* details = editor.findChild<QLabel*>(QStringLiteral("propertyDetails"));
  ASSERT_NE(details, nullptr);
  EXPECT_TRUE(details->text().contains(QStringLiteral("Display Name")));
  EXPECT_TRUE(details->text().contains(QStringLiteral("Name used by the example.")));
  EXPECT_TRUE(details->text().contains(QStringLiteral("General")));

  auto* preview = editor.findChild<QPlainTextEdit*>(QStringLiteral("yamlPreview"));
  ASSERT_NE(preview, nullptr);
  EXPECT_TRUE(preview->toPlainText().contains(QStringLiteral("example:")));
  EXPECT_TRUE(preview->toPlainText().contains(QStringLiteral("name: initial")));

  ASSERT_TRUE(editor.model()->setData(editor.model()->index(0, 1), QStringLiteral("updated")));
  EXPECT_TRUE(preview->toPlainText().contains(QStringLiteral("name: updated")));
}

TEST(PropertyTreeEditorWidgetUnit, FiltersRecursivelyAndNavigatesDiagnostics)
{
  PropertyTreeEditorWidget editor;
  editor.load(createSchema(), YAML::Load("name: demo\ncount: 0\nratio: 1.5\nmode: safe\npayload: raw"));
  auto* tree_view = editor.findChild<QTreeView*>();
  auto* diagnostics = editor.findChild<QListWidget*>();
  ASSERT_NE(tree_view, nullptr);
  ASSERT_NE(diagnostics, nullptr);
  ASSERT_GT(diagnostics->count(), 0);

  editor.setFilterText(QStringLiteral("count"));
  EXPECT_EQ(editor.filterText(), QStringLiteral("count"));
  EXPECT_EQ(tree_view->model()->rowCount(), 1);

  ASSERT_TRUE(editor.focusPath(QStringLiteral("name")));
  EXPECT_TRUE(editor.filterText().isEmpty());
  EXPECT_EQ(tree_view->currentIndex().data(PropertyTreeModel::PathRole).toString(), QStringLiteral("name"));

  diagnostics->itemClicked(diagnostics->item(0));
  EXPECT_EQ(tree_view->currentIndex().data(PropertyTreeModel::PathRole).toString(), QStringLiteral("count"));
}

TEST(PropertyTreeEditorWidgetUnit, RefreshesSchemaDeclaredPluginCatalogs)
{
  using namespace tesseract::common;
  // clang-format off
  const PropertyTree schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .attribute(property_attribute::CONFIG_KEY, "example_plugins")
      .pluginContainerMap("solvers", "example::SolverFactory", "ExampleSolver")
      .build();
  // clang-format on

  PropertyTreeEditorWidget editor;
  int refresh_count{ 0 };
  QObject::connect(&editor, &PropertyTreeEditorWidget::pluginSchemasChanged, [&refresh_count]() { ++refresh_count; });
  editor.load(schema, YAML::Load("example_plugins:\n  solvers: {}"));
  ASSERT_TRUE(editor.yaml()["example_plugins"]);
  EXPECT_TRUE(editor.yaml()["example_plugins"]["solvers"].IsMap());

  const auto refresh_buttons = editor.findChildren<QPushButton*>();
  const auto refresh_button = std::find_if(refresh_buttons.begin(), refresh_buttons.end(), [](const auto* button) {
    return button->text() == QStringLiteral("Refresh Plugins");
  });
  ASSERT_NE(refresh_button, refresh_buttons.end());
  EXPECT_FALSE((*refresh_button)->isHidden());

  editor.refreshPluginSchemas();
  EXPECT_EQ(refresh_count, 1);
  ASSERT_EQ(editor.pluginSchemaCatalogs().size(), 1U);
  EXPECT_EQ(editor.pluginSchemaCatalogs().front().property_path, "solvers");
  EXPECT_EQ(editor.pluginSchemaCatalogs().front().section, "ExampleSolver");

  editor.load(createSchema(), YAML::Load("name: replacement"));
  EXPECT_TRUE(editor.pluginSchemaCatalogs().empty());
  EXPECT_TRUE((*refresh_button)->isHidden());
}

TEST(PropertyTreeEditorWidgetUnit, KeepsDraftWhenInitialPluginRefreshFails)
{
  using namespace tesseract::common;
  // clang-format off
  const PropertyTree schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .attribute(property_attribute::CONFIG_KEY, "example_plugins")
      .customType("libraries", property_type::createList(property_type::STRING))
        .attribute(property_attribute::PLUGIN_DISCOVERY_ROLE, "unsupported")
        .done()
      .pluginContainer("plugins", "example::PluginFactory", "ExamplePlugin")
      .build();
  // clang-format on

  PropertyTreeEditorWidget editor;
  int failure_count{ 0 };
  QObject::connect(&editor, &PropertyTreeEditorWidget::pluginSchemaRefreshFailed, [&failure_count](const QString&) {
    ++failure_count;
  });

  EXPECT_NO_THROW(editor.load(schema,
                              YAML::Load("example_plugins:\n"
                                         "  libraries: []\n"
                                         "  plugins: {plugins: {}}")));
  EXPECT_EQ(failure_count, 1);
  EXPECT_EQ(editor.pluginSchemaState(), PropertyTreeEditorWidget::PluginSchemaState::Failed);
  EXPECT_FALSE(editor.pluginSchemaError().isEmpty());
  EXPECT_TRUE(editor.yaml()["example_plugins"]["libraries"].IsSequence());
}

TEST(PropertyTreeEditorWidgetUnit, MarksPluginSchemasStaleOnlyForDiscoveryInputChanges)
{
  using namespace tesseract::common;
  // clang-format off
  const PropertyTree schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .string("name").done()
      .customType("paths", property_type::createList(property_type::STRING))
        .attribute(property_attribute::PLUGIN_DISCOVERY_ROLE, plugin_discovery_role::SEARCH_PATHS)
        .done()
      .pluginContainer("plugins", "example::PluginFactory", "ExamplePlugin")
      .build();
  // clang-format on

  PropertyTreeEditorWidget editor;
  editor.load(schema, YAML::Load("name: original\npaths: [/a]\nplugins: {plugins: {}}"));
  ASSERT_EQ(editor.pluginSchemaState(), PropertyTreeEditorWidget::PluginSchemaState::Current);

  ASSERT_TRUE(editor.model()->setData(valueIndex(*editor.model(), "name"), QStringLiteral("updated")));
  EXPECT_EQ(editor.pluginSchemaState(), PropertyTreeEditorWidget::PluginSchemaState::Current);

  const QModelIndex paths = findChild(*editor.model(), QStringLiteral("paths"));
  ASSERT_TRUE(paths.isValid());
  const QModelIndex first_path = editor.model()->index(0, 1, paths);
  ASSERT_TRUE(first_path.isValid());
  ASSERT_TRUE(editor.model()->setData(first_path, QStringLiteral("/updated")));
  EXPECT_EQ(editor.pluginSchemaState(), PropertyTreeEditorWidget::PluginSchemaState::Stale);
}

TEST(PropertyTreeEditorWidgetUnit, PreservesTreeStateAcrossDynamicEdits)
{
  using namespace tesseract::common;
  // clang-format off
  const PropertyTree schema = PropertyTreeBuilder()
      .attribute(property_attribute::TYPE, property_type::CONTAINER)
      .customType("paths", property_type::createList(property_type::STRING)).done()
      .build();
  // clang-format on

  PropertyTreeEditorWidget editor;
  editor.load(schema, YAML::Load("paths: [/a, /b]"));
  auto* tree_view = editor.findChild<QTreeView*>();
  ASSERT_NE(tree_view, nullptr);

  QModelIndex paths = findChild(*editor.model(), QStringLiteral("paths"));
  ASSERT_TRUE(paths.isValid());
  QModelIndex view_paths = findChild(*tree_view->model(), QStringLiteral("paths"));
  ASSERT_TRUE(view_paths.isValid());
  tree_view->setExpanded(view_paths, true);
  tree_view->setCurrentIndex(tree_view->model()->index(0, 1, view_paths));

  int reset_count{ 0 };
  QObject::connect(editor.model(), &QAbstractItemModel::modelReset, [&reset_count]() { ++reset_count; });
  ASSERT_TRUE(editor.model()->setData(editor.model()->index(0, 1, paths), QStringLiteral("/updated")));
  EXPECT_EQ(reset_count, 0);
  EXPECT_TRUE(tree_view->isExpanded(view_paths));

  ASSERT_TRUE(editor.model()->addListEntry(paths));
  EXPECT_EQ(reset_count, 1);
  view_paths = findChild(*tree_view->model(), QStringLiteral("paths"));
  EXPECT_TRUE(tree_view->isExpanded(view_paths));
  EXPECT_EQ(tree_view->currentIndex().data(PropertyTreeModel::PathRole).toString(), QStringLiteral("paths[0]"));
  EXPECT_EQ(tree_view->currentIndex().column(), 1);
}
}  // namespace tesseract::gui

int main(int argc, char** argv)
{
  qputenv("QT_QPA_PLATFORM", QByteArray("offscreen"));
  QApplication application(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
