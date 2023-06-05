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
#include <tesseract_qt/command_language/models/cartesian_waypoint_standard_item.h>
#include <tesseract_qt/command_language/models/joint_waypoint_standard_item.h>
#include <tesseract_qt/command_language/models/state_waypoint_standard_item.h>
#include <tesseract_qt/command_language/models/null_waypoint_standard_item.h>
#include <tesseract_qt/common/models/transform_standard_item.h>
#include <tesseract_qt/common/models/vector_double_standard_item.h>
#include <tesseract_qt/common/models/joint_state_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/poly/cartesian_waypoint_poly.h>
#include <tesseract_command_language/poly/joint_waypoint_poly.h>
#include <tesseract_command_language/poly/state_waypoint_poly.h>

namespace tesseract_gui
{
CartesianWaypointStandardItem::CartesianWaypointStandardItem(const tesseract_planning::CartesianWaypointPoly& cwp)
  : QStandardItem(icons::getOriginIcon(), "Cartesian Waypoint")
{
  ctor(cwp);
}

CartesianWaypointStandardItem::CartesianWaypointStandardItem(const QString& text,
                                                             const tesseract_planning::CartesianWaypointPoly& cwp)
  : QStandardItem(icons::getOriginIcon(), text)
{
  ctor(cwp);
}

CartesianWaypointStandardItem::CartesianWaypointStandardItem(const QIcon& icon,
                                                             const QString& text,
                                                             const tesseract_planning::CartesianWaypointPoly& cwp)
  : QStandardItem(icon, text)
{
  ctor(cwp);
}

int CartesianWaypointStandardItem::type() const { return static_cast<int>(StandardItemType::CL_CARTESIAN_WAYPOINT); }

void CartesianWaypointStandardItem::ctor(const tesseract_planning::CartesianWaypointPoly& cwp)
{
  appendRow(createStandardItemString("name", cwp.getName()));
  appendRow(new TransformStandardItem("waypoint", cwp.getTransform()));
  appendRow(new VectorDoubleStandardItem("lower_tolerance", cwp.getLowerTolerance()));
  appendRow(new VectorDoubleStandardItem("upper_tolerance", cwp.getUpperTolerance()));

  if (cwp.hasSeed())
    appendRow(new JointStateStandardItem("seed", cwp.getSeed()));
}
}  // namespace tesseract_gui
