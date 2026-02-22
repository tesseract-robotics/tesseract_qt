/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2024 Levi Armstrong <levi.armstrong@gmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * @par
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <tesseract_qt/planning/widgets/task_composer_tool_bar.h>
#include <tesseract_qt/common/events/task_composer_events.h>
#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_qt/common/component_info.h>
#include <QApplication>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QMessageBox>

namespace tesseract::gui
{
struct TaskComposerToolBar::Implementation
{
  std::shared_ptr<const ComponentInfo> component_info;

  QAction* load_config_action{ nullptr };
  QAction* load_log_action{ nullptr };
  QAction* save_log_action{ nullptr };
  QAction* plot_dotgraph_action{ nullptr };

  QAction* component_info_action{ nullptr };

  QString default_config_dir;
  QString default_log_dir;
};

TaskComposerToolBar::TaskComposerToolBar(QWidget* parent) : TaskComposerToolBar(nullptr, parent) {}

TaskComposerToolBar::TaskComposerToolBar(std::shared_ptr<const ComponentInfo> component_info, QWidget* parent)
  : QToolBar(parent), data_(std::make_unique<Implementation>())
{
  data_->component_info = std::move(component_info);

  QSettings ms;
  ms.beginGroup("TaskComposerToolBar");
  data_->default_config_dir =
      ms.value("default_config_dir", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0])
          .toString();
  data_->default_log_dir =
      ms.value("default_log_dir", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]).toString();
  ms.endGroup();

  data_->load_config_action = addAction(icons::getOpenIcon(), "Load Task Composer Server Config", [this]() {
    QStringList filters;
    filters.append("Task Composer Config (*.yaml)");

    QFileDialog dialog(this, "Open Task Composer Config", data_->default_config_dir);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
    dialog.setModal(true);
    dialog.setNameFilters(filters);
    if (dialog.exec() == 1)
    {
      QString config_filepath = dialog.selectedFiles().first();
      if (!QFileInfo(config_filepath).exists())
      {
        QMessageBox messageBox;
        messageBox.critical(nullptr, "Error", "Config file does not exist");
        messageBox.setFixedSize(500, 200);
        messageBox.show();
        return;
      }

      data_->default_config_dir = QFileInfo(config_filepath).absoluteDir().path();
      events::TaskComposerLoadConfig event(data_->component_info, config_filepath.toStdString());
      QApplication::sendEvent(qApp, &event);
      data_->load_log_action->setEnabled(true);
    }
  });

  data_->load_log_action = addAction(icons::getLogIcon(), "Load Task Composer Log", [this]() {
    QStringList filters;
    filters.append("Task Composer Log (*.tclx *.tclj *.tclb)");

    QFileDialog dialog(this, "Open Task Composer Log", data_->default_log_dir);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
    dialog.setModal(true);
    dialog.setNameFilters(filters);
    if (dialog.exec() == 1)
    {
      QString config_filepath = dialog.selectedFiles().first();
      if (!QFileInfo(config_filepath).exists())
      {
        QMessageBox messageBox;
        messageBox.critical(nullptr, "Error", "Log file does not exist");
        messageBox.setFixedSize(500, 200);
        messageBox.show();
        return;
      }

      data_->default_log_dir = QFileInfo(config_filepath).absoluteDir().path();
      events::TaskComposerLoadLog event(data_->component_info, config_filepath.toStdString());
      QApplication::sendEvent(qApp, &event);
    }
  });
  data_->load_log_action->setEnabled(false);

  addSeparator();

  data_->save_log_action = addAction(icons::getSaveIcon(), "Save Task Composer Log", [this]() {
    QStringList filters;
    filters.append("Task Composer Log (*.tclx *.tclj *.tclb)");

    QFileDialog dialog(this, "Save Task Composer Log", data_->default_log_dir);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
    dialog.setModal(true);
    dialog.setNameFilters(filters);
    if (dialog.exec() == 1)
    {
      QString config_filepath = dialog.selectedFiles().first();
      data_->default_log_dir = QFileInfo(config_filepath).absoluteDir().path();
      events::TaskComposerSaveLog event(data_->component_info, config_filepath.toStdString());
      QApplication::sendEvent(qApp, &event);
    }
  });

  addSeparator();

  data_->plot_dotgraph_action = addAction(icons::getPlotIcon(), "Plot Task Composer Dotgraph", [this]() {
    events::TaskComposerPlotDotgraph event(data_->component_info);
    QApplication::sendEvent(qApp, &event);
  });

  addSeparator();

  data_->component_info_action = addAction(QIcon::fromTheme("dialog-information"), "Component Info", [this]() {
    if (data_->component_info != nullptr)
      QMessageBox::information(
          this, "Component Information", QString::fromStdString(data_->component_info->toString()));
    else
      QMessageBox::information(this, "Component Information", "Null");
  });
}

TaskComposerToolBar::~TaskComposerToolBar()
{
  {
    QSettings ms;
    ms.beginGroup("TaskComposerToolBar");
    ms.setValue("default_config_dir", data_->default_config_dir);
    ms.setValue("default_log_dir", data_->default_log_dir);
    ms.endGroup();
  }
}

void TaskComposerToolBar::setComponentInfo(std::shared_ptr<const ComponentInfo> component_info)
{
  data_->component_info = std::move(component_info);
}
std::shared_ptr<const ComponentInfo> TaskComposerToolBar::getComponentInfo() const { return data_->component_info; }

}  // namespace tesseract::gui
