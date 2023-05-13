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

#include <tesseract_qt/common/models/component_info_model.h>
#include <tesseract_qt/common/component_info_standard_item.h>
#include <tesseract_qt/common/component_info.h>

#include <QApplication>

namespace tesseract_gui
{
struct ComponentInfoModel::Implementation
{
  std::unordered_map<std::string, ComponentInfoStandardItem*> items;
};

ComponentInfoModel::ComponentInfoModel(QObject* parent) : ComponentInfoModel(ComponentInfoMap(), parent) {}

ComponentInfoModel::ComponentInfoModel(const ComponentInfoMap& component_infos, QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  clear();
}

ComponentInfoModel::ComponentInfoModel(const ComponentInfoModel& other)
  : QStandardItemModel(other.d_ptr->parent), data_(std::make_unique<Implementation>())
{
}

ComponentInfoModel::~ComponentInfoModel() = default;

ComponentInfoModel& ComponentInfoModel::operator=(const ComponentInfoModel& other) { return *this; }

std::vector<std::string> ComponentInfoModel::getKeys() const
{
  std::vector<std::string> keys;
  keys.reserve(data_->items.size());
  for (const auto& pair : data_->items)
    keys.push_back(pair.first);

  return keys;
}

const ComponentInfo& ComponentInfoModel::getComponentInfo(const std::string& key) const
{
  auto it = data_->items.find(key);
  if (it == data_->items.end())
    throw std::runtime_error("ComponentInfoModel, does not have ComponentInfo under key: '" + key + "'");

  return *(it->second->component_info);
}

ComponentInfoMap ComponentInfoModel::getComponentInfos() const
{
  ComponentInfoMap component_infos;
  for (const auto& pair : data_->items)
    component_infos[pair.first] = *(pair.second->component_info);

  return component_infos;
}

void ComponentInfoModel::set(const ComponentInfoMap& component_infos)
{
  clear();

  for (const auto& pair : component_infos)
    add(pair.first, pair.second);

  sort(0);
}

void ComponentInfoModel::add(const std::string& key, const ComponentInfo& component_info)
{
  if (data_->items.find(key) != data_->items.end())
    remove(key);

  auto* item = new ComponentInfoStandardItem(QString::fromStdString(key), component_info);
  appendRow(item);
  data_->items[key] = item;

  sort(0);
}

void ComponentInfoModel::remove(const std::string& key)
{
  auto it = data_->items.find(key);
  if (it == data_->items.end())
    return;

  QModelIndex mi = QStandardItemModel::indexFromItem(it->second);
  QStandardItemModel::removeRow(mi.row(), mi.parent());
  data_->items.erase(key);

  /** @todo should probably remove all children */

  sort(0);
}

void ComponentInfoModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Name", "Values" });
  data_->items.clear();
}

}  // namespace tesseract_gui
