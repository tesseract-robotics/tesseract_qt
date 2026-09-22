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
#include <tesseract_qt/common/widgets/property_tree_editor_widget.h>

#include <tesseract_qt/common/models/property_tree_model.h>
#include <tesseract_qt/common/widgets/property_tree_item_delegate.h>
#include <tesseract_qt/common/widgets/tree_view.h>

#include <QAction>
#include <QFontDatabase>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSet>
#include <QSizePolicy>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QTabWidget>
#include <QToolButton>
#include <QUndoCommand>
#include <QUndoStack>
#include <QVBoxLayout>

#include <algorithm>
#include <functional>
#include <utility>

namespace tesseract::gui
{
namespace
{
bool containsPluginMetadata(const tesseract::common::PropertyTree& schema)
{
  if (schema.hasAttribute(tesseract::common::property_attribute::PLUGIN_SECTION))
    return true;

  const auto keys = schema.keys();
  return std::any_of(
      keys.begin(), keys.end(), [&schema](const auto& key) { return containsPluginMetadata(schema.at(key)); });
}

void collectPluginDiscoveryPaths(const tesseract::common::PropertyTree& schema,
                                 const QString& path,
                                 QSet<QString>& paths)
{
  if (schema.hasAttribute(tesseract::common::property_attribute::PLUGIN_DISCOVERY_ROLE))
    paths.insert(path);

  for (const auto& key : schema.keys())
  {
    const QString child_path = path.isEmpty() ? QString::fromStdString(key) : path + "." + QString::fromStdString(key);
    collectPluginDiscoveryPaths(schema.at(key), child_path, paths);
  }
}

bool pathAffectsPluginDiscovery(const QSet<QString>& discovery_paths, const QString& changed_path)
{
  return std::any_of(discovery_paths.begin(), discovery_paths.end(), [&changed_path](const QString& discovery_path) {
    return changed_path == discovery_path || changed_path.startsWith(discovery_path + "[");
  });
}

YAML::Node unwrapConfig(const tesseract::common::PropertyTree& schema, const YAML::Node& config)
{
  const auto config_key = schema.getAttribute(tesseract::common::property_attribute::CONFIG_KEY);
  if (!config_key.has_value() || !config || !config.IsMap())
    return config;

  const auto key = config_key->as<std::string>();
  return config[key] ? config[key] : config;
}

QModelIndex findPath(const QAbstractItemModel& model, const QString& path, const QModelIndex& parent = {})
{
  for (int row = 0; row < model.rowCount(parent); ++row)
  {
    const QModelIndex index = model.index(row, 0, parent);
    if (index.data(PropertyTreeModel::PathRole).toString() == path)
      return index;
    const QModelIndex descendant = findPath(model, path, index);
    if (descendant.isValid())
      return descendant;
  }
  return {};
}

QModelIndex findValidationError(const QAbstractItemModel& model, const QString& error, const QModelIndex& parent = {})
{
  QModelIndex closest_match;
  for (int row = 0; row < model.rowCount(parent); ++row)
  {
    const QModelIndex index = model.index(row, 0, parent);
    if (index.data(PropertyTreeModel::ValidationErrorsRole).toStringList().contains(error))
      closest_match = index;
    const QModelIndex descendant = findValidationError(model, error, index);
    if (descendant.isValid())
      return descendant;
  }
  return closest_match;
}

class ReplaceYamlCommand : public QUndoCommand
{
public:
  using ApplyFunction = std::function<void(const YAML::Node&, const QString&)>;

  ReplaceYamlCommand(YAML::Node before, YAML::Node after, QString changed_path, QString text, ApplyFunction apply)
    : before_(std::move(before))
    , after_(std::move(after))
    , changed_path_(std::move(changed_path))
    , apply_(std::move(apply))
  {
    setText(std::move(text));
  }

  void undo() override { apply_(before_, changed_path_); }

  void redo() override
  {
    if (first_redo_)
    {
      first_redo_ = false;
      return;
    }
    apply_(after_, changed_path_);
  }

private:
  YAML::Node before_;
  YAML::Node after_;
  QString changed_path_;
  ApplyFunction apply_;
  bool first_redo_{ true };
};

class PropertyTreeFilterProxyModel : public QSortFilterProxyModel
{
public:
  explicit PropertyTreeFilterProxyModel(QObject* parent = nullptr) : QSortFilterProxyModel(parent) {}

