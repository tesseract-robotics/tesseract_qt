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
#include <boost/uuid/uuid_io.hpp>

#include <QStringListModel>

namespace tesseract_gui
{
struct CreateChildComponentInfoDialog::Implementation
{
  std::shared_ptr<const ComponentInfo> parent_component_info;

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

std::shared_ptr<ComponentInfo> CreateChildComponentInfoDialog::getComponentInfo() const
{
  std::string name = ui->name_line_edit->text().toStdString();

  std::shared_ptr<ComponentInfo> child;
  if (name.empty())
    child = data_->parent_component_info->createChild();
  else
    child = data_->parent_component_info->createChild(name);

  child->setDescription(ui->description_line_edit->text().toStdString());
  return child;
}

void CreateChildComponentInfoDialog::reset(std::shared_ptr<const ComponentInfo> parent_component_info)
{
  data_->parent_component_info = std::move(parent_component_info);
  ui->scene_name_line_edit->setText(QString::fromStdString(data_->parent_component_info->getSceneName()));
  ui->description_line_edit->setText(QString::fromStdString(data_->parent_component_info->getDescription()));
  QStringList lineage;
  lineage.append(QString::fromStdString(boost::uuids::to_string(data_->parent_component_info->getNamespace())));
  for (const auto& l : data_->parent_component_info->getLineage())
    lineage.append(QString::fromStdString(boost::uuids::to_string(l)));

  data_->lineage_model.setStringList(lineage);
}
}  // namespace tesseract_gui
