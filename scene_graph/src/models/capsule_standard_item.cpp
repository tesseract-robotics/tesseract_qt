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
#include <tesseract_qt/scene_graph/models/capsule_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_geometry/impl/capsule.h>

namespace tesseract::gui
{
CapsuleStandardItem::CapsuleStandardItem(std::shared_ptr<const tesseract::geometry::Capsule> capsule)
  : QStandardItem(icons::getCapsuleIcon(), "Capsule"), capsule(std::move(capsule))
{
  ctor();
}

CapsuleStandardItem::CapsuleStandardItem(const QString& text,
                                         std::shared_ptr<const tesseract::geometry::Capsule> capsule)
  : QStandardItem(icons::getCapsuleIcon(), text), capsule(std::move(capsule))
{
  ctor();
}

CapsuleStandardItem::CapsuleStandardItem(const QIcon& icon,
                                         const QString& text,
                                         std::shared_ptr<const tesseract::geometry::Capsule> capsule)
  : QStandardItem(icon, text), capsule(std::move(capsule))
{
  ctor();
}

int CapsuleStandardItem::type() const { return static_cast<int>(StandardItemType::SG_CAPSULE); }

void CapsuleStandardItem::ctor()
{
  appendRow(createStandardItemFloat("radius", capsule->getRadius()));
  appendRow(createStandardItemFloat("length", capsule->getLength()));
}
}  // namespace tesseract::gui
