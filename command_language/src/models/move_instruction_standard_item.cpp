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
#include <tesseract_qt/command_language/models/move_instruction_standard_item.h>
#include <tesseract_qt/command_language/models/cartesian_waypoint_standard_item.h>
#include <tesseract_qt/command_language/models/joint_waypoint_standard_item.h>
#include <tesseract_qt/command_language/models/state_waypoint_standard_item.h>
#include <tesseract_qt/command_language/models/null_waypoint_standard_item.h>
#include <tesseract_qt/common/models/manipulator_info_standard_item.h>
#include <tesseract_qt/common/models/standard_item_type.h>
#include <tesseract_qt/common/models/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/move_instruction.h>
#include <tesseract_command_language/poly/cartesian_waypoint_poly.h>
#include <tesseract_command_language/poly/joint_waypoint_poly.h>
#include <tesseract_command_language/poly/state_waypoint_poly.h>

namespace tesseract_gui
{
MoveInstructionStandardItem::MoveInstructionStandardItem(const tesseract_planning::MoveInstructionPoly& mi)
  : QStandardItem(icons::getUnknownIcon(), "Move Instruction")
{
  ctor(mi);
}

MoveInstructionStandardItem::MoveInstructionStandardItem(const QString& text,
                                                         const tesseract_planning::MoveInstructionPoly& mi)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(mi);
}

MoveInstructionStandardItem::MoveInstructionStandardItem(const QIcon& icon,
                                                         const QString& text,
                                                         const tesseract_planning::MoveInstructionPoly& mi)
  : QStandardItem(icon, text)
{
  ctor(mi);
}

int MoveInstructionStandardItem::type() const { return static_cast<int>(StandardItemType::CL_MOVE_INSTRUCTION); }

std::string toString(tesseract_planning::MoveInstructionType m_type)
{
  switch (m_type)
  {
    case tesseract_planning::MoveInstructionType::LINEAR:
      return "LINEAR";
    case tesseract_planning::MoveInstructionType::FREESPACE:
      return "FREESPACE";
    case tesseract_planning::MoveInstructionType::CIRCULAR:
      return "CIRCULAR";
    default:
      throw std::runtime_error("Unhandled tesseract_planning::MoveInstructionType");
  }
}

void MoveInstructionStandardItem::ctor(const tesseract_planning::MoveInstructionPoly& mi)
{
  appendRow(createStandardItemString("description", mi.getDescription()));
  appendRow(createStandardItemString("profile", mi.getProfile()));
  appendRow(createStandardItemString("path_profile", mi.getPathProfile()));
  appendRow(createStandardItemString("type", toString(mi.getMoveType())));
  appendRow(new ManipulatorInfoStandardItem(mi.getManipulatorInfo()));  // NOLINT

  if (mi.getWaypoint().isCartesianWaypoint())
  {
    auto* wp =
        new CartesianWaypointStandardItem("waypoint", mi.getWaypoint().as<tesseract_planning::CartesianWaypointPoly>());
    auto* desc = new QStandardItem("Cartesian Waypoint");
    appendRow({ wp, desc });
  }
  else if (mi.getWaypoint().isJointWaypoint())
  {
    auto* wp = new JointWaypointStandardItem("waypoint", mi.getWaypoint().as<tesseract_planning::JointWaypointPoly>());
    auto* desc = new QStandardItem("Joint Waypoint");
    appendRow({ wp, desc });
  }
  else if (mi.getWaypoint().isStateWaypoint())
  {
    auto* wp = new StateWaypointStandardItem("waypoint", mi.getWaypoint().as<tesseract_planning::StateWaypointPoly>());
    auto* desc = new QStandardItem("State Waypoint");
    appendRow({ wp, desc });
  }
  else if (mi.getWaypoint().isNull())
  {
    auto* wp = new NullWaypointStandardItem("waypoint");
    auto* desc = new QStandardItem("Null Waypoint");
    appendRow({ wp, desc });
  }
}
}  // namespace tesseract_gui
