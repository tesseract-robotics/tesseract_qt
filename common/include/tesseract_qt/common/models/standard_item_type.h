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
#ifndef TESSERACT_GUI_COMMON_STANDARD_ITEMS_TYPES_H
#define TESSERACT_GUI_COMMON_STANDARD_ITEMS_TYPES_H

#include <QStandardItem>

namespace tesseract_gui
{
// clang-format off
enum class StandardItemType : int
{
  // Common Types
  COMMON_PLUGIN_INFO                  = QStandardItem::UserType + 1,
  COMMON_PLUGIN_INFO_CONTAINER        = QStandardItem::UserType + 2,
  COMMON_KINEMATICS_PLUGIN_INFO       = QStandardItem::UserType + 3,
  COMMON_CONTACT_MANAGERS_PLUGIN_INFO = QStandardItem::UserType + 4,
  COMMON_CALIBRATION_INFO             = QStandardItem::UserType + 5,
  COMMON_COLLISION_MARGIN_DATA        = QStandardItem::UserType + 6,
  COMMON_PAIRS_COLLISION_MARGIN_DATA  = QStandardItem::UserType + 7,
  COMMON_TRANSFORM                    = QStandardItem::UserType + 8,
  COMMON_TRANSFORM_POSITION           = QStandardItem::UserType + 9,
  COMMON_TRANSFORM_QUATERNION         = QStandardItem::UserType + 10,
  COMMON_NAMESPACE                    = QStandardItem::UserType + 11,
  COMMON_VECTOR_DOUBLE                = QStandardItem::UserType + 12,
  COMMON_VECTOR_STRING                = QStandardItem::UserType + 13,
  COMMON_JOINT_STATE                  = QStandardItem::UserType + 14,
  COMMON_JOINT_TRAJECTORY             = QStandardItem::UserType + 15,
  COMMON_MANIPULATOR_INFO             = QStandardItem::UserType + 16,
  COMMON_COMPONENT_INFO               = QStandardItem::UserType + 17,
  COMMON_TYPES_END                    = QStandardItem::UserType + 18,

  // Tool Path Types
  COMMON_TOOL_PATH         = COMMON_TYPES_END + 1,
  COMMON_TOOL_PATH_SEGMENT = COMMON_TYPES_END + 2,
  COMMON_TOOL_PATH_END     = COMMON_TYPES_END + 3,

  // Joint Trajectory Set Types
  JOINT_TRAJECTORY_SET            = COMMON_TOOL_PATH_END + 1,
  JOINT_TRAJECTORY_SET_TRAJECTORY = COMMON_TOOL_PATH_END + 2,
  JOINT_TRAJECTORY_SET_STATE      = COMMON_TOOL_PATH_END + 3,
  JOINT_TRAJECTORY_SET_END        = COMMON_TOOL_PATH_END + 4,

