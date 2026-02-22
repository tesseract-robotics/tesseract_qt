#include <tesseract_qt/tool_path/widgets/tool_path_file_dialog.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/environment_manager.h>
#include <tesseract_qt/common/environment_wrapper.h>

#include <tesseract_environment/environment.h>

#include "ui_tool_path_file_dialog.h"

#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QStringList>

namespace tesseract::gui
{
ToolPathFileDialog::ToolPathFileDialog(QFileDialog::AcceptMode accept_mode, QWidget* parent)
  : ToolPathFileDialog(nullptr, accept_mode, parent)
{
}

ToolPathFileDialog::ToolPathFileDialog(std::shared_ptr<const ComponentInfo> component_info,
                                       QFileDialog::AcceptMode accept_mode,
                                       QWidget* parent)
  : QDialog(parent)
  , ui_(std::make_unique<Ui::ToolPathFileDialog>())
  , component_info_(std::move(component_info))
  , accept_mode_(accept_mode)
{
  ui_->setupUi(this);

  QSettings ms;
  ms.beginGroup("ToolPathFileDialog");
  default_directory_ =
      ms.value("default_directory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]).toString();
  ms.endGroup();

  connect(ui_->file_path_push_button, SIGNAL(clicked()), this, SLOT(onBrowseClicked()));
}

ToolPathFileDialog::~ToolPathFileDialog()
{
  QSettings ms;
  ms.beginGroup("ToolPathFileDialog");
  ms.setValue("default_directory", default_directory_);
  ms.endGroup();
}
void ToolPathFileDialog::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  component_info_ = std::move(component_info);
}
std::shared_ptr<const ComponentInfo> ToolPathFileDialog::getComponentInfo() const { return component_info_; }

void ToolPathFileDialog::setAcceptMode(QFileDialog::AcceptMode accept_mode) { accept_mode_ = accept_mode; }

QString ToolPathFileDialog::getLinkName() const { return ui_->frame_combo_box->currentText(); }
QString ToolPathFileDialog::getFilePath() const { return ui_->file_path_line_edit->text(); }

void ToolPathFileDialog::showEvent(QShowEvent* e)
{
  // If an environment has already been assigned load the data
  auto env_wrapper = EnvironmentManager::get(component_info_);
  if (env_wrapper != nullptr && env_wrapper->getEnvironment()->isInitialized())
  {
    QStringList list;
    list.append("__NULL__");
    std::vector<std::string> link_names = env_wrapper->getEnvironment()->getLinkNames();
    for (const auto& link_name : link_names)
      list.append(QString::fromStdString(link_name));

    list.sort();
    ui_->frame_combo_box->clear();
    ui_->frame_combo_box->addItems(list);
  }

  QDialog::showEvent(e);
}

void ToolPathFileDialog::onBrowseClicked()
{
  QString title;
  QStringList dialog_filters;

  if (accept_mode_ == QFileDialog::AcceptOpen)
  {  // Setup open dialog
    title = "Open Tool Path";

    dialog_filters.append("Tool Path XML (*.tpx)");
    dialog_filters.append("Tool Path Binary (*.tpb)");
    dialog_filters.append("Tool Path YAML (*.yaml)");
    dialog_filters.append("Tool Path Json (*.json)");
  }
  else
  {  // Setup save dialog
    title = "Save Tool Path";

    dialog_filters.append("Tool Path XML (*.tpx)");
    dialog_filters.append("Tool Path Binary (*.tpb)");
    dialog_filters.append("Tool Path YAML (*.yaml)");
  }

  QFileDialog dialog(this, title, default_directory_);
  dialog.setAcceptMode(accept_mode_);
  dialog.setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  dialog.setModal(true);
  dialog.setNameFilters(dialog_filters);
  if (dialog.exec() == 1)
  {
    QString file_path = dialog.selectedFiles().first();

    if (accept_mode_ == QFileDialog::AcceptSave)
    {
      int idx = dialog_filters.indexOf(dialog.selectedNameFilter());
      if (idx == 0)
      {
        if (!file_path.endsWith(".tpx"))
          file_path += ".tpx";
      }
      else if (idx == 1)
      {
        if (!file_path.endsWith(".tpb"))
          file_path += ".tpb";
      }
      else if (idx == 2)
      {
        if (!file_path.endsWith(".yaml"))
          file_path += ".yaml";
      }
      else if (idx == 3)
      {
        if (!file_path.endsWith(".json"))
          file_path += ".json";
      }
    }

    ui_->file_path_line_edit->setText(file_path);
    default_directory_ = QFileInfo(file_path).absoluteDir().path();
  }
}

}  // namespace tesseract::gui
