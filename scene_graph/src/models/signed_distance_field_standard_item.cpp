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
#include <tesseract_qt/scene_graph/models/signed_distance_field_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract/geometry/impl/signed_distance_field.h>

namespace tesseract::gui
{
SignedDistanceFieldStandardItem::SignedDistanceFieldStandardItem(
    std::shared_ptr<const tesseract::geometry::SignedDistanceField> sdf)
  : QStandardItem(icons::getOctreeIcon(), "Signed Distance Field"), sdf(std::move(sdf))
{
  ctor();
}

SignedDistanceFieldStandardItem::SignedDistanceFieldStandardItem(
    const QString& text,
    std::shared_ptr<const tesseract::geometry::SignedDistanceField> sdf)
  : QStandardItem(icons::getOctreeIcon(), text), sdf(std::move(sdf))
{
  ctor();
}

SignedDistanceFieldStandardItem::SignedDistanceFieldStandardItem(
    const QIcon& icon,
    const QString& text,
    std::shared_ptr<const tesseract::geometry::SignedDistanceField> sdf)
  : QStandardItem(icon, text), sdf(std::move(sdf))
{
  ctor();
}

int SignedDistanceFieldStandardItem::type() const
{
  return static_cast<int>(StandardItemType::SG_SIGNED_DISTANCE_FIELD);
}

void SignedDistanceFieldStandardItem::ctor()
{
  const Eigen::Vector3i& dim = sdf->getDimensions();
  const Eigen::Vector3d& scale = sdf->getScale();

  appendRow(createStandardItemString(
      "dimensions", std::to_string(dim.x()) + " x " + std::to_string(dim.y()) + " x " + std::to_string(dim.z())));
  appendRow(createStandardItemString(
      "scale", std::to_string(scale.x()) + " " + std::to_string(scale.y()) + " " + std::to_string(scale.z())));
  appendRow(createStandardItemString("discretized", (sdf->isDiscretized()) ? "True" : "False"));
}
}  // namespace tesseract::gui