  // Scene Graph Types
  SG_MATERIAL     = JOINT_TRAJECTORY_SET_END + 1,
  SG_INERTIAL     = JOINT_TRAJECTORY_SET_END + 2,
  SG_ORIGIN       = JOINT_TRAJECTORY_SET_END + 3,
  SG_GEOMETRY     = JOINT_TRAJECTORY_SET_END + 4,
  SG_VISUAL       = JOINT_TRAJECTORY_SET_END + 5,
  SG_COLLISION    = JOINT_TRAJECTORY_SET_END + 6,
  SG_LINK         = JOINT_TRAJECTORY_SET_END + 7,
  SG_DYNAMICS     = JOINT_TRAJECTORY_SET_END + 8,
  SG_LIMITS       = JOINT_TRAJECTORY_SET_END + 9,
  SG_SAFETY       = JOINT_TRAJECTORY_SET_END + 10,
  SG_CALIBRATION  = JOINT_TRAJECTORY_SET_END + 11,
  SG_MIMIC        = JOINT_TRAJECTORY_SET_END + 12,
  SG_JOINT        = JOINT_TRAJECTORY_SET_END + 13,
  SG_SCENE_GRAPH  = JOINT_TRAJECTORY_SET_END + 15,
  SG_BOX          = JOINT_TRAJECTORY_SET_END + 16,
  SG_CAPSULE      = JOINT_TRAJECTORY_SET_END + 17,
  SG_CONE         = JOINT_TRAJECTORY_SET_END + 18,
  SG_CYLINDER     = JOINT_TRAJECTORY_SET_END + 19,
  SG_PLANE        = JOINT_TRAJECTORY_SET_END + 20,
  SG_SPHERE       = JOINT_TRAJECTORY_SET_END + 21,
  SG_POLYGON_MESH = JOINT_TRAJECTORY_SET_END + 22,
  SG_OCTREE       = JOINT_TRAJECTORY_SET_END + 23,
  SG_VISUALS      = JOINT_TRAJECTORY_SET_END + 24,
  SG_COLLISIONS   = JOINT_TRAJECTORY_SET_END + 25,
  SG_SCENE_STATE  = JOINT_TRAJECTORY_SET_END + 26,
  SG_TYPES_END    = JOINT_TRAJECTORY_SET_END + 27,

  // SRDF Types
  SRDF_OPW_PARAMS         = SG_TYPES_END + 1,
  SRDF_GROUP_TCPS         = SG_TYPES_END + 2,
  SRDF_TCP                = SG_TYPES_END + 3,
  SRDF_CHAIN_GROUP        = SG_TYPES_END + 4,
  SRDF_JOINT_GROUP        = SG_TYPES_END + 5,
  SRDF_LINK_GROUP         = SG_TYPES_END + 6,
  SRDF_GROUP_JOINT_STATES = SG_TYPES_END + 7,
  SRDF_GROUP_JOINT_STATE  = SG_TYPES_END + 8,
  SRDF_KINEMATICS_INFO    = SG_TYPES_END + 9,
  SRDF_TYPES_END          = SG_TYPES_END + 10,

  // Environment Command Types
  ENV_COMMAND_UNINITIALIZED                         = SG_TYPES_END + 1,
  ENV_COMMAND_ADD_LINK                              = SG_TYPES_END + 2,
  ENV_COMMAND_MOVE_LINK                             = SG_TYPES_END + 3,
  ENV_COMMAND_MOVE_JOINT                            = SG_TYPES_END + 4,
  ENV_COMMAND_REMOVE_LINK                           = SG_TYPES_END + 5,
  ENV_COMMAND_REMOVE_JOINT                          = SG_TYPES_END + 6,
  ENV_COMMAND_CHANGE_LINK_ORIGIN                    = SG_TYPES_END + 7,
  ENV_COMMAND_CHANGE_JOINT_ORIGIN                   = SG_TYPES_END + 8,
  ENV_COMMAND_CHANGE_LINK_COLLISION_ENABLED         = SG_TYPES_END + 9,
  ENV_COMMAND_CHANGE_LINK_VISIBILITY                = SG_TYPES_END + 10,
  ENV_COMMAND_MODIFY_ALLOWED_COLLISIONS             = SG_TYPES_END + 11,
  ENV_COMMAND_REMOVE_ALLOWED_COLLISION_LINK         = SG_TYPES_END + 12,
  ENV_COMMAND_ADD_SCENE_GRAPH                       = SG_TYPES_END + 13,
  ENV_COMMAND_CHANGE_JOINT_POSITION_LIMITS          = SG_TYPES_END + 14,
  ENV_COMMAND_CHANGE_JOINT_VELOCITY_LIMITS          = SG_TYPES_END + 15,
  ENV_COMMAND_CHANGE_JOINT_ACCELERATION_LIMITS      = SG_TYPES_END + 16,
  ENV_COMMAND_ADD_KINEMATICS_INFORMATION            = SG_TYPES_END + 17,
  ENV_COMMAND_REPLACE_JOINT                         = SG_TYPES_END + 18,
  ENV_COMMAND_CHANGE_COLLISION_MARGINS              = SG_TYPES_END + 19,
  ENV_COMMAND_ADD_CONTACT_MANAGERS_PLUGIN_INFO      = SG_TYPES_END + 20,
  ENV_COMMAND_SET_ACTIVE_DISCRETE_CONTACT_MANAGER   = SG_TYPES_END + 21,
  ENV_COMMAND_SET_ACTIVE_CONTINUOUS_CONTACT_MANAGER = SG_TYPES_END + 22,
  ENV_COMMAND_ADD_TRAJECTORY_LINK                   = SG_TYPES_END + 23,
  ENV_COMMAND_COMMANDS                              = SG_TYPES_END + 24,
  ENV_COMMAND_TYPES_END                             = SG_TYPES_END + 25,

