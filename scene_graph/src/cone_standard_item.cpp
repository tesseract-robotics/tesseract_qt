/**
 * @author Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @copyright Copyright (C) 2022 Levi Armstrong <levi.armstrong@gmail.com>
 *
 * @par License
 * GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
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
#include <tesseract_qt/scene_graph/cone_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
ConeStandardItem::ConeStandardItem(tesseract_geometry::Cone::Ptr cone)
  : QStandardItem(icons::getConeIcon(), "Cone"), cone(std::move(cone))
{
  ctor();
}

ConeStandardItem::ConeStandardItem(const QString& text, tesseract_geometry::Cone::Ptr cone)
  : QStandardItem(icons::getConeIcon(), text), cone(std::move(cone))
{
  ctor();
}

ConeStandardItem::ConeStandardItem(const QIcon& icon, const QString& text, tesseract_geometry::Cone::Ptr cone)
  : QStandardItem(icon, text), cone(std::move(cone))
{
  ctor();
}

int ConeStandardItem::type() const { return static_cast<int>(StandardItemType::SG_CONE); }

void ConeStandardItem::ctor()
{
  appendRow(createStandardItemFloat("radius", cone->getRadius()));
  appendRow(createStandardItemFloat("length", cone->getLength()));
}
}  // namespace tesseract_gui
