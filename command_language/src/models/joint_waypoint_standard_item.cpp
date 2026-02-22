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
#include <tesseract_qt/command_language/models/joint_waypoint_standard_item.h>
#include <tesseract_qt/common/models/vector_double_standard_item.h>
#include <tesseract_qt/common/models/vector_string_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/joint_waypoint.h>

namespace tesseract::gui
{
JointWaypointStandardItem::JointWaypointStandardItem(const tesseract::command_language::JointWaypointPoly& jwp)
  : QStandardItem(icons::getJointIcon(), "Joint Waypoint")
{
  ctor(jwp);
}

JointWaypointStandardItem::JointWaypointStandardItem(const QString& text,
                                                     const command_language::JointWaypointPoly& jwp)
  : QStandardItem(icons::getJointIcon(), text)
{
  ctor(jwp);
}

JointWaypointStandardItem::JointWaypointStandardItem(const QIcon& icon,
                                                     const QString& text,
                                                     const tesseract::command_language::JointWaypointPoly& jwp)
  : QStandardItem(icon, text)
{
  ctor(jwp);
}

int JointWaypointStandardItem::type() const { return static_cast<int>(StandardItemType::CL_JOINT_WAYPOINT); }

void JointWaypointStandardItem::ctor(const tesseract::command_language::JointWaypointPoly& jwp)
{
  appendRow(createStandardItemString("name", jwp.getName()));
  appendRow(new VectorStringStandardItem("joint_names", jwp.getNames()));
  appendRow(new VectorDoubleStandardItem("waypoint", jwp.getPosition()));
  appendRow(new VectorDoubleStandardItem("lower_tolerance", jwp.getLowerTolerance()));
  appendRow(new VectorDoubleStandardItem("upper_tolerance", jwp.getUpperTolerance()));
}
}  // namespace tesseract::gui