  // Collision Types
  COLLISION_CONTACT_RESULT        = ENV_COMMAND_TYPES_END + 1,
  COLLISION_CONTACT_RESULT_VECTOR = ENV_COMMAND_TYPES_END + 2,
  COLLISION_CONTACT_REQUEST       = ENV_COMMAND_TYPES_END + 3,
  COLLISION_CONTACT_DETAILS       = ENV_COMMAND_TYPES_END + 4,
  COLLISION_COLLISION_MARGIN_DATA = ENV_COMMAND_TYPES_END + 5,
  COLLISION_TYPES_END             = ENV_COMMAND_TYPES_END + 6,

  // COMMAND_LANGUAGE Types
  CL_CARTESIAN_WAYPOINT     = COLLISION_TYPES_END + 1,
  CL_JOINT_WAYPOINT         = COLLISION_TYPES_END + 2,
  CL_STATE_WAYPOINT         = COLLISION_TYPES_END + 3,
  CL_NULL_WAYPOINT          = COLLISION_TYPES_END + 4,
  CL_MOVE_INSTRUCTION       = COLLISION_TYPES_END + 5,
  CL_SET_ANALOG_INSTRUCTION = COLLISION_TYPES_END + 6,
  CL_SET_TOOL_INSTRUCTION   = COLLISION_TYPES_END + 7,
  CL_TIMER_INSTRUCTION      = COLLISION_TYPES_END + 8,
  CL_WAIT_INSTRUCTION       = COLLISION_TYPES_END + 9,
  CL_COMPOSITE_INSTRUCTION  = COLLISION_TYPES_END + 10,
  CL_VECTOR_INSTRUCTION     = COLLISION_TYPES_END + 11,
  CL_NULL_INSTRUCTION       = COLLISION_TYPES_END + 12,
  CL_INSTRUCTION            = COLLISION_TYPES_END + 13,
  CL_PLAN_INSTRUCTION       = COLLISION_TYPES_END + 14,
  CL_TYPES_END              = COLLISION_TYPES_END + 15,

  // Motion planning types
  MP_PLANNING_PROFILE_REMAPPING  = CL_TYPES_END + 1,
  MP_PROCESS_PLANNING_REQUEST    = CL_TYPES_END + 2,
  MP_TASK_COMPOSER_PROBLEM       = CL_TYPES_END + 3,
  MP_TASK_COMPOSER_INPUT         = CL_TYPES_END + 4,
  MP_TASK_COMPOSER_NODE_INFO     = CL_TYPES_END + 5,
  MP_TASK_COMPOSER_NODE_INFO_MAP = CL_TYPES_END + 6,
  MP_TASK_COMPOSER_DATA_STORAGE  = CL_TYPES_END + 7,
  MP_TYPES_END                   = CL_TYPES_END + 8,

};
// clang-format on
}  // namespace tesseract_gui

#endif  // TESSERACT_GUI_COMMON_STANDARD_ITEMS_TYPES_H
