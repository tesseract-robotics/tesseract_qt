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

#include <string>

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

namespace
{
std::string toString(const Eigen::Vector3d& v)
{
  return std::to_string(v.x()) + " " + std::to_string(v.y()) + " " + std::to_string(v.z());
}

std::string toString(tesseract::geometry::SignedDistanceFieldInterpolation interpolation)
{
  return (interpolation == tesseract::geometry::SignedDistanceFieldInterpolation::TRICUBIC) ? "tricubic" : "trilinear";
}

/** @brief The rows describing a field's grid: its resolution, its domain and, if sparse, its band clamp. */
void appendGridRows(QStandardItem& item, const tesseract::geometry::SignedDistanceField& sdf)
{
  const Eigen::Vector3i& dim = sdf.getDimensions();
  item.appendRow(createStandardItemString(
      "dimensions", std::to_string(dim.x()) + " x " + std::to_string(dim.y()) + " x " + std::to_string(dim.z())));
  // The domain is in the field's local (unscaled) frame.
  item.appendRow(createStandardItemString("domain min", toString(sdf.getDomain().min())));
  item.appendRow(createStandardItemString("domain max", toString(sdf.getDomain().max())));
  if (sdf.isSparse() && sdf.getLevels().empty())
    item.appendRow(createStandardItemFloat("band clamp", sdf.getBandClamp()));
}
}  // namespace

void SignedDistanceFieldStandardItem::ctor()
{
  const auto& levels = sdf->getLevels();

  std::string storage = "dense";
  if (!levels.empty())
    storage = std::to_string(levels.size()) + " levels";
  else if (sdf->isSparse())
    storage = "sparse";
  appendRow(createStandardItemString("storage", storage));
  appendGridRows(*this, *sdf);
  appendRow(createStandardItemString("scale", toString(sdf->getScale())));
  // A multi-level field reads every level this way; the levels' own settings are unused.
  appendRow(createStandardItemString("interpolation", toString(sdf->getInterpolation())));
  appendRow(createStandardItemString("gradient interpolation", toString(sdf->getGradientInterpolation())));
  if (!sdf->isSparse())
    appendRow(createStandardItemString("discretized", (sdf->isDiscretized()) ? "True" : "False"));

  if (!levels.empty())
  {
    // Finest first, as the field reads them; they share the field's scale.
    auto* levels_item = new QStandardItem("levels");
    for (std::size_t i = 0; i < levels.size(); ++i)
    {
      auto* level_item = new QStandardItem(icon(), QString("[%1]").arg(static_cast<qulonglong>(i)));
      appendGridRows(*level_item, *levels[i]);
      levels_item->appendRow(level_item);
    }
    appendRow(levels_item);
  }
}
}  // namespace tesseract::gui
