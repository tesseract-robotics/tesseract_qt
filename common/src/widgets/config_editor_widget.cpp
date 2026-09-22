/*
 * @copyright Copyright (C) 2026 Levi Armstrong <leviarmstrong@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 */
#include <tesseract_qt/common/widgets/config_editor_widget.h>

#include <tesseract_qt/common/models/property_tree_model.h>

#include <QSaveFile>
#include <QVBoxLayout>

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <utility>

namespace tesseract::gui
{
namespace
{
YAML::Node unwrapConfig(const ConfigDefinition& definition, const YAML::Node& document)
{
  if (!document || !document.IsMap())
    return document;

  const std::string key = definition.configKey();
  return document[key] ? document[key] : document;
}

QString serializeYaml(const YAML::Node& document)
{
  if (!document)
    return {};
  YAML::Emitter emitter;
  emitter << document;
  if (!emitter.good())
    throw std::runtime_error(std::string("Failed to serialize YAML document: ") + emitter.GetLastError());
  return QString::fromStdString(emitter.c_str());
}

bool writeYaml(const QString& path, const YAML::Node& document, QString* error)
{
  if (path.isEmpty())
  {
    if (error != nullptr)
      *error = QObject::tr("A destination path is required.");
    return false;
  }

  QString serialized;
  try
  {
    serialized = serializeYaml(document) + "\n";
  }
  catch (const std::exception& exception)
  {
    if (error != nullptr)
      *error = QString::fromStdString(exception.what());
    return false;
  }

  QSaveFile output(path);
  if (!output.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    if (error != nullptr)
      *error = QObject::tr("Could not open '%1' for writing: %2").arg(path, output.errorString());
    return false;
  }

  const QByteArray contents = serialized.toUtf8();
  if (output.write(contents) != contents.size())
  {
    if (error != nullptr)
      *error = QObject::tr("Could not write '%1': %2").arg(path, output.errorString());
    output.cancelWriting();
    return false;
  }

  if (!output.commit())
  {
    if (error != nullptr)
      *error = QObject::tr("Could not commit '%1': %2").arg(path, output.errorString());
    return false;
  }

  return true;
}

std::vector<std::string> combinedSearchInputs(const std::vector<std::string>& defaults,
                                              const std::vector<std::string>& additions)
{
  std::vector<std::string> result = defaults;
  for (const auto& addition : additions)
  {
    if (std::find(result.begin(), result.end(), addition) == result.end())
      result.push_back(addition);
  }
  return result;
}
}  // namespace

struct ConfigEditorWidget::Implementation
{
  explicit Implementation(ConfigEditorWidget* q) : editor(new PropertyTreeEditorWidget(q)) {}

