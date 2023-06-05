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
#include <tesseract_qt/common/models/component_info_standard_item.h>
#include <tesseract_qt/common/component_info.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>
#include <boost/uuid/uuid_io.hpp>

namespace tesseract_gui
{
ComponentInfoStandardItem::ComponentInfoStandardItem(std::shared_ptr<ComponentInfo> component_info)
  : ComponentInfoStandardItem(icons::getConvexMeshIcon(), "Component Info", std::move(component_info))
{
}

ComponentInfoStandardItem::ComponentInfoStandardItem(const QString& text, std::shared_ptr<ComponentInfo> component_info)
  : ComponentInfoStandardItem(icons::getConvexMeshIcon(), text, std::move(component_info))
{
}

ComponentInfoStandardItem::ComponentInfoStandardItem(const QIcon& icon,
                                                     const QString& text,
                                                     std::shared_ptr<ComponentInfo> component_info)
  : QStandardItem(icon, text), component_info(std::move(component_info))
{
  appendRow(createStandardItemString("Scene Name", this->component_info->getSceneName()));
  appendRow(createStandardItemString("Namespace", boost::uuids::to_string(this->component_info->getNamespace())));
  appendRow(createStandardItemString("Description", this->component_info->getDescription()));
  if (this->component_info->hasParent())
  {
    auto* item = new QStandardItem("Lineage");
    std::list<boost::uuids::uuid> lineage = this->component_info->getLineage();
    std::size_t i{ 0 };
    for (const auto& n : lineage)
      item->appendRow(createStandardItemString(QString("[%1]").arg(i++).toStdString(), boost::uuids::to_string(n)));

    appendRow({ item, new QStandardItem(boost::uuids::to_string(this->component_info->getNamespace()).c_str()) });
  }
}

int ComponentInfoStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_COMPONENT_INFO); }

}  // namespace tesseract_gui
