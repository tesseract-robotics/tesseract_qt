/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
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
#include <tesseract_qt/common/contact_managers_plugin_info_standard_item.h>
#include <tesseract_qt/common/plugin_info_container_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
ContactManagersPluginInfoStandardItem::ContactManagersPluginInfoStandardItem(
    tesseract_common::ContactManagersPluginInfo plugin_info)
  : QStandardItem(icons::getCubeIcon(), "Contact Managers Plugin Info"), plugin_info(std::move(plugin_info))
{
  ctor();
}

ContactManagersPluginInfoStandardItem::ContactManagersPluginInfoStandardItem(
    const QString& text,
    tesseract_common::ContactManagersPluginInfo plugin_info)
  : QStandardItem(icons::getCubeIcon(), text), plugin_info(std::move(plugin_info))
{
  ctor();
}

ContactManagersPluginInfoStandardItem::ContactManagersPluginInfoStandardItem(
    const QIcon& icon,
    const QString& text,
    tesseract_common::ContactManagersPluginInfo plugin_info)
  : QStandardItem(icon, text), plugin_info(std::move(plugin_info))
{
  ctor();
}

int ContactManagersPluginInfoStandardItem::type() const
{
  return static_cast<int>(StandardItemType::COMMON_KINEMATICS_PLUGIN_INFO);
}

void ContactManagersPluginInfoStandardItem::ctor()
{
  auto* search_paths = new QStandardItem("search paths");
  std::size_t cnt{ 0 };
  for (const auto& search_path : plugin_info.search_paths)
    search_paths->appendRow(createStandardItemString(QString("[%1]").arg(cnt++).toStdString(), search_path));

  appendRow({ search_paths, new QStandardItem() });

  auto* search_libraries = new QStandardItem("search libraries");
  cnt = 0;
  for (const auto& search_library : plugin_info.search_libraries)
    search_libraries->appendRow(createStandardItemString(QString("[%1]").arg(cnt++).toStdString(), search_library));

  appendRow({ search_libraries, new QStandardItem() });

  appendRow(new PluginInfoContainerStandardItem(
      icons::getCollisionIcon(), "Discrete Plugins", plugin_info.discrete_plugin_infos));
  appendRow(new PluginInfoContainerStandardItem(
      icons::getCollisionIcon(), "Continuous Plugins", plugin_info.continuous_plugin_infos));
}
}  // namespace tesseract_gui