  void setFilterText(QString text)
  {
    text_ = std::move(text);
    invalidateFilter();
  }

  const QString& filterText() const { return text_; }

  int matchCount() const { return countMatches({}); }

protected:
  bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override
  {
    if (text_.isEmpty())
      return true;

    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    if (matches(index))
      return true;

    for (int row = 0; row < sourceModel()->rowCount(index); ++row)
    {
      if (filterAcceptsRow(row, index))
        return true;
    }
    return false;
  }

private:
  bool matches(const QModelIndex& index) const
  {
    QStringList values;
    for (int column = 0; column < sourceModel()->columnCount(index.parent()); ++column)
      values.push_back(sourceModel()->index(index.row(), column, index.parent()).data().toString());
    values.push_back(index.data(PropertyTreeModel::PathRole).toString());
    values.push_back(index.data(PropertyTreeModel::DocumentationRole).toString());
    values.append(index.data(PropertyTreeModel::ValidationErrorsRole).toStringList());
    return values.join("\n").contains(text_, Qt::CaseInsensitive);
  }

  int countMatches(const QModelIndex& parent) const
  {
    int count{ 0 };
    for (int row = 0; row < sourceModel()->rowCount(parent); ++row)
    {
      const QModelIndex index = sourceModel()->index(row, 0, parent);
      if (matches(index))
        ++count;
      count += countMatches(index);
    }
    return count;
  }

