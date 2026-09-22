/*
 * @copyright Copyright (C) 2026 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 */
#include "config_editor_main_window.h"

#include <tesseract_qt/common/config_definition.h>
#include <tesseract_qt/common/widgets/config_editor_widget.h>

#include <QAction>
#include <QCloseEvent>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QToolBar>

#include <yaml-cpp/yaml.h>

#include <exception>
#include <utility>

namespace tesseract::gui
{
namespace
{
QString pluginStateText(PropertyTreeEditorWidget::PluginSchemaState state)
{
  using State = PropertyTreeEditorWidget::PluginSchemaState;
  switch (state)
  {
    case State::NotRequired:
      return QObject::tr("No plugin discovery required");
    case State::Current:
      return QObject::tr("Plugin schemas current");
    case State::Stale:
      return QObject::tr("Plugin schemas need refresh");
    case State::Failed:
      return QObject::tr("Plugin schema refresh failed");
  }
  return {};
}
}  // namespace

struct ConfigEditorMainWindow::Implementation
{
  ConfigEditorWidget* editor{ nullptr };
  QComboBox* type{ nullptr };
  QLabel* status{ nullptr };
  QAction* save{ nullptr };
  QAction* save_as{ nullptr };
  QAction* export_draft{ nullptr };
  QMenu* recent_files{ nullptr };
  const ConfigDefinition* definition{ nullptr };
  QStringList recent_paths;
  bool changing_type{ false };
};

ConfigEditorMainWindow::ConfigEditorMainWindow(const std::string& initial_type,
                                               const QString& initial_path,
                                               std::vector<std::string> additional_search_paths,
                                               std::vector<std::string> additional_search_libraries,
                                               QWidget* parent)
  : QMainWindow(parent), data_(std::make_unique<Implementation>())
{
  data_->definition = &configDefinition(initial_type);
  data_->editor = new ConfigEditorWidget(this);
  data_->editor->setPluginSearchPaths(std::move(additional_search_paths));
  data_->editor->setPluginSearchLibraries(std::move(additional_search_libraries));
  setCentralWidget(data_->editor);

  auto* file_menu = menuBar()->addMenu(tr("&File"));
  auto* new_action = file_menu->addAction(tr("&New"));
  new_action->setShortcut(QKeySequence::New);
  auto* open_action = file_menu->addAction(tr("&Open…"));
  open_action->setShortcut(QKeySequence::Open);
  data_->recent_files = file_menu->addMenu(tr("Open &Recent"));
  file_menu->addSeparator();
  data_->save = file_menu->addAction(tr("&Save"));
  data_->save->setShortcut(QKeySequence::Save);
  data_->save_as = file_menu->addAction(tr("Save &As…"));
  data_->save_as->setShortcut(QKeySequence::SaveAs);
  data_->export_draft = file_menu->addAction(tr("Export &Draft…"));
  file_menu->addSeparator();
  auto* quit_action = file_menu->addAction(tr("&Quit"));
  quit_action->setShortcut(QKeySequence::Quit);

  auto* edit_menu = menuBar()->addMenu(tr("&Edit"));
  edit_menu->addAction(data_->editor->editor()->undoAction());
  edit_menu->addAction(data_->editor->editor()->redoAction());
  edit_menu->addSeparator();
  edit_menu->addAction(data_->editor->editor()->focusSearchAction());

  auto* view_menu = menuBar()->addMenu(tr("&View"));
  view_menu->addAction(data_->editor->editor()->expandAllAction());
  view_menu->addAction(data_->editor->editor()->collapseAllAction());
  auto* refresh_action = view_menu->addAction(tr("Refresh Plugin Schemas"));
  refresh_action->setShortcut(QKeySequence::Refresh);

  auto* toolbar = addToolBar(tr("Configuration"));
  toolbar->setObjectName(QStringLiteral("configurationToolbar"));
  toolbar->addAction(new_action);
  toolbar->addAction(open_action);
  toolbar->addAction(data_->save);
  toolbar->addSeparator();
  toolbar->addWidget(new QLabel(tr("Configuration:"), toolbar));
  data_->type = new QComboBox(toolbar);
  for (const auto& definition : configDefinitions())
    data_->type->addItem(definition.label, QString::fromStdString(definition.id));
  data_->type->setCurrentIndex(data_->type->findData(QString::fromStdString(data_->definition->id)));
  toolbar->addWidget(data_->type);
  toolbar->addSeparator();
  toolbar->addAction(refresh_action);

  data_->status = new QLabel(this);
  statusBar()->addPermanentWidget(data_->status);

  QSettings settings;
  restoreGeometry(settings.value(QStringLiteral("configEditor/geometry")).toByteArray());
  restoreState(settings.value(QStringLiteral("configEditor/windowState")).toByteArray());
  data_->recent_paths = settings.value(QStringLiteral("configEditor/recentFiles")).toStringList();
  rebuildRecentFilesMenu();

  connect(new_action, &QAction::triggered, this, [this]() {
    if (maybeSave())
      loadCurrentDefinition();
  });
  connect(open_action, &QAction::triggered, this, [this]() { openFile(); });
  connect(data_->save, &QAction::triggered, this, [this]() { save(); });
  connect(data_->save_as, &QAction::triggered, this, [this]() { saveAs(); });
  connect(data_->export_draft, &QAction::triggered, this, [this]() { exportDraft(); });
  connect(quit_action, &QAction::triggered, this, &QWidget::close);
  connect(refresh_action, &QAction::triggered, this, [this]() {
    try
    {
      data_->editor->refreshPluginSchemas();
    }
    catch (const std::exception& exception)
    {
      QMessageBox::critical(this, tr("Plugin Refresh Failed"), QString::fromStdString(exception.what()));
    }
  });
  connect(data_->type, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
    if (data_->changing_type)
      return;
    const auto* previous = data_->definition;
    if (!maybeSave())
    {
      const QSignalBlocker blocker(data_->type);
      data_->type->setCurrentIndex(data_->type->findData(QString::fromStdString(previous->id)));
      return;
    }

    try
    {
      data_->definition = &configDefinition(data_->type->currentData().toString().toStdString());
      loadCurrentDefinition();
    }
    catch (const std::exception& exception)
    {
      data_->definition = previous;
      const QSignalBlocker blocker(data_->type);
      data_->type->setCurrentIndex(data_->type->findData(QString::fromStdString(previous->id)));
      QMessageBox::critical(this, tr("Configuration Error"), QString::fromStdString(exception.what()));
    }
  });
  connect(data_->editor, &ConfigEditorWidget::modifiedChanged, this, [this](bool) { updateWindowState(); });
  connect(data_->editor, &ConfigEditorWidget::sourcePathChanged, this, [this](const QString&) { updateWindowState(); });
  connect(data_->editor, &ConfigEditorWidget::validationChanged, this, [this](bool, const QStringList&) {
    updateWindowState();
  });
  connect(data_->editor,
          &ConfigEditorWidget::pluginSchemaStateChanged,
          this,
          [this](PropertyTreeEditorWidget::PluginSchemaState, const QString&) { updateWindowState(); });