  PropertyTreeEditorWidget* editor;
  std::optional<ConfigDefinition> definition;
  QString source_path;
  QString clean_document;
  std::vector<std::string> additional_search_paths;
  std::vector<std::string> additional_search_libraries;
  bool loading{ false };
  bool modified{ false };
  bool save_ready{ false };
};

ConfigEditorWidget::ConfigEditorWidget(QWidget* parent) : QWidget(parent), data_(std::make_unique<Implementation>(this))
{
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(data_->editor);

  const auto update_modified = [this]() {
    if (data_->loading || !data_->definition.has_value())
      return;
    const bool modified = serializeYaml(data_->editor->yaml()) != data_->clean_document;
    if (modified != data_->modified)
    {
      data_->modified = modified;
      Q_EMIT modifiedChanged(modified);
    }
  };
  const auto update_save_ready = [this]() {
    const auto state = data_->editor->pluginSchemaState();
    const bool discovery_ready = state == PropertyTreeEditorWidget::PluginSchemaState::NotRequired ||
                                 state == PropertyTreeEditorWidget::PluginSchemaState::Current;
    const bool ready = data_->definition.has_value() && data_->editor->isValid() && discovery_ready;
    if (ready != data_->save_ready)
    {
      data_->save_ready = ready;
      Q_EMIT saveReadinessChanged(ready);
    }
  };

  connect(
      data_->editor, &PropertyTreeEditorWidget::configurationChanged, this, [update_modified]() { update_modified(); });
  connect(data_->editor,
          &PropertyTreeEditorWidget::validationChanged,
          this,
          [this, update_save_ready](bool valid, const QStringList& errors) {
            if (data_->loading)
              return;
            Q_EMIT validationChanged(valid, errors);
            update_save_ready();
          });
  connect(data_->editor,
          &PropertyTreeEditorWidget::pluginSchemaStateChanged,
          this,
          [this, update_save_ready](PropertyTreeEditorWidget::PluginSchemaState state, const QString& message) {
            if (data_->loading)
              return;
            Q_EMIT pluginSchemaStateChanged(state, message);
            update_save_ready();
          });
}

ConfigEditorWidget::~ConfigEditorWidget() = default;

void ConfigEditorWidget::newConfiguration(const ConfigDefinition& definition)
{
  loadConfiguration(definition, definition.empty_document);
}

void ConfigEditorWidget::loadConfiguration(const ConfigDefinition& definition,
                                           const YAML::Node& document,
                                           QString source_path)
{
  PropertyTreeModel candidate;
  candidate.load(definition.schema, unwrapConfig(definition, document));

  data_->loading = true;
  try
  {
    data_->editor->setPluginSearchPaths(
        combinedSearchInputs(definition.default_search_paths, data_->additional_search_paths));
    data_->editor->setPluginSearchLibraries(
        combinedSearchInputs(definition.default_search_libraries, data_->additional_search_libraries));
    data_->editor->setPluginSearchEnvironment(definition.search_paths_environment,
                                              definition.search_libraries_environment);
    data_->editor->load(definition.schema, document);
  }
  catch (...)
  {
    data_->loading = false;
    throw;
  }

  const QString previous_path = data_->source_path;
  const bool was_modified = data_->modified;
  data_->definition = definition;
  data_->source_path = std::move(source_path);
  data_->clean_document = serializeYaml(data_->editor->yaml());
  data_->modified = false;
  const auto state = data_->editor->pluginSchemaState();
  data_->save_ready = data_->editor->isValid() && (state == PropertyTreeEditorWidget::PluginSchemaState::NotRequired ||
                                                   state == PropertyTreeEditorWidget::PluginSchemaState::Current);
  data_->loading = false;

  if (was_modified)
    Q_EMIT modifiedChanged(false);
  if (previous_path != data_->source_path)
    Q_EMIT sourcePathChanged(data_->source_path);
  Q_EMIT documentChanged();
  Q_EMIT validationChanged(data_->editor->isValid(), data_->editor->validationErrors());
  Q_EMIT pluginSchemaStateChanged(state, data_->editor->pluginSchemaError());
  Q_EMIT saveReadinessChanged(data_->save_ready);
}

void ConfigEditorWidget::clear()
{
  const bool had_document = data_->definition.has_value();
  const bool was_modified = data_->modified;
  const bool was_save_ready = data_->save_ready;
  const bool had_path = !data_->source_path.isEmpty();
  data_->loading = true;
  data_->editor->clear();
  data_->definition.reset();
  data_->source_path.clear();
  data_->clean_document.clear();
  data_->modified = false;
  data_->save_ready = false;
  data_->loading = false;
  if (was_modified)
    Q_EMIT modifiedChanged(false);
  if (had_path)
    Q_EMIT sourcePathChanged({});
  if (was_save_ready)
    Q_EMIT saveReadinessChanged(false);
  if (had_document)
    Q_EMIT documentChanged();
}

bool ConfigEditorWidget::hasDocument() const { return data_->definition.has_value(); }

const ConfigDefinition* ConfigEditorWidget::definition() const
{
  return data_->definition.has_value() ? &(*data_->definition) : nullptr;
}

YAML::Node ConfigEditorWidget::yaml() const { return data_->editor->yaml(); }

QString ConfigEditorWidget::sourcePath() const { return data_->source_path; }

void ConfigEditorWidget::setSourcePath(QString path)
{
  if (data_->source_path == path)
    return;
  data_->source_path = std::move(path);
  Q_EMIT sourcePathChanged(data_->source_path);
}

bool ConfigEditorWidget::isModified() const { return data_->modified; }

void ConfigEditorWidget::markClean()
{
  if (!data_->definition.has_value())
    return;
  data_->clean_document = serializeYaml(data_->editor->yaml());
  if (data_->modified)
  {
    data_->modified = false;
    Q_EMIT modifiedChanged(false);
  }
}

QStringList ConfigEditorWidget::validationErrors() const { return data_->editor->validationErrors(); }

bool ConfigEditorWidget::isValid() const { return data_->editor->isValid(); }

bool ConfigEditorWidget::isSaveReady() const { return data_->save_ready; }

PropertyTreeEditorWidget::PluginSchemaState ConfigEditorWidget::pluginSchemaState() const
{
  return data_->editor->pluginSchemaState();
}

QString ConfigEditorWidget::pluginSchemaError() const { return data_->editor->pluginSchemaError(); }

void ConfigEditorWidget::setPluginSearchPaths(std::vector<std::string> paths)
{
  data_->additional_search_paths = std::move(paths);
  if (data_->definition.has_value())
    data_->editor->setPluginSearchPaths(
        combinedSearchInputs(data_->definition->default_search_paths, data_->additional_search_paths));
}

void ConfigEditorWidget::setPluginSearchLibraries(std::vector<std::string> libraries)
{
  data_->additional_search_libraries = std::move(libraries);
  if (data_->definition.has_value())
    data_->editor->setPluginSearchLibraries(
        combinedSearchInputs(data_->definition->default_search_libraries, data_->additional_search_libraries));
}

void ConfigEditorWidget::refreshPluginSchemas() { data_->editor->refreshPluginSchemas(); }

bool ConfigEditorWidget::save(const QString& path, QString* error)
{
  if (!isSaveReady())
  {
    if (error != nullptr)
      *error = tr("The configuration must be valid and plugin schemas must be current before it can be saved.");
    return false;
  }
  if (!writeYaml(path, yaml(), error))
    return false;
  setSourcePath(path);
  markClean();
  return true;
}

bool ConfigEditorWidget::exportDraft(const QString& path, QString* error) const
{
  if (!hasDocument())
  {
    if (error != nullptr)
      *error = tr("There is no configuration document to export.");
    return false;
  }
  return writeYaml(path, yaml(), error);
}

PropertyTreeEditorWidget* ConfigEditorWidget::editor() { return data_->editor; }

const PropertyTreeEditorWidget* ConfigEditorWidget::editor() const { return data_->editor; }
}  // namespace tesseract::gui