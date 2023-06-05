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

#include <tesseract_qt/common/models/joint_state_standard_item.h>
#include <tesseract_qt/common/models/vector_double_standard_item.h>
#include <tesseract_qt/common/models/vector_string_standard_item.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_common/joint_state.h>

namespace tesseract_gui
{
JointStateStandardItem::JointStateStandardItem(const tesseract_common::JointState& state) { ctor(state); }

JointStateStandardItem::JointStateStandardItem(const QString& text, const tesseract_common::JointState& state)
  : QStandardItem(icons::getRobotArmIcon(), text)
{
  ctor(state);
}

JointStateStandardItem::JointStateStandardItem(const QIcon& icon,
                                               const QString& text,
                                               const tesseract_common::JointState& state)
  : QStandardItem(icon, text)
{
  ctor(state);
}

int JointStateStandardItem::type() const { return static_cast<int>(StandardItemType::COMMON_JOINT_STATE); }

void JointStateStandardItem::ctor(const tesseract_common::JointState& state)
{
  // Add State Joint Names
  appendRow(new VectorStringStandardItem("joint_names", state.joint_names));
  appendRow(new VectorDoubleStandardItem("position", state.position));
  appendRow(new VectorDoubleStandardItem("velocity", state.velocity));
  appendRow(new VectorDoubleStandardItem("acceleration", state.acceleration));
  appendRow(createStandardItemFloat("time", state.time));
}
}  // namespace tesseract_gui