  if (initial_path.isEmpty())
    loadCurrentDefinition();
  else
    openFile(initial_path);
  resize(1250, 800);
}

ConfigEditorMainWindow::~ConfigEditorMainWindow() = default;

void ConfigEditorMainWindow::closeEvent(QCloseEvent* event)
{
  if (!maybeSave())
  {
    event->ignore();
    return;
  }

  QSettings settings;
  settings.setValue(QStringLiteral("configEditor/geometry"), saveGeometry());
  settings.setValue(QStringLiteral("configEditor/windowState"), saveState());
  settings.setValue(QStringLiteral("configEditor/recentFiles"), data_->recent_paths);
  event->accept();
}

bool ConfigEditorMainWindow::maybeSave()
{
  if (!data_->editor->isModified())
    return true;

  const auto answer = QMessageBox::warning(this,
                                           tr("Unsaved Changes"),
                                           tr("The current configuration has unsaved changes."),
                                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                           QMessageBox::Save);
  if (answer == QMessageBox::Cancel)
    return false;
  if (answer == QMessageBox::Discard)
    return true;
  return save();
}

bool ConfigEditorMainWindow::save()
{
  if (data_->editor->sourcePath().isEmpty())
    return saveAs();

  QString error;
  if (!data_->editor->save(data_->editor->sourcePath(), &error))
  {
    QMessageBox::critical(this, tr("Save Failed"), error);
    return false;
  }
  addRecentFile(data_->editor->sourcePath());
  updateWindowState();
  return true;
}

