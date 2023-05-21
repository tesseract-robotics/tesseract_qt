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
#include <tesseract_qt/common/widgets/create_component_info_dialog.h>
#include "ui_create_component_info_dialog.h"

#include <tesseract_qt/common/component_info.h>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace tesseract_gui
{
CreateComponentInfoDialog::CreateComponentInfoDialog(QWidget* parent)
  : QDialog(parent), ui(std::make_unique<Ui::CreateComponentInfoDialog>())
{
  ui->setupUi(this);
  reset();
}

CreateComponentInfoDialog::~CreateComponentInfoDialog() = default;

void CreateComponentInfoDialog::setSceneNames(const QStringList& scene_names)
{
  ui->scene_name_combo_box->clear();
  ui->scene_name_combo_box->addItems(scene_names);
}

std::shared_ptr<ComponentInfo> CreateComponentInfoDialog::getComponentInfo() const
{
  //  const std::string scene_name = ui->scene_name_combo_box->currentText().toStdString();
  //  const boost::uuids::uuid ns = boost::lexical_cast<boost::uuids::uuid>(ui->ns_line_edit->text().toStdString());
  //  const std::string description = ui->description_line_edit->text().toStdString();
  //  return ComponentInfo(scene_name, { ns }, description);
  return nullptr;
}

void CreateComponentInfoDialog::reset()
{
  ui->scene_name_combo_box->setCurrentText("tesseract_default");
  ui->ns_line_edit->setText(QString::fromStdString(boost::uuids::to_string(boost::uuids::random_generator()())));
  ui->description_line_edit->setText("");
}
}  // namespace tesseract_gui
