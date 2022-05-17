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
#include <tesseract_qt/command_language/plan_instruction_standard_item.h>
#include <tesseract_qt/command_language/cartesian_waypoint_standard_item.h>
#include <tesseract_qt/command_language/joint_waypoint_standard_item.h>
#include <tesseract_qt/command_language/state_waypoint_standard_item.h>
#include <tesseract_qt/command_language/null_waypoint_standard_item.h>
#include <tesseract_qt/common/manipulator_info_standard_item.h>
#include <tesseract_qt/common/standard_item_type.h>
#include <tesseract_qt/common/standard_item_utils.h>
#include <tesseract_qt/common/icon_utils.h>

#include <tesseract_command_language/waypoint_type.h>
#include <tesseract_command_language/plan_instruction.h>
#include <tesseract_command_language/cartesian_waypoint.h>
#include <tesseract_command_language/state_waypoint.h>
#include <tesseract_command_language/joint_waypoint.h>
#include <tesseract_command_language/null_waypoint.h>

namespace tesseract_gui
{
PlanInstructionStandardItem::PlanInstructionStandardItem(const tesseract_planning::PlanInstruction& pi)
  : QStandardItem(icons::getUnknownIcon(), "Plan Instruction")
{
  ctor(pi);
}

PlanInstructionStandardItem::PlanInstructionStandardItem(const QString& text,
                                                         const tesseract_planning::PlanInstruction& pi)
  : QStandardItem(icons::getUnknownIcon(), text)
{
  ctor(pi);
}

PlanInstructionStandardItem::PlanInstructionStandardItem(const QIcon& icon,
                                                         const QString& text,
                                                         const tesseract_planning::PlanInstruction& pi)
  : QStandardItem(icon, text)
{
  ctor(pi);
}

int PlanInstructionStandardItem::type() const { return static_cast<int>(StandardItemType::CL_PLAN_INSTRUCTION); }

std::string toString(tesseract_planning::PlanInstructionType p_type)
{
  switch (p_type)
  {
    case tesseract_planning::PlanInstructionType::LINEAR:
      return "LINEAR";
    case tesseract_planning::PlanInstructionType::FREESPACE:
      return "FREESPACE";
    case tesseract_planning::PlanInstructionType::CIRCULAR:
      return "CIRCULAR";
    case tesseract_planning::PlanInstructionType::START:
      return "START";
    default:
      throw std::runtime_error("Unhandled tesseract_planning::PlanInstructionType");
  }
}

void PlanInstructionStandardItem::ctor(const tesseract_planning::PlanInstruction& pi)
{
  appendRow(createStandardItemString("description", pi.getDescription()));
  appendRow(createStandardItemString("profile", pi.getProfile()));
  appendRow(createStandardItemString("path_profile", pi.getPathProfile()));
  appendRow(createStandardItemString("type", toString(pi.getPlanType())));
  appendRow(new ManipulatorInfoStandardItem(pi.getManipulatorInfo()));  // NOLINT

  if (tesseract_planning::isCartesianWaypoint(pi.getWaypoint()))
  {
    auto* wp =
        new CartesianWaypointStandardItem("waypoint", pi.getWaypoint().as<tesseract_planning::CartesianWaypoint>());
    auto* desc = new QStandardItem("Cartesian Waypoint");
    appendRow({ wp, desc });
  }
  else if (tesseract_planning::isJointWaypoint(pi.getWaypoint()))
  {
    auto* wp = new JointWaypointStandardItem("waypoint", pi.getWaypoint().as<tesseract_planning::JointWaypoint>());
    auto* desc = new QStandardItem("Joint Waypoint");
    appendRow({ wp, desc });
  }
  else if (tesseract_planning::isStateWaypoint(pi.getWaypoint()))
  {
    auto* wp = new StateWaypointStandardItem("waypoint", pi.getWaypoint().as<tesseract_planning::StateWaypoint>());
    auto* desc = new QStandardItem("State Waypoint");
    appendRow({ wp, desc });
  }
  else if (tesseract_planning::isNullWaypoint(pi.getWaypoint()))
  {
    auto* wp = new NullWaypointStandardItem("waypoint", pi.getWaypoint().as<tesseract_planning::NullWaypoint>());
    auto* desc = new QStandardItem("Null Waypoint");
    appendRow({ wp, desc });
  }
}
}  // namespace tesseract_gui