  QString text_;
};

QString validationDisplayText(const QString& error)
{
  const int separator = error.lastIndexOf(QStringLiteral(": "));
  if (separator <= 0)
    return error;

  const QString path = error.left(separator);
  const QString issue = error.mid(separator + 2);
  return issue + "\n" + path;
}
}  // namespace

struct PropertyTreeEditorWidget::Implementation
{
  tesseract::common::PropertyTree schema;
  PluginSchemaResolver resolver;
  PropertyTreeModel* model{ nullptr };
  PropertyTreeFilterProxyModel* proxy_model{ nullptr };
  TreeView* tree_view{ nullptr };
  QLineEdit* filter{ nullptr };
  QLabel* filter_count{ nullptr };
  QPushButton* refresh_plugins{ nullptr };
  QPushButton* add{ nullptr };
  QPushButton* rename{ nullptr };
  QPushButton* remove{ nullptr };
  QPushButton* move_up{ nullptr };
  QPushButton* move_down{ nullptr };
  QLabel* plugin_error{ nullptr };
  QFrame* diagnostics{ nullptr };
  QLabel* errors_label{ nullptr };
  QListWidget* errors{ nullptr };
  QLabel* details{ nullptr };
  QPlainTextEdit* yaml_preview{ nullptr };
  QSet<QString> expanded_paths;
  QString current_path;
  int current_column{ 0 };
  QUndoStack* undo_stack{ nullptr };
  QAction* undo_action{ nullptr };
  QAction* redo_action{ nullptr };
  QAction* focus_search_action{ nullptr };
  QAction* expand_all_action{ nullptr };
  QAction* collapse_all_action{ nullptr };
  YAML::Node previous_config;
  bool applying_undo_redo{ false };
  QSet<QString> plugin_discovery_paths;
  bool has_plugin_metadata{ false };
  PropertyTreeEditorWidget::PluginSchemaState plugin_schema_state{
    PropertyTreeEditorWidget::PluginSchemaState::NotRequired
  };
  QString plugin_schema_error;
};

PropertyTreeEditorWidget::PropertyTreeEditorWidget(QWidget* parent)
  : QWidget(parent), data_(std::make_unique<Implementation>())
{
  data_->model = new PropertyTreeModel(this);
  data_->undo_stack = new QUndoStack(this);
  data_->undo_action = data_->undo_stack->createUndoAction(this, tr("Undo"));
  data_->redo_action = data_->undo_stack->createRedoAction(this, tr("Redo"));
  data_->undo_action->setShortcut(QKeySequence::Undo);
  data_->redo_action->setShortcut(QKeySequence::Redo);
  data_->focus_search_action = new QAction(tr("Find"), this);
  data_->focus_search_action->setShortcut(QKeySequence::Find);
  data_->expand_all_action = new QAction(tr("Expand All"), this);
  data_->collapse_all_action = new QAction(tr("Collapse All"), this);
  addAction(data_->undo_action);
  addAction(data_->redo_action);
  addAction(data_->focus_search_action);
  data_->tree_view = new TreeView(this);
  data_->proxy_model = new PropertyTreeFilterProxyModel(this);
  data_->proxy_model->setSourceModel(data_->model);
  data_->tree_view->setModel(data_->proxy_model);
  data_->tree_view->setItemDelegateForColumn(1, new PropertyTreeItemDelegate(data_->tree_view));
  data_->tree_view->setAlternatingRowColors(true);
  data_->tree_view->setUniformRowHeights(false);
  data_->tree_view->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  data_->tree_view->header()->setSectionResizeMode(1, QHeaderView::Stretch);
  data_->tree_view->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

  data_->filter = new QLineEdit(this);
  data_->filter->setPlaceholderText(tr("Search properties, values, paths, and errors…"));
  data_->filter->setClearButtonEnabled(true);
  data_->filter_count = new QLabel(this);
  data_->filter_count->hide();

  data_->plugin_error = new QLabel(this);
  data_->plugin_error->setWordWrap(true);
  data_->plugin_error->setStyleSheet(QStringLiteral("color: #c62828;"));
  data_->plugin_error->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  data_->plugin_error->hide();
  data_->diagnostics = new QFrame(this);
  data_->diagnostics->setObjectName(QStringLiteral("validationDiagnostics"));
  data_->diagnostics->setFrameShape(QFrame::StyledPanel);
  data_->diagnostics->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  data_->diagnostics->setMaximumHeight(190);
  data_->errors_label = new QLabel(data_->diagnostics);
  data_->errors_label->setObjectName(QStringLiteral("validationDiagnosticsHeader"));
  data_->errors_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  data_->errors = new QListWidget(data_->diagnostics);
  data_->errors->setObjectName(QStringLiteral("validationDiagnosticsList"));
  data_->errors->setWordWrap(true);
  data_->errors->setTextElideMode(Qt::ElideNone);
  data_->errors->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  data_->errors->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  data_->errors->setAlternatingRowColors(true);
  data_->errors->setSpacing(2);
  auto* diagnostics_layout = new QVBoxLayout(data_->diagnostics);
  diagnostics_layout->setContentsMargins(6, 4, 6, 6);
  diagnostics_layout->setSpacing(4);
  diagnostics_layout->addWidget(data_->errors_label);
  diagnostics_layout->addWidget(data_->errors);
  data_->diagnostics->hide();

  data_->details = new QLabel(tr("Select a property to inspect its schema."), this);
  data_->details->setObjectName(QStringLiteral("propertyDetails"));
  data_->details->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  data_->details->setTextFormat(Qt::RichText);
  data_->details->setTextInteractionFlags(Qt::TextSelectableByMouse);
  data_->details->setWordWrap(true);
  data_->details->setMargin(8);

  data_->yaml_preview = new QPlainTextEdit(this);
  data_->yaml_preview->setObjectName(QStringLiteral("yamlPreview"));
  data_->yaml_preview->setReadOnly(true);
  data_->yaml_preview->setLineWrapMode(QPlainTextEdit::NoWrap);
  data_->yaml_preview->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

  data_->refresh_plugins = new QPushButton(tr("Refresh Plugins"), this);
  data_->refresh_plugins->hide();
  data_->add = new QPushButton(tr("Add"), this);
  data_->rename = new QPushButton(tr("Rename"), this);
  data_->remove = new QPushButton(tr("Remove"), this);
  data_->move_up = new QPushButton(tr("Move Up"), this);
  data_->move_down = new QPushButton(tr("Move Down"), this);

  auto* search = new QHBoxLayout();
  search->addWidget(data_->filter);
  search->addWidget(data_->filter_count);

  auto* controls = new QHBoxLayout();
  const auto add_history_button = [this, controls](QAction* action, const QString& label, const QString& object_name) {
    auto* button = new QToolButton(this);
    button->setObjectName(object_name);
    button->setText(label);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setEnabled(action->isEnabled());
    button->setToolTip(action->text());
    connect(button, &QToolButton::clicked, action, &QAction::trigger);
    connect(action, &QAction::changed, button, [button, action]() {
      button->setEnabled(action->isEnabled());
      button->setToolTip(action->text());
    });
    controls->addWidget(button);
  };
  add_history_button(data_->undo_action, tr("Undo"), QStringLiteral("undoButton"));
  add_history_button(data_->redo_action, tr("Redo"), QStringLiteral("redoButton"));
  controls->addWidget(data_->refresh_plugins);
  controls->addStretch();
  controls->addWidget(data_->add);
  controls->addWidget(data_->rename);
  controls->addWidget(data_->remove);
  controls->addWidget(data_->move_up);
  controls->addWidget(data_->move_down);
  for (QAction* action : { data_->expand_all_action, data_->collapse_all_action })
  {
    auto* button = new QToolButton(this);
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    controls->addWidget(button);
  }

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addLayout(search);
  layout->addLayout(controls);
  auto* inspector = new QTabWidget(this);
  inspector->setObjectName(QStringLiteral("propertyInspector"));
  inspector->addTab(data_->details, tr("Details"));
  inspector->addTab(data_->yaml_preview, tr("YAML Preview"));
  auto* splitter = new QSplitter(Qt::Horizontal, this);
  splitter->setObjectName(QStringLiteral("propertyEditorSplitter"));
  splitter->addWidget(data_->tree_view);
  splitter->addWidget(inspector);
  splitter->setStretchFactor(0, 3);
  splitter->setStretchFactor(1, 1);
  splitter->setSizes({ 800, 320 });
  layout->addWidget(splitter, 1);
  layout->addWidget(data_->plugin_error);
  layout->addWidget(data_->diagnostics);

  connect(data_->model, &PropertyTreeModel::configurationChangedAt, this, [this](const QString& path) {
    const YAML::Node current_config = data_->model->yaml();
    if (!data_->applying_undo_redo && data_->previous_config)
    {
      QString action_text = path.isEmpty() ? tr("Edit configuration") : tr("Edit %1").arg(path);
      data_->undo_stack->push(new ReplaceYamlCommand(YAML::Clone(data_->previous_config),
                                                     YAML::Clone(current_config),
                                                     path,
                                                     std::move(action_text),
                                                     [this](const YAML::Node& config, const QString& changed_path) {
                                                       data_->applying_undo_redo = true;
                                                       try
                                                       {
                                                         data_->model->replaceYaml(config, changed_path);
                                                       }
                                                       catch (...)
                                                       {
                                                         data_->applying_undo_redo = false;
                                                         throw;
                                                       }
                                                       data_->applying_undo_redo = false;
                                                     }));
    }
    data_->previous_config = YAML::Clone(current_config);
    if (pathAffectsPluginDiscovery(data_->plugin_discovery_paths, path))
      setPluginSchemaState(PluginSchemaState::Stale);
    for (const auto& expanded_path : std::as_const(data_->expanded_paths))
    {
      const QModelIndex index = findPath(*data_->proxy_model, expanded_path);
      if (index.isValid())
        data_->tree_view->setExpanded(index, true);
    }
    const QModelIndex current = findPath(*data_->proxy_model, data_->current_path);
    if (current.isValid())
      data_->tree_view->setCurrentIndex(current.sibling(current.row(), data_->current_column));
    updateYamlPreview();
    Q_EMIT configurationChanged();
  });
  connect(data_->model, &PropertyTreeModel::validationChanged, this, [this](bool valid, const QStringList& errors) {
    data_->diagnostics->setVisible(!valid);
    data_->errors_label->setText(
        tr("%n validation issue(s) — select one to reveal its property", nullptr, errors.size()));
    data_->errors->clear();
    for (const QString& error : errors)
    {
      auto* item = new QListWidgetItem(validationDisplayText(error), data_->errors);
      item->setData(Qt::UserRole,
                    findValidationError(*data_->model, error).data(PropertyTreeModel::PathRole).toString());
      item->setToolTip(error);
    }
    Q_EMIT validationChanged(valid, errors);
  });
  connect(data_->filter, &QLineEdit::textChanged, this, [this](const QString& text) {
    data_->proxy_model->setFilterText(text);
    data_->filter_count->setVisible(!text.isEmpty());
    data_->filter_count->setText(tr("%1 matches").arg(data_->proxy_model->matchCount()));
    if (!text.isEmpty())
      data_->tree_view->expandAll();
  });
  const auto update_filter_count = [this]() {
    if (!data_->filter->text().isEmpty())
      data_->filter_count->setText(tr("%1 matches").arg(data_->proxy_model->matchCount()));
  };
  connect(data_->model, &QAbstractItemModel::modelReset, this, update_filter_count);
  connect(data_->model, &QAbstractItemModel::dataChanged, this, update_filter_count);
  connect(data_->focus_search_action, &QAction::triggered, data_->filter, QOverload<>::of(&QLineEdit::setFocus));
  connect(data_->expand_all_action, &QAction::triggered, data_->tree_view, &QTreeView::expandAll);
  connect(data_->collapse_all_action, &QAction::triggered, data_->tree_view, &QTreeView::collapseAll);
  connect(data_->errors, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
    focusPath(item->data(Qt::UserRole).toString());
  });
  connect(data_->refresh_plugins, &QPushButton::clicked, this, [this]() {
    try
    {
      refreshPluginSchemas();
    }
    catch (const std::exception& exception)
    {
      Q_EMIT pluginSchemaRefreshFailed(QString::fromStdString(exception.what()));
    }
  });
  connect(data_->tree_view, &QTreeView::expanded, this, [this](const QModelIndex& index) {
    data_->expanded_paths.insert(index.data(PropertyTreeModel::PathRole).toString());
  });
  connect(data_->tree_view, &QTreeView::collapsed, this, [this](const QModelIndex& index) {
    data_->expanded_paths.remove(index.data(PropertyTreeModel::PathRole).toString());
  });
  connect(data_->proxy_model, &QAbstractItemModel::modelReset, this, [this]() {
    for (const auto& path : std::as_const(data_->expanded_paths))
    {
      const QModelIndex index = findPath(*data_->model, path);
      if (index.isValid())
        data_->tree_view->setExpanded(index, true);
    }

    const QModelIndex current = findPath(*data_->model, data_->current_path);
    if (current.isValid())
      data_->tree_view->setCurrentIndex(current.sibling(current.row(), data_->current_column));
  });
  const auto update_controls = [this]() {
    const QModelIndex proxy_selected = data_->tree_view->currentIndex();
    const QModelIndex selected = data_->proxy_model->mapToSource(proxy_selected);
    data_->add->setEnabled(selected.data(PropertyTreeModel::CanAddRole).toBool());
    data_->rename->setEnabled(selected.data(PropertyTreeModel::CanRenameRole).toBool());
    data_->remove->setEnabled(selected.data(PropertyTreeModel::CanRemoveRole).toBool());
    const bool list_entry = selected.data(PropertyTreeModel::CanRemoveRole).toBool() &&
                            selected.data(PropertyTreeModel::PathRole).toString().endsWith("]");
    data_->move_up->setEnabled(list_entry && selected.row() > 0);
    data_->move_down->setEnabled(list_entry && selected.row() + 1 < data_->model->rowCount(selected.parent()));
  };
  connect(data_->tree_view->selectionModel(),
          &QItemSelectionModel::currentChanged,
          this,
          [this, update_controls](const QModelIndex& current, const QModelIndex&) {
            if (current.isValid())
            {
              data_->current_path = current.data(PropertyTreeModel::PathRole).toString();
              data_->current_column = current.column();
            }
            updateDetails(current);
            update_controls();
          });
  connect(data_->proxy_model, &QAbstractItemModel::modelReset, this, update_controls);
  connect(data_->add, &QPushButton::clicked, this, [this]() {
    const QModelIndex selected = data_->proxy_model->mapToSource(data_->tree_view->currentIndex());
    const QString type = selected.data(PropertyTreeModel::TypeRole).toString();
    if (type.startsWith("Map["))
    {
      bool accepted{ false };
      const QString key = QInputDialog::getText(this, tr("Add Entry"), tr("Name"), QLineEdit::Normal, {}, &accepted);
      if (accepted && data_->model->addMapEntry(selected, key))
        data_->tree_view->expandToDepth(1);
    }
    else if (data_->model->addListEntry(selected))
    {
      data_->tree_view->expandToDepth(1);
    }
  });
  connect(data_->rename, &QPushButton::clicked, this, [this]() {
    const QModelIndex selected = data_->proxy_model->mapToSource(data_->tree_view->currentIndex());
    bool accepted{ false };
    const QString key = QInputDialog::getText(this,
                                              tr("Rename Entry"),
                                              tr("Name"),
                                              QLineEdit::Normal,
                                              selected.sibling(selected.row(), 0).data().toString(),
                                              &accepted);
    if (accepted)
      data_->model->renameMapEntry(selected, key);
  });
  connect(data_->remove, &QPushButton::clicked, this, [this]() {
    data_->model->removeEntry(data_->proxy_model->mapToSource(data_->tree_view->currentIndex()));
  });
  connect(data_->move_up, &QPushButton::clicked, this, [this]() {
    const QModelIndex selected = data_->proxy_model->mapToSource(data_->tree_view->currentIndex());
    data_->model->moveListEntry(selected, selected.row() - 1);
  });
  connect(data_->move_down, &QPushButton::clicked, this, [this]() {
    const QModelIndex selected = data_->proxy_model->mapToSource(data_->tree_view->currentIndex());
    data_->model->moveListEntry(selected, selected.row() + 1);
  });
  update_controls();
}

PropertyTreeEditorWidget::~PropertyTreeEditorWidget() = default;

void PropertyTreeEditorWidget::load(const tesseract::common::PropertyTree& schema, const YAML::Node& config)
{
  const bool has_plugin_metadata = containsPluginMetadata(schema);
  try
  {
    data_->model->load(schema, unwrapConfig(schema, config));
  }
  catch (...)
  {
    data_->schema = {};
    data_->resolver.clear();
    data_->plugin_discovery_paths.clear();
    data_->has_plugin_metadata = false;
    data_->refresh_plugins->hide();
    setPluginSchemaState(PluginSchemaState::NotRequired);
    throw;
  }

  data_->schema = schema;
  data_->resolver.clear();
  data_->plugin_discovery_paths.clear();
  data_->has_plugin_metadata = has_plugin_metadata;
  collectPluginDiscoveryPaths(schema, {}, data_->plugin_discovery_paths);
  data_->undo_stack->clear();
  data_->previous_config = data_->model->yaml();
  data_->tree_view->expandToDepth(1);
  updateYamlPreview();
  updateDetails(data_->tree_view->currentIndex());
  data_->refresh_plugins->setVisible(has_plugin_metadata);

  if (!has_plugin_metadata)
  {
    setPluginSchemaState(PluginSchemaState::NotRequired);
    return;
  }

  try
  {
    data_->resolver.refresh(schema, config);
    data_->model->setPluginSchemaCatalogs(data_->resolver.catalogs());
    setPluginSchemaState(PluginSchemaState::Current);
  }
  catch (const std::exception& exception)
  {
    const QString message = QString::fromStdString(exception.what());
    setPluginSchemaState(PluginSchemaState::Failed, message);
    Q_EMIT pluginSchemaRefreshFailed(message);
  }
}

