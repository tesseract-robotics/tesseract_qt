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
#include <tesseract_qt/common/widgets/component_info_dialog.h>
#include "ui_component_info_dialog.h"

#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include <QStringListModel>
namespace tesseract_gui
{
struct ComponentInfoDialog::Implementation
{
  QStringListModel model;

  QString getKey(const ComponentInfo& component_info)
  {
    return QString("%1::%2").arg(component_info.getName().c_str(),
                                 boost::uuids::to_string(component_info.getNamespace()).c_str());
  }

  void updateModel()
  {
    auto component_infos = ComponentInfoManager::get();

    QStringList list;
    for (const auto& component_info : component_infos)
      list.append(getKey(*component_info));

    model.setStringList(list);
  }
};

ComponentInfoDialog::ComponentInfoDialog(QWidget* parent)
  : QDialog(parent), ui(std::make_unique<Ui::ComponentInfoDialog>()), data_(std::make_unique<Implementation>())
{
  ui->setupUi(this);
  ui->component_info_manager_widget->enableEditMode(true);
  ui->component_info_combo_box->setModel(&data_->model);

  setWindowTitle("Component Info Selector");
  connect(ui->component_info_manager_widget, SIGNAL(refreshed()), this, SLOT(onRefreshed()));

  ui->component_info_manager_widget->refresh();
}

ComponentInfoDialog::~ComponentInfoDialog() = default;

std::shared_ptr<const ComponentInfo> ComponentInfoDialog::getComponentInfo() const
{
  QString current_text = ui->component_info_combo_box->currentText();
  if (current_text.isEmpty())
    return nullptr;

  QStringList list = current_text.split("::");
  if (list.size() != 2)
    return nullptr;

  auto uuid = boost::lexical_cast<boost::uuids::uuid>(list[1].toStdString());
  return ComponentInfoManager::get(uuid);
}

QStringListModel& ComponentInfoDialog::getModel() const { return data_->model; }

void ComponentInfoDialog::onRefreshed()
{
  QString current_text = ui->component_info_combo_box->currentText();
  data_->updateModel();
  if (!current_text.isEmpty())
  {
    QStringList list = current_text.split("::");
    if (list.size() != 2)
      return;

    auto uuid = boost::lexical_cast<boost::uuids::uuid>(list[1].toStdString());
    if (ComponentInfoManager::get(uuid) != nullptr)
      ui->component_info_combo_box->setCurrentText(current_text);
  }
}
}  // namespace tesseract_gui
