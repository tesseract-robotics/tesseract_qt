/*
 * @copyright Copyright (C) 2026 Levi Armstrong <leviarmstrong@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 */
#ifndef TESSERACT_QT_COMMON_CONFIG_EDITOR_WIDGET_H
#define TESSERACT_QT_COMMON_CONFIG_EDITOR_WIDGET_H

#ifndef Q_MOC_RUN
#include <tesseract_qt/common/config_definition.h>
#include <tesseract_qt/common/widgets/property_tree_editor_widget.h>
#include <yaml-cpp/yaml.h>

#include <QString>
#include <QStringList>
#include <QWidget>
#endif

#include <memory>
#include <string>
#include <vector>

namespace tesseract::gui
{
/** @brief Embeddable, single-document editor for a schema-defined YAML configuration. */
class ConfigEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigEditorWidget(QWidget* parent = nullptr);
  ~ConfigEditorWidget() override;

  ConfigEditorWidget(const ConfigEditorWidget&) = delete;
  ConfigEditorWidget& operator=(const ConfigEditorWidget&) = delete;
  ConfigEditorWidget(ConfigEditorWidget&&) = delete;
  ConfigEditorWidget& operator=(ConfigEditorWidget&&) = delete;

  /** @brief Create and load an empty document for a definition. */
  void newConfiguration(const ConfigDefinition& definition);

  /**
   * @brief Load a configuration document transactionally.
   * @param definition Configuration type definition.
   * @param document Wrapped or unwrapped YAML document.
   * @param source_path Optional source path associated with the document.
   * @throws std::runtime_error if the document cannot be represented by the schema.
   */
  void loadConfiguration(const ConfigDefinition& definition, const YAML::Node& document, QString source_path = {});

  /** @brief Clear the active document. */
  void clear();

  /** @brief Return true when a document is loaded. */
  bool hasDocument() const;

  /** @brief Return the active definition, or nullptr when empty. */
  const ConfigDefinition* definition() const;

  /** @brief Return the current normalized and wrapped YAML document. */
  YAML::Node yaml() const;

  /** @brief Return the path associated with the active document. */
  QString sourcePath() const;

  /** @brief Associate a path with the active document without writing it. */
  void setSourcePath(QString path);

  /** @brief Return true when the document differs from its clean baseline. */
  bool isModified() const;

  /** @brief Mark the current document as the clean baseline. */
  void markClean();

  /** @brief Return current strict schema validation errors. */
  QStringList validationErrors() const;

  /** @brief Return true when strict schema validation succeeds. */
  bool isValid() const;

  /** @brief Return true when normal Save is permitted. */
  bool isSaveReady() const;

  /** @brief Return the current plugin schema discovery state. */
  PropertyTreeEditorWidget::PluginSchemaState pluginSchemaState() const;

  /** @brief Return the latest plugin schema discovery error. */
  QString pluginSchemaError() const;

  /** @brief Configure additional plugin search paths. */
  void setPluginSearchPaths(std::vector<std::string> paths);

  /** @brief Configure additional plugin search libraries. */
  void setPluginSearchLibraries(std::vector<std::string> libraries);

  /** @brief Refresh plugin schemas for the current document. */
  void refreshPluginSchemas();

  /**
   * @brief Atomically save a valid document and mark it clean.
   * @param path Destination path.
   * @param error Optional destination for an actionable error message.
   * @return True on success.
   */
  bool save(const QString& path, QString* error = nullptr);

  /**
   * @brief Atomically export the current draft without changing path or clean state.
   * @param path Destination path.
   * @param error Optional destination for an actionable error message.
   * @return True on success.
   */
  bool exportDraft(const QString& path, QString* error = nullptr) const;

  /** @brief Access the generic schema editor. */
  PropertyTreeEditorWidget* editor();

  /** @brief Access the generic schema editor. */
  const PropertyTreeEditorWidget* editor() const;

Q_SIGNALS:
  /** @brief Emitted after the active definition or document is replaced. */
  void documentChanged();

  /** @brief Emitted whenever the modified state changes. */
  void modifiedChanged(bool modified);

  /** @brief Emitted whenever the associated source path changes. */
  void sourcePathChanged(const QString& path);

  /** @brief Emitted whenever strict validation is recomputed. */
  void validationChanged(bool valid, const QStringList& errors);

  /** @brief Emitted whenever plugin discovery state changes. */
  void pluginSchemaStateChanged(PropertyTreeEditorWidget::PluginSchemaState state, const QString& message);

  /** @brief Emitted whenever normal Save readiness changes. */
  void saveReadinessChanged(bool ready);

private:
  struct Implementation;
  std::unique_ptr<Implementation> data_;
};
}  // namespace tesseract::gui

#endif  // TESSERACT_QT_COMMON_CONFIG_EDITOR_WIDGET_H