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
#include <tesseract_qt/scene_graph/models/mimic_standard_item.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/standard_item_type.h>

namespace tesseract_gui
{
MimicStandardItem::MimicStandardItem(tesseract_scene_graph::JointMimic::Ptr mimic)
  : QStandardItem("Mimic"), mimic(std::move(mimic))
{
  ctor();
}

MimicStandardItem::MimicStandardItem(const QString& text, tesseract_scene_graph::JointMimic::Ptr mimic)
  : QStandardItem(text), mimic(std::move(mimic))
{
  ctor();
}

MimicStandardItem::MimicStandardItem(const QIcon& icon,
                                     const QString& text,
                                     tesseract_scene_graph::JointMimic::Ptr mimic)
  : QStandardItem(icon, text), mimic(std::move(mimic))
{
  ctor();
}

int MimicStandardItem::type() const { return static_cast<int>(StandardItemType::SG_MIMIC); }

void MimicStandardItem::ctor()
{
  appendRow(createStandardItemFloat("offset", mimic->offset));
  appendRow(createStandardItemFloat("multiplier", mimic->multiplier));
  appendRow(createStandardItemString("joint_name", mimic->joint_name));
}
}  // namespace tesseract_gui