void PropertyTreeEditorWidget::clear()
{
  data_->schema = {};
  data_->resolver.clear();
  data_->plugin_discovery_paths.clear();
  data_->has_plugin_metadata = false;
  data_->model->clear();
  data_->undo_stack->clear();
  data_->previous_config = {};
  data_->yaml_preview->clear();
  updateDetails({});
  data_->refresh_plugins->hide();
  setPluginSchemaState(PluginSchemaState::NotRequired);
}

YAML::Node PropertyTreeEditorWidget::yaml() const
{
  YAML::Node config = data_->model->yaml();
  const auto config_key = data_->schema.getAttribute(tesseract::common::property_attribute::CONFIG_KEY);
  if (!config_key.has_value())
    return config;

  YAML::Node document(YAML::NodeType::Map);
  document[config_key->as<std::string>()] = config;
  return document;
}

QStringList PropertyTreeEditorWidget::validationErrors() const { return data_->model->validationErrors(); }

bool PropertyTreeEditorWidget::isValid() const { return data_->model->isValid(); }

void PropertyTreeEditorWidget::setPluginSearchPaths(std::vector<std::string> paths)
{
  data_->resolver.setSearchPaths(std::move(paths));
  if (data_->has_plugin_metadata)
    setPluginSchemaState(PluginSchemaState::Stale);
}

