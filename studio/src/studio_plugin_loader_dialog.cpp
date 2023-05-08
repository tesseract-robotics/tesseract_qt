/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2023 Levi Armstrong <levi.armstrong@gmail.com>
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
#include <tesseract_qt/studio/studio_plugin_loader_dialog.h>
#include <tesseract_qt/studio/studio_plugin_factory.h>
#include "ui_studio_plugin_loader_dialog.h"

#include <tesseract_qt/common/icon_utils.h>
#include <tesseract_common/plugin_loader.h>

#include <QMenu>

namespace tesseract_gui
{
struct StudioPluginLoaderDialog::Implementation
{
  QMenu contect_menu;
  std::shared_ptr<tesseract_gui::StudioPluginFactory> plugin_factory;
};

StudioPluginLoaderDialog::StudioPluginLoaderDialog(std::shared_ptr<tesseract_gui::StudioPluginFactory> plugin_factory,
                                                   QWidget* parent)
  : QDialog(parent), ui(std::make_unique<Ui::StudioPluginLoaderDialog>()), data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);

  setWindowIcon(icons::getPluginIcon());
  setWindowModality(Qt::ApplicationModal);  // Required, see RenderWidget::onFrameSwapped()
  setModal(true);

  data_->plugin_factory = std::move(plugin_factory);
  data_->contect_menu.addAction(ui->actionRemove_Plugin);
  data_->contect_menu.setWindowModality(Qt::ApplicationModal);

  ui->actionRemove_Plugin->setIcon(icons::getTrashIcon());
  connect(ui->actionRemove_Plugin, &QAction::triggered, [this]() {
    int current_index = ui->tool_box->currentIndex();
    if (current_index >= 0)
      ui->tool_box->removeItem(current_index);
  });

  connect(ui->tool_box, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

StudioPluginLoaderDialog::~StudioPluginLoaderDialog() = default;

void StudioPluginLoaderDialog::showEvent(QShowEvent* event)
{
  std::vector<std::string> plugins =
      data_->plugin_factory->getPluginLoader().getAvailablePlugins(StudioConfigWidgetFactory::getSectionName());
  ui->combo_box->clear();
  for (const auto& plugin : plugins)
    ui->combo_box->addItem(QString::fromStdString(plugin));

  QDialog::showEvent(event);
}

void StudioPluginLoaderDialog::showContextMenu(const QPoint& pos)
{
  QPoint global_pos = ui->tool_box->mapToGlobal(pos);
  if (ui->tool_box->currentWidget() != nullptr)
  {
    QPoint local_pos = ui->tool_box->currentWidget()->mapFromGlobal(global_pos);
    QRect rect = ui->tool_box->currentWidget()->rect();
    rect.setTop(-30);  // Account for page header
    if (rect.contains(local_pos))
      data_->contect_menu.exec(global_pos);
  }
}
}  // namespace tesseract_gui
