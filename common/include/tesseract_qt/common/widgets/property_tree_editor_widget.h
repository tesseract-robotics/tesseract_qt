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
#ifndef TESSERACT_QT_COMMON_PROPERTY_TREE_EDITOR_WIDGET_H
#define TESSERACT_QT_COMMON_PROPERTY_TREE_EDITOR_WIDGET_H

#ifndef Q_MOC_RUN
#include <tesseract_qt/common/plugin_schema_resolver.h>
#include <tesseract/common/property_tree.h>
#include <yaml-cpp/yaml.h>

#include <QStringList>
#include <QWidget>
#endif

#include <cstdint>
#include <memory>

class QAction;
class QUndoStack;

namespace tesseract::gui
{
class PropertyTreeModel;
struct PluginSchemaCatalog;

/** @brief Tree-based editor that renders and validates a PropertyTree schema. */
class PropertyTreeEditorWidget : public QWidget
{
  Q_OBJECT

public:
  /** @brief State of the plugin schemas used to render the current draft. */
  enum class PluginSchemaState : std::uint8_t
  {
    NotRequired,
    Current,
    Stale,
    Failed
  };
  Q_ENUM(PluginSchemaState)

  explicit PropertyTreeEditorWidget(QWidget* parent = nullptr);
  ~PropertyTreeEditorWidget() override;

  PropertyTreeEditorWidget(const PropertyTreeEditorWidget&) = delete;
  PropertyTreeEditorWidget& operator=(const PropertyTreeEditorWidget&) = delete;
  PropertyTreeEditorWidget(PropertyTreeEditorWidget&&) = delete;
  PropertyTreeEditorWidget& operator=(PropertyTreeEditorWidget&&) = delete;

  /** @brief Load a schema and optional configuration draft. */
  void load(const tesseract::common::PropertyTree& schema, const YAML::Node& config = YAML::Node());

  /** @brief Clear the editor. */
  void clear();

  /** @brief Get the current normalized YAML configuration draft. */
  YAML::Node yaml() const;

  /** @brief Return current strict schema validation errors. */
  QStringList validationErrors() const;

  /** @brief Return true when the current draft passes strict schema validation. */
  bool isValid() const;

  /** @brief Set search paths used in addition to paths declared in the configuration draft. */
  void setPluginSearchPaths(std::vector<std::string> paths);

  /** @brief Set search libraries used in addition to libraries declared in the configuration draft. */
  void setPluginSearchLibraries(std::vector<std::string> libraries);

  /** @brief Set optional boost_plugin_loader search environment variable names. */
  void setPluginSearchEnvironment(std::string paths_environment, std::string libraries_environment);

  /** @brief Explicitly refresh all plugin catalogs declared by the loaded schema. */
  void refreshPluginSchemas();

  /** @brief Return the plugin catalogs from the most recent successful refresh. */
  const std::vector<PluginSchemaCatalog>& pluginSchemaCatalogs() const;

  /** @brief Return the state of plugin schemas used by the editor. */
  PluginSchemaState pluginSchemaState() const;

  /** @brief Return the most recent plugin schema discovery error. */
  QString pluginSchemaError() const;

  /** @brief Return the undo stack for configuration edits. */
  QUndoStack* undoStack();

  /** @brief Return the undo stack for configuration edits. */
  const QUndoStack* undoStack() const;

  /** @brief Return the reusable Undo action. */
  QAction* undoAction();

  /** @brief Return the reusable Redo action. */
  QAction* redoAction();

  /** @brief Set the recursive property filter text. */
  void setFilterText(QString text);

  /** @brief Return the current recursive property filter text. */
  QString filterText() const;

  /** @brief Focus and reveal the property identified by a model path. */
  bool focusPath(const QString& path);

  /** @brief Return the reusable action that focuses the search field. */
  QAction* focusSearchAction();

  /** @brief Return the reusable Expand All action. */
  QAction* expandAllAction();

  /** @brief Return the reusable Collapse All action. */
  QAction* collapseAllAction();

  /** @brief Access the editor model. */
  PropertyTreeModel* model();

  /** @brief Access the editor model. */
  const PropertyTreeModel* model() const;

Q_SIGNALS:
  /** @brief Emitted after the user changes the configuration draft. */
  void configurationChanged();

  /** @brief Emitted whenever validation is recomputed. */
  void validationChanged(bool valid, const QStringList& errors);

  /** @brief Emitted after plugin schemas are refreshed and committed. */
  void pluginSchemasChanged();

  /** @brief Emitted when plugin schema discovery state changes. */
  void pluginSchemaStateChanged(PluginSchemaState state, const QString& message);

  /** @brief Emitted when a refresh requested by the widget button fails. */
  void pluginSchemaRefreshFailed(const QString& message);

private:
  void setPluginSchemaState(PluginSchemaState state, QString message = {});
  void updateDetails(const QModelIndex& index);
  void updateYamlPreview();

  struct Implementation;
  std::unique_ptr<Implementation> data_;
};
}  // namespace tesseract::gui

#endif  // TESSERACT_QT_COMMON_PROPERTY_TREE_EDITOR_WIDGET_H
