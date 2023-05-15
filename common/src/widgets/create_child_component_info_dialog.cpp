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
#include <tesseract_qt/common/widgets/create_child_component_info_dialog.h>
#include "ui_create_child_component_info_dialog.h"

#include <tesseract_qt/common/component_info.h>

#include <QStringListModel>

namespace tesseract_gui
{
struct CreateChildComponentInfoDialog::Implementation
{
  ComponentInfo component_info;

  QStringListModel lineage_model;
};

CreateChildComponentInfoDialog::CreateChildComponentInfoDialog(QWidget* parent)
  : QDialog(parent)
  , ui(std::make_unique<Ui::CreateChildComponentInfoDialog>())
  , data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);
  ui->lineage_list_view->setModel(&data_->lineage_model);
}

CreateChildComponentInfoDialog::~CreateChildComponentInfoDialog() = default;

ComponentInfo CreateChildComponentInfoDialog::getComponentInfo() const
{
  data_->component_info.setDescription(ui->description_line_edit->text().toStdString());
  return data_->component_info;
}

void CreateChildComponentInfoDialog::reset(const ComponentInfo& parent_component_info)
{
  data_->component_info = parent_component_info.createChild();
  ui->scene_name_line_edit->setText(QString::fromStdString(data_->component_info.getSceneName()));
  ui->ns_line_edit->setText(QString::fromStdString(data_->component_info.getNamespace()));
  ui->description_line_edit->setText(QString::fromStdString(data_->component_info.getDescription()));
  QStringList lineage;
  for (const auto& l : data_->component_info.getLineage())
    lineage.append(QString::fromStdString(l));

  data_->lineage_model.setStringList(lineage);
}
}  // namespace tesseract_gui
