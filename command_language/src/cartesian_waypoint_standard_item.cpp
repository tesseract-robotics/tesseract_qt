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
#include <tesseract_qt/command_language/cartesian_waypoint_standard_item.h>
#include <tesseract_qt/command_language/joint_waypoint_standard_item.h>
#include <tesseract_qt/command_language/state_waypoint_standard_item.h>
#include <tesseract_qt/command_language/null_waypoint_standard_item.h>
#include <tesseract_qt/common/transform_standard_item.h>
#include <tesseract_qt/common/vector_double_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/cartesian_waypoint.h>
#include <tesseract_command_language/joint_waypoint.h>
#include <tesseract_command_language/state_waypoint.h>
#include <tesseract_command_language/null_waypoint.h>
#include <tesseract_command_language/waypoint_type.h>

namespace tesseract_gui
{
CartesianWaypointStandardItem::CartesianWaypointStandardItem(const tesseract_planning::CartesianWaypoint& cwp)
  : QStandardItem(icons::getOriginIcon(), "Cartesian Waypoint")
{
  ctor(cwp);
}

CartesianWaypointStandardItem::CartesianWaypointStandardItem(const QString& text,
                                                             const tesseract_planning::CartesianWaypoint& cwp)
  : QStandardItem(icons::getOriginIcon(), text)
{
  ctor(cwp);
}

CartesianWaypointStandardItem::CartesianWaypointStandardItem(const QIcon& icon,
                                                             const QString& text,
                                                             const tesseract_planning::CartesianWaypoint& cwp)
  : QStandardItem(icon, text)
{
  ctor(cwp);
}

int CartesianWaypointStandardItem::type() const { return static_cast<int>(StandardItemType::CL_CARTESIAN_WAYPOINT); }

void CartesianWaypointStandardItem::ctor(const tesseract_planning::CartesianWaypoint& cwp)
{
  appendRow(new TransformStandardItem("waypoint", cwp.waypoint));
  appendRow(new VectorDoubleStandardItem("lower_tolerance", cwp.lower_tolerance));
  appendRow(new VectorDoubleStandardItem("upper_tolerance", cwp.upper_tolerance));

  if (tesseract_planning::isNullWaypoint(cwp.seed))
    appendRow(new NullWaypointStandardItem("seed", cwp.seed.as<tesseract_planning::NullWaypoint>()));
  else if (tesseract_planning::isJointWaypoint(cwp.seed))
    appendRow(new JointWaypointStandardItem("seed", cwp.seed.as<tesseract_planning::JointWaypoint>()));
  else if (tesseract_planning::isStateWaypoint(cwp.seed))
    appendRow(new StateWaypointStandardItem("seed", cwp.seed.as<tesseract_planning::StateWaypoint>()));
}
}  // namespace tesseract_gui