void PropertyTreeEditorWidget::setPluginSearchLibraries(std::vector<std::string> libraries)
{
  data_->resolver.setSearchLibraries(std::move(libraries));
  if (data_->has_plugin_metadata)
    setPluginSchemaState(PluginSchemaState::Stale);
}

void PropertyTreeEditorWidget::setPluginSearchEnvironment(std::string paths_environment,
                                                          std::string libraries_environment)
{
  data_->resolver.setSearchEnvironment(std::move(paths_environment), std::move(libraries_environment));
  if (data_->has_plugin_metadata)
    setPluginSchemaState(PluginSchemaState::Stale);
}

void PropertyTreeEditorWidget::refreshPluginSchemas()
{
  if (!data_->has_plugin_metadata)
  {
    setPluginSchemaState(PluginSchemaState::NotRequired);
    return;
  }

  try
  {
    data_->resolver.refresh(data_->schema, yaml());
    data_->model->setPluginSchemaCatalogs(data_->resolver.catalogs());
    data_->tree_view->expandToDepth(1);
    setPluginSchemaState(PluginSchemaState::Current);
    Q_EMIT pluginSchemasChanged();
  }
  catch (const std::exception& exception)
  {
    setPluginSchemaState(PluginSchemaState::Failed, QString::fromStdString(exception.what()));
    throw;
  }
}

