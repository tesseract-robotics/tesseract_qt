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

#include <tesseract_qt/common/models/component_info_manager_model.h>
#include <tesseract_qt/common/models/component_info_standard_item.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/component_info_manager.h>

#include <boost/uuid/uuid_hash.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <QApplication>

namespace tesseract_gui
{
struct ComponentInfoManagerModel::Implementation
{
  std::map<boost::uuids::uuid, ComponentInfoStandardItem*> items;
};

ComponentInfoManagerModel::ComponentInfoManagerModel(QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
}

ComponentInfoManagerModel::ComponentInfoManagerModel(const ComponentInfoManagerModel& other)
  : QStandardItemModel(other.d_ptr->parent), data_(std::make_unique<Implementation>())
{
}

ComponentInfoManagerModel::~ComponentInfoManagerModel() = default;

ComponentInfoManagerModel& ComponentInfoManagerModel::operator=(const ComponentInfoManagerModel& other)
{
  return *this;
}

void ComponentInfoManagerModel::refresh()
{
  clear();

  std::vector<std::shared_ptr<ComponentInfo>> component_infos = ComponentInfoManager::get();

  for (const auto& component_info : component_infos)
  {
    auto* item = new ComponentInfoStandardItem(QString::fromStdString(component_info->getName()), component_info);
    data_->items[component_info->getNamespace()] = item;
    appendRow(item);
  }
}

void ComponentInfoManagerModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Component Info" });
  data_->items.clear();
}

}  // namespace tesseract_gui
