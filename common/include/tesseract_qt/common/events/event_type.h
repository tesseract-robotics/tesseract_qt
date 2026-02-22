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
#ifndef TESSERACT_GUI_COMMON_EVENT_TYPE_H
#define TESSERACT_GUI_COMMON_EVENT_TYPE_H

#include <QEvent>
namespace tesseract::gui::events
{
struct EventType
{
  // Application Events
  static const int APP_STATUS_MESSAGE_SHOW;
  static const int APP_STATUS_MESSAGE_CLEAR;

  // Render Events
  static const int PRE_RENDER;
  static const int RENDER;
  static const int SNAP_INTERVALS;
  static const int SPAWN_FROM_DESCRIPTION;
  static const int SPAWN_FROM_PATH;
  static const int HOVER_TO_SCENE;
  static const int LEFT_CLICK_TO_SCENE;
  static const int RIGHT_CLICK_TO_SCENE;
  static const int DROPDOWN_MENU_ENABLED;
  static const int KEY_RELEASE_ON_SCENE;
  static const int KEY_PRESS_ON_SCENE;
  static const int LEFT_CLICK_ON_SCENE;
  static const int RIGHT_CLICK_ON_SCENE;
  static const int BLOCK_ORBIT;
  static const int HOVER_ON_SCENE;
  static const int SPAWN_CLONE_FROM_NAME;
  static const int DROP_ON_SCENE;
  static const int SCROLL_ON_SCENE;
  static const int DRAG_ON_SCENE;
  static const int MOUSE_PRESS_ON_SCENE;

  // Tool Path
  static const int TOOL_PATH_ADD;
  static const int TOOL_PATH_REMOVE;
  static const int TOOL_PATH_REMOVE_ALL;
  static const int TOOL_PATH_REMOVE_SELECTED;
  static const int TOOL_PATH_HIDE;
  static const int TOOL_PATH_HIDE_ALL;
  static const int TOOL_PATH_SHOW;
  static const int TOOL_PATH_SHOW_ALL;
  static const int TOOL_PATH_OPEN;
  static const int TOOL_PATH_SAVE;

  // Scene Graph
  static const int SCENE_GRAPH_CLEAR;
  static const int SCENE_GRAPH_SET;
  static const int SCENE_GRAPH_ADD_LINK;
  static const int SCENE_GRAPH_ADD_JOINT;
  static const int SCENE_GRAPH_MOVE_LINK;
  static const int SCENE_GRAPH_MOVE_JOINT;
  static const int SCENE_GRAPH_REMOVE_LINK;
  static const int SCENE_GRAPH_REMOVE_JOINT;
  static const int SCENE_GRAPH_REPLACE_JOINT;
  static const int SCENE_GRAPH_MODIFY_LINK_VISIBILITY;
  static const int SCENE_GRAPH_MODIFY_LINK_VISIBILITY_ALL;
  static const int SCENE_GRAPH_STATE_CHANGED;
  static const int SCENE_GRAPH_PLOT;

  // Contact Results
  static const int CONTACT_RESULTS_CLEAR;
  static const int CONTACT_RESULTS_SET;
  static const int CONTACT_RESULTS_REMOVE;
  static const int CONTACT_RESULTS_VISIBILITY;
  static const int CONTACT_RESULTS_VISIBILITY_ALL;
  static const int CONTACT_RESULTS_COMPUTE;

  // Contact Results
  static const int ACM_CLEAR;
  static const int ACM_SET;
  static const int ACM_ADD;
  static const int ACM_REMOVE;
  static const int ACM_REMOVE_LINK;
  static const int ACM_VISIBILITY;
  static const int ACM_GENERATE;

  // Command Language
  static const int CL_COMPOSITE_INSTRUCTION_CLEAR;
  static const int CL_COMPOSITE_INSTRUCTION_SET;
  static const int CL_COMPOSITE_INSTRUCTION_REMOVE;

  // Group Joint States
  static const int GROUP_JOINT_STATES_CLEAR;
  static const int GROUP_JOINT_STATES_SET;
  static const int GROUP_JOINT_STATES_ADD;
  static const int GROUP_JOINT_STATES_REMOVE;
  static const int GROUP_JOINT_STATES_REMOVE_GROUP;
  static const int GROUP_JOINT_STATES_SHOW;
  static const int GROUP_JOINT_STATES_HIDE;

  // Group TCPS
  static const int GROUP_TCPS_CLEAR;
  static const int GROUP_TCPS_SET;
  static const int GROUP_TCPS_ADD;
  static const int GROUP_TCPS_REMOVE;
  static const int GROUP_TCPS_REMOVE_GROUP;

  // Kinematic Groups
  static const int KINEMATIC_GROUPS_CLEAR;
  static const int KINEMATIC_GROUPS_SET;
  static const int KINEMATIC_GROUPS_ADD_CHAIN;
  static const int KINEMATIC_GROUPS_ADD_JOINT;
  static const int KINEMATIC_GROUPS_ADD_LINK;
  static const int KINEMATIC_GROUPS_REMOVE;

  // Environment Commands
  static const int ENVIRONMENT_COMMANDS_CLEAR;
  static const int ENVIRONMENT_COMMANDS_SET;
  static const int ENVIRONMENT_COMMANDS_APPEND;

  // Environment Commands
  static const int ENVIRONMENT_APPLY_COMMANDS;
  static const int ENVIRONMENT_SET_STATE;

  // Joint Trajectory
  static const int JOINT_TRAJECTORY_ADD;
  static const int JOINT_TRAJECTORY_REMOVE;
  static const int JOINT_TRAJECTORY_REMOVE_NAMESPACE;
  static const int JOINT_TRAJECTORY_REMOVE_SELECTED;
  static const int JOINT_TRAJECTORY_REMOVE_ALL;
  static const int JOINT_TRAJECTORY_OPEN;
  static const int JOINT_TRAJECTORY_SAVE;
  static const int JOINT_TRAJECTORY_PLOT;
  static const int JOINT_TRAJECTORY_TOOLBAR_STATE;

  // Manipulation
  static const int MANIPULATION_CHANGED;

  // Task Composer
  static const int TASK_COMPOSER_LOAD_CONFIG;
  static const int TASK_COMPOSER_LOAD_LOG;
  static const int TASK_COMPOSER_ADD_LOG;
  static const int TASK_COMPOSER_SAVE_LOG;
  static const int TASK_COMPOSER_PLOT_DOTGRAPH;
  static const int TASK_COMPOSER_SET_PROFILES;

  // Status Log
  static const int STATUS_LOG_INFO;
  static const int STATUS_LOG_WARN;
  static const int STATUS_LOG_ERROR;
  static const int STATUS_LOG_CLEAR;
  static const int STATUS_LOG_INFO_TOGGLE_ON;
  static const int STATUS_LOG_WARN_TOGGLE_ON;
  static const int STATUS_LOG_ERROR_TOGGLE_ON;
  static const int STATUS_LOG_INFO_TOGGLE_OFF;
  static const int STATUS_LOG_WARN_TOGGLE_OFF;
  static const int STATUS_LOG_ERROR_TOGGLE_OFF;
};
}  // namespace tesseract::gui::events

#endif  // TESSERACT_GUI_COMMON_EVENT_TYPE_H