const std::vector<PluginSchemaCatalog>& PropertyTreeEditorWidget::pluginSchemaCatalogs() const
{
  return data_->resolver.catalogs();
}

PropertyTreeEditorWidget::PluginSchemaState PropertyTreeEditorWidget::pluginSchemaState() const
{
  return data_->plugin_schema_state;
}

QString PropertyTreeEditorWidget::pluginSchemaError() const { return data_->plugin_schema_error; }

QUndoStack* PropertyTreeEditorWidget::undoStack() { return data_->undo_stack; }

const QUndoStack* PropertyTreeEditorWidget::undoStack() const { return data_->undo_stack; }

QAction* PropertyTreeEditorWidget::undoAction() { return data_->undo_action; }

QAction* PropertyTreeEditorWidget::redoAction() { return data_->redo_action; }

void PropertyTreeEditorWidget::setFilterText(QString text) { data_->filter->setText(std::move(text)); }

QString PropertyTreeEditorWidget::filterText() const { return data_->filter->text(); }

bool PropertyTreeEditorWidget::focusPath(const QString& path)
{
  if (path.isEmpty())
    return false;

  QModelIndex source_index = findPath(*data_->model, path);
  if (!source_index.isValid())
    return false;

  QModelIndex proxy_index = data_->proxy_model->mapFromSource(source_index);
  if (!proxy_index.isValid())
  {
    setFilterText({});
    proxy_index = data_->proxy_model->mapFromSource(source_index);
  }
  if (!proxy_index.isValid())
    return false;

  for (QModelIndex parent = proxy_index.parent(); parent.isValid(); parent = parent.parent())
    data_->tree_view->setExpanded(parent, true);
  proxy_index = proxy_index.sibling(proxy_index.row(), 1);
  data_->tree_view->setCurrentIndex(proxy_index);
  data_->tree_view->scrollTo(proxy_index);
  return true;
}

QAction* PropertyTreeEditorWidget::focusSearchAction() { return data_->focus_search_action; }

QAction* PropertyTreeEditorWidget::expandAllAction() { return data_->expand_all_action; }