bool ConfigEditorMainWindow::saveAs()
{
  const QString path = QFileDialog::getSaveFileName(
      this, tr("Save Configuration"), data_->editor->sourcePath(), tr("YAML files (*.yaml *.yml)"));
  if (path.isEmpty())
    return false;

  QString error;
  if (!data_->editor->save(path, &error))
  {
    QMessageBox::critical(this, tr("Save Failed"), error);
    return false;
  }
  addRecentFile(path);
  updateWindowState();
  return true;
}

void ConfigEditorMainWindow::exportDraft()
{
  const QString path = QFileDialog::getSaveFileName(
      this, tr("Export Configuration Draft"), data_->editor->sourcePath(), tr("YAML files (*.yaml *.yml)"));
  if (path.isEmpty())
    return;

  QString error;
  if (!data_->editor->exportDraft(path, &error))
    QMessageBox::critical(this, tr("Export Failed"), error);
}

void ConfigEditorMainWindow::openFile(const QString& requested_path)
{
  if (!maybeSave())
    return;

  QString path = requested_path;
  if (path.isEmpty())
    path = QFileDialog::getOpenFileName(this, tr("Open Configuration"), {}, tr("YAML files (*.yaml *.yml)"));
  if (path.isEmpty())
    return;

  try
  {
    const YAML::Node document = YAML::LoadFile(path.toStdString());
    const auto& definition = detectConfigDefinition(document);
    data_->editor->loadConfiguration(definition, document, path);
    data_->definition = &definition;
    const QSignalBlocker blocker(data_->type);
    data_->type->setCurrentIndex(data_->type->findData(QString::fromStdString(definition.id)));
    addRecentFile(path);
    updateWindowState();
  }
  catch (const std::exception& exception)
  {
    QMessageBox::critical(this, tr("Open Failed"), QString::fromStdString(exception.what()));
  }
}

void ConfigEditorMainWindow::loadCurrentDefinition(const QString& path)
{
  data_->editor->loadConfiguration(*data_->definition, data_->definition->empty_document, path);
  updateWindowState();
}

void ConfigEditorMainWindow::updateWindowState()
{
  const QString path = data_->editor->sourcePath();
  const QString document_name = path.isEmpty() ? tr("Untitled") : QFileInfo(path).fileName();
  setWindowTitle(tr("%1 — %2[*]").arg(document_name, data_->definition->title));
  setWindowModified(data_->editor->isModified());

  const QString validity = data_->editor->isValid() ? tr("Valid") : tr("Invalid");
  data_->status->setText(validity + QStringLiteral(" — ") + pluginStateText(data_->editor->pluginSchemaState()));
  data_->save->setEnabled(data_->editor->isSaveReady());
  data_->save_as->setEnabled(data_->editor->isSaveReady());
  data_->export_draft->setEnabled(data_->editor->hasDocument());
}

void ConfigEditorMainWindow::addRecentFile(const QString& path)
{
  const QString canonical_path = QFileInfo(path).absoluteFilePath();
  data_->recent_paths.removeAll(canonical_path);
  data_->recent_paths.prepend(canonical_path);
  while (data_->recent_paths.size() > 10)
    data_->recent_paths.removeLast();
  rebuildRecentFilesMenu();
}

void ConfigEditorMainWindow::rebuildRecentFilesMenu()
{
  data_->recent_files->clear();
  for (const auto& path : std::as_const(data_->recent_paths))
  {
    auto* action = data_->recent_files->addAction(QFileInfo(path).fileName());
    action->setToolTip(path);
    connect(action, &QAction::triggered, this, [this, path]() { openFile(path); });
  }
  data_->recent_files->setEnabled(!data_->recent_paths.isEmpty());
}
}  // namespace tesseract::gui
