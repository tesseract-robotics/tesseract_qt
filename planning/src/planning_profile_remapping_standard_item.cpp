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
#include <tesseract_qt/planning/planning_profile_remapping_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
PlannerProfileRemappingStandardItem::PlannerProfileRemappingStandardItem(const PlannerProfileRemapping& remapping)
  : QStandardItem(icons::getUnknownIcon(), "Planner Profile Remapping")
{
  ctor(remapping);
}

PlannerProfileRemappingStandardItem::PlannerProfileRemappingStandardItem(const QString& text,
                                                                         const PlannerProfileRemapping& remapping)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(remapping);
}

PlannerProfileRemappingStandardItem::PlannerProfileRemappingStandardItem(const QIcon& icon,
                                                                         const QString& text,
                                                                         const PlannerProfileRemapping& remapping)
  : QStandardItem(icon, text)
{
  ctor(remapping);
}

int PlannerProfileRemappingStandardItem::type() const
{
  return static_cast<int>(StandardItemType::MP_PLANNING_PROFILE_REMAPPING);
}

void PlannerProfileRemappingStandardItem::ctor(const PlannerProfileRemapping& remapping)
{
  for (const auto& p : remapping)
  {
    auto* planner = new QStandardItem(p.first.c_str());
    for (const auto& r : p.second)
      planner->appendRow(createStandardItemString(r.first, r.second));

    appendRow(planner);
  }
}
}  // namespace tesseract_gui
