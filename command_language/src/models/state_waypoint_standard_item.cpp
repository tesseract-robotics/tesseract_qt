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
#include <tesseract_qt/command_language/models/state_waypoint_standard_item.h>
#include <tesseract_qt/common/models/vector_double_standard_item.h>
#include <tesseract_qt/common/models/vector_string_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/poly/state_waypoint_poly.h>

namespace tesseract::gui
{
StateWaypointStandardItem::StateWaypointStandardItem(const tesseract::command_language::StateWaypointPoly& swp)
  : QStandardItem(icons::getRobotArmIcon(), "State Waypoint")
{
  ctor(swp);
}

StateWaypointStandardItem::StateWaypointStandardItem(const QString& text,
                                                     const tesseract::command_language::StateWaypointPoly& swp)
  : QStandardItem(icons::getRobotArmIcon(), text)
{
  ctor(swp);
}

StateWaypointStandardItem::StateWaypointStandardItem(const QIcon& icon,
                                                     const QString& text,
                                                     const tesseract::command_language::StateWaypointPoly& swp)
  : QStandardItem(icon, text)
{
  ctor(swp);
}

int StateWaypointStandardItem::type() const { return static_cast<int>(StandardItemType::CL_STATE_WAYPOINT); }

void StateWaypointStandardItem::ctor(const tesseract::command_language::StateWaypointPoly& swp)
{
  // Add State Joint Names
  appendRow(createStandardItemString("name", swp.getName()));
  appendRow(new VectorStringStandardItem("joint_names", swp.getNames()));
  appendRow(new VectorDoubleStandardItem("position", swp.getPosition()));
  appendRow(new VectorDoubleStandardItem("velocity", swp.getVelocity()));
  appendRow(new VectorDoubleStandardItem("acceleration", swp.getAcceleration()));
  appendRow(createStandardItemFloat("time", swp.getTime()));
}
}  // namespace tesseract::gui
