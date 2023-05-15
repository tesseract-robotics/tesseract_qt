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
#include <tesseract_qt/common/component_info_standard_item.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
ComponentInfoStandardItem::ComponentInfoStandardItem(ComponentInfo component_info)
  : ComponentInfoStandardItem(icons::getConvexMeshIcon(), "Component Info", std::move(component_info))
{
}

ComponentInfoStandardItem::ComponentInfoStandardItem(const QString& text, ComponentInfo component_info)
  : ComponentInfoStandardItem(icons::getConvexMeshIcon(), text, std::move(component_info))
{
}

ComponentInfoStandardItem::ComponentInfoStandardItem(const QIcon& icon,
                                                     const QString& text,
                                                     ComponentInfo component_info)
  : QStandardItem(icon, text), component_info(std::make_unique<ComponentInfo>(std::move(component_info)))
{
  appendRow(createStandardItemString("Scene Name", this->component_info->getSceneName()));
  appendRow(createStandardItemString("Namespace", this->component_info->getNamespace()));
  if (this->component_info->hasParent())
  {
    auto* item = new QStandardItem("Lineage");
    std::list<std::string> lineage = this->component_info->getLineage();
    std::size_t i{ 0 };
    for (const auto& n : lineage)
      item->appendRow(createStandardItemString(QString("[%1]").arg(i++).toStdString(), n));

    appendRow(item);
  }
}

int ComponentInfoStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_COMPONENT_INFO); }

}  // namespace tesseract_gui
