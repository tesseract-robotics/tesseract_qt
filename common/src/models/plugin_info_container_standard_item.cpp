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
#include <tesseract_qt/common/models/plugin_info_container_standard_item.h>
#include <tesseract_qt/common/models/plugin_info_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
PluginInfoContainerStandardItem::PluginInfoContainerStandardItem(
    tesseract_common::PluginInfoContainer plugin_info_container)
  : QStandardItem(icons::getCubeIcon(), "Plugin Info Container")
  , plugin_info_container(std::move(plugin_info_container))
{
  ctor();
}

PluginInfoContainerStandardItem::PluginInfoContainerStandardItem(
    const QString& text,
    tesseract_common::PluginInfoContainer plugin_info_container)
  : QStandardItem(icons::getCubeIcon(), text), plugin_info_container(std::move(plugin_info_container))
{
  ctor();
}

PluginInfoContainerStandardItem::PluginInfoContainerStandardItem(
    const QIcon& icon,
    const QString& text,
    tesseract_common::PluginInfoContainer plugin_info_container)
  : QStandardItem(icon, text), plugin_info_container(std::move(plugin_info_container))
{
  ctor();
}

int PluginInfoContainerStandardItem::type() const
{
  return static_cast<int>(StandardItemType::COMMON_PLUGIN_INFO_CONTAINER);
}

void PluginInfoContainerStandardItem::ctor()
{
  appendRow(createStandardItemString("default", plugin_info_container.default_plugin));

  auto* plugins = new QStandardItem(icons::getCubeIcon(), "plugins");
  for (const auto& plugin : plugin_info_container.plugins)
    plugins->appendRow(new PluginInfoStandardItem(plugin.first.c_str(), plugin.second));

  appendRow(plugins);
}
}  // namespace tesseract_gui