QAction* PropertyTreeEditorWidget::collapseAllAction() { return data_->collapse_all_action; }

PropertyTreeModel* PropertyTreeEditorWidget::model() { return data_->model; }

const PropertyTreeModel* PropertyTreeEditorWidget::model() const { return data_->model; }

void PropertyTreeEditorWidget::updateDetails(const QModelIndex& index)
{
  if (!index.isValid())
  {
    data_->details->setText(tr("Select a property to inspect its schema."));
    return;
  }

  const QModelIndex property_index = index.sibling(index.row(), 0);
  const auto escaped = [](const QVariant& value) { return value.toString().toHtmlEscaped(); };
  QStringList rows;
  const auto add_row = [&rows, &escaped](const QString& label, const QVariant& value) {
    if (value.isValid() && !value.toString().isEmpty())
      rows.push_back(
          QStringLiteral("<tr><th align=\"left\">%1</th><td>%2</td></tr>").arg(label.toHtmlEscaped(), escaped(value)));
  };

  add_row(tr("Path"), property_index.data(PropertyTreeModel::PathRole));
  add_row(tr("Type"), property_index.data(PropertyTreeModel::TypeRole));
  add_row(tr("Required"), property_index.data(PropertyTreeModel::RequiredRole).toBool() ? tr("Yes") : tr("No"));
  add_row(tr("Default"), property_index.data(PropertyTreeModel::DefaultRole));
  add_row(tr("Minimum"), property_index.data(PropertyTreeModel::MinimumRole));
  add_row(tr("Maximum"), property_index.data(PropertyTreeModel::MaximumRole));
  add_row(tr("Minimum Length"), property_index.data(PropertyTreeModel::MinimumLengthRole));
  add_row(tr("Maximum Length"), property_index.data(PropertyTreeModel::MaximumLengthRole));
  add_row(tr("Group"), property_index.data(PropertyTreeModel::GroupRole));
  const QStringList enum_values = property_index.data(PropertyTreeModel::EnumValuesRole).toStringList();
  if (!enum_values.isEmpty())
    add_row(tr("Choices"), enum_values.join(QStringLiteral(", ")));

  QString details = QStringLiteral("<h3>%1</h3><table cellspacing=\"4\">%2</table>")
                        .arg(property_index.data(Qt::DisplayRole).toString().toHtmlEscaped(), rows.join(QString()));
  const QString documentation = property_index.data(PropertyTreeModel::DocumentationRole).toString();
  if (!documentation.isEmpty())
    details += QStringLiteral("<p>%1</p>").arg(documentation.toHtmlEscaped());
  const QStringList errors = property_index.data(PropertyTreeModel::ValidationErrorsRole).toStringList();
  if (!errors.isEmpty())
  {
    QStringList escaped_errors;
    for (const auto& error : errors)
      escaped_errors.push_back(error.toHtmlEscaped());
    details += QStringLiteral("<p><b>%1</b><br><span style=\"color:#c62828\">%2</span></p>")
                   .arg(tr("Validation"), escaped_errors.join(QStringLiteral("<br>")));
  }
  data_->details->setText(details);
}

void PropertyTreeEditorWidget::updateYamlPreview()
{
  YAML::Emitter emitter;
  emitter << yaml();
  if (emitter.good())
    data_->yaml_preview->setPlainText(QString::fromUtf8(emitter.c_str()));
  else
    data_->yaml_preview->setPlainText(
        tr("Unable to serialize the current YAML draft: %1").arg(QString::fromStdString(emitter.GetLastError())));
}

void PropertyTreeEditorWidget::setPluginSchemaState(PluginSchemaState state, QString message)
{
  if (data_->plugin_schema_state == state && data_->plugin_schema_error == message)
    return;

  data_->plugin_schema_state = state;
  data_->plugin_schema_error = std::move(message);
  data_->plugin_error->setText(data_->plugin_schema_error);
  data_->plugin_error->setVisible(state == PluginSchemaState::Failed && !data_->plugin_schema_error.isEmpty());
  Q_EMIT pluginSchemaStateChanged(state, data_->plugin_schema_error);
}
}  // namespace tesseract::gui
