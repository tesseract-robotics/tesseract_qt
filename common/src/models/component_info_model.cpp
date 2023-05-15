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

ComponentInfoModel::ComponentInfoModel(QObject* parent) : ComponentInfoModel(ComponentInfoVector(), parent) {}

ComponentInfoModel::ComponentInfoModel(const ComponentInfoVector& component_infos, QObject* parent)
  : QStandardItemModel(parent), data_(std::make_unique<Implementation>())
{
  set(component_infos);
}

ComponentInfoModel::ComponentInfoModel(const ComponentInfoModel& other)
  : QStandardItemModel(other.d_ptr->parent), data_(std::make_unique<Implementation>())
{
}

ComponentInfoModel::~ComponentInfoModel() = default;

ComponentInfoModel& ComponentInfoModel::operator=(const ComponentInfoModel& other) { return *this; }

std::vector<std::string> ComponentInfoModel::getNamespaces() const
{
  std::vector<std::string> ns;
  ns.reserve(data_->items.size());
  for (const auto& pair : data_->items)
    ns.push_back(pair.first);

  return ns;
}

const ComponentInfo& ComponentInfoModel::getComponentInfo(const std::string& ns) const
{
  auto it = data_->items.find(ns);
  if (it == data_->items.end())
    throw std::runtime_error("ComponentInfoModel, does not have ComponentInfo under namespace: '" + ns + "'");

  return *(it->second->component_info);
}

ComponentInfoVector ComponentInfoModel::getComponentInfos() const
{
  ComponentInfoVector component_infos;
  component_infos.reserve(data_->items.size());
  for (const auto& pair : data_->items)
    component_infos.push_back(*(pair.second->component_info));

  return component_infos;
}

void ComponentInfoModel::set(const ComponentInfoVector& component_infos)
{
  clear();

  for (const auto& component_info : component_infos)
    add(component_info);
}

void ComponentInfoModel::add(const ComponentInfo& component_info)
{
  if (data_->items.find(component_info.getNamespace()) != data_->items.end())
    remove(component_info.getNamespace());

  auto* item = new ComponentInfoStandardItem(QString::fromStdString(component_info.getNamespace()), component_info);
  appendRow(item);
  data_->items[component_info.getNamespace()] = item;
}

void ComponentInfoModel::remove(const std::string& ns)
{
  auto it = data_->items.find(ns);
  if (it == data_->items.end())
    return;

  std::vector<ComponentInfoStandardItem*> remove_items;
  remove_items.push_back(it->second);
  ComponentInfo ci = *(it->second->component_info);
  for (const auto& pair : data_->items)
  {
    if (pair.first != ns && ci.isChild(*(pair.second->component_info)))
      remove_items.push_back(pair.second);
  }

  // Remove item and all of its children
  for (const auto& ri : remove_items)
  {
    std::string rns = ri->component_info->getNamespace();
    QModelIndex mi = QStandardItemModel::indexFromItem(ri);
    QStandardItemModel::removeRow(mi.row(), mi.parent());
    data_->items.erase(rns);
  }
}

void ComponentInfoModel::clear()
{
  QStandardItemModel::clear();
  setColumnCount(2);
  setHorizontalHeaderLabels({ "Namespace", "Component Info" });
  data_->items.clear();
}

}  // namespace tesseract_gui
