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
#include <tesseract_qt/command_language/state_waypoint_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/state_waypoint.h>

namespace tesseract_gui
{
StateWaypointStandardItem::StateWaypointStandardItem(const tesseract_planning::StateWaypoint& swp)
  : JointStateStandardItem(icons::getRobotArmIcon(), "State Waypoint", swp)
{
}

StateWaypointStandardItem::StateWaypointStandardItem(const QString& text, const tesseract_planning::StateWaypoint& swp)
  : JointStateStandardItem(icons::getRobotArmIcon(), text, swp)
{
}

StateWaypointStandardItem::StateWaypointStandardItem(const QIcon& icon,
                                                     const QString& text,
                                                     const tesseract_planning::StateWaypoint& swp)
  : JointStateStandardItem(icon, text, swp)
{
}

int StateWaypointStandardItem::type() const { return static_cast<int>(StandardItemType::CL_STATE_WAYPOINT); }

}  // namespace tesseract_gui
