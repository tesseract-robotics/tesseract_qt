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

#include <tesseract_qt/studio/studio_plugin_config_widget.h>
#include <tesseract_qt/common/models/component_info_model.h>

#include <QStandardItemModel>
#include <QStringListModel>
#include <QStringList>
#include <QSet>

namespace tesseract_gui
{
StudioPluginConfigWidget::StudioPluginConfigWidget(QWidget* parent)
  : QWidget(parent), component_info_ns_(std::make_unique<QStringListModel>())
{
}

StudioPluginConfigWidget::~StudioPluginConfigWidget() = default;

void StudioPluginConfigWidget::setComponentInfoModel(std::shared_ptr<const ComponentInfoModel> model)
{
  if (model_ != nullptr)
  {
    disconnect(model_.get(),
               &ComponentInfoModel::rowsInserted,
               this,
               &StudioPluginConfigWidget::onComponentInfoModelRowsInserted);
    disconnect(model_.get(),
               &ComponentInfoModel::rowsRemoved,
               this,
               &StudioPluginConfigWidget::onComponentInfoModelRowsRemoved);
  }

  model_ = model;
  onComponentInfoModelRowsInserted();

  connect(model_.get(),
          &ComponentInfoModel::rowsInserted,
          this,
          &StudioPluginConfigWidget::onComponentInfoModelRowsInserted);
  connect(
      model_.get(), &ComponentInfoModel::rowsRemoved, this, &StudioPluginConfigWidget::onComponentInfoModelRowsRemoved);
}

std::shared_ptr<const ComponentInfoModel> StudioPluginConfigWidget::getComponentInfoModel() const { return model_; }

void StudioPluginConfigWidget::onComponentInfoModelRowsRemoved()
{
  QStringList current_list = component_info_ns_->stringList();
  QStringList updated_list = model_->getNamespaces();
  QSet removed = current_list.toSet().subtract(updated_list.toSet());

  component_info_ns_->setStringList(model_->getNamespaces());
}

void StudioPluginConfigWidget::onComponentInfoModelRowsInserted()
{
  QStringList current_list = component_info_ns_->stringList();
  QStringList updated_list = model_->getNamespaces();
  QSet added = updated_list.toSet().subtract(current_list.toSet());

  component_info_ns_->setStringList(model_->getNamespaces());
}

}  // namespace tesseract_gui
