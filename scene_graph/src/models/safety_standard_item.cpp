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
#include <tesseract_qt/scene_graph/models/safety_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

namespace tesseract_gui
{
SafetyStandardItem::SafetyStandardItem(tesseract_scene_graph::JointSafety::Ptr safety)
  : QStandardItem(icons::getSafetyIcon(), "Safety"), safety(std::move(safety))
{
  ctor();
}

SafetyStandardItem::SafetyStandardItem(const QString& text, tesseract_scene_graph::JointSafety::Ptr safety)
  : QStandardItem(icons::getSafetyIcon(), text), safety(std::move(safety))
{
  ctor();
}

SafetyStandardItem::SafetyStandardItem(const QIcon& icon,
                                       const QString& text,
                                       tesseract_scene_graph::JointSafety::Ptr safety)
  : QStandardItem(icon, text), safety(std::move(safety))
{
  ctor();
}

int SafetyStandardItem::type() const { return static_cast<int>(StandardItemType::SG_SAFETY); }

void SafetyStandardItem::ctor()
{
  appendRow(createStandardItemFloat("soft_upper_limit", safety->soft_upper_limit));
  appendRow(createStandardItemFloat("soft_lower_limit", safety->soft_lower_limit));
  appendRow(createStandardItemFloat("k_position", safety->k_position));
  appendRow(createStandardItemFloat("k_velocity", safety->k_velocity));
}
}  // namespace tesseract_gui
