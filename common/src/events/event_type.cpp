#include <tesseract_qt/common/events/event_type.h>

#include <qcoreevent.h>

namespace tesseract_gui::events
{
// Application Events
const int EventType::APP_STATUS_MESSAGE_SHOW = QEvent::registerEventType();
const int EventType::APP_STATUS_MESSAGE_CLEAR = QEvent::registerEventType();

// Render Events
const int EventType::PRE_RENDER = QEvent::registerEventType();
const int EventType::RENDER = QEvent::registerEventType();
const int EventType::SNAP_INTERVALS = QEvent::registerEventType();
const int EventType::SPAWN_FROM_DESCRIPTION = QEvent::registerEventType();
const int EventType::SPAWN_FROM_PATH = QEvent::registerEventType();
const int EventType::HOVER_TO_SCENE = QEvent::registerEventType();
const int EventType::LEFT_CLICK_TO_SCENE = QEvent::registerEventType();
const int EventType::RIGHT_CLICK_TO_SCENE = QEvent::registerEventType();
const int EventType::DROPDOWN_MENU_ENABLED = QEvent::registerEventType();
const int EventType::KEY_RELEASE_ON_SCENE = QEvent::registerEventType();
const int EventType::KEY_PRESS_ON_SCENE = QEvent::registerEventType();
const int EventType::LEFT_CLICK_ON_SCENE = QEvent::registerEventType();
const int EventType::RIGHT_CLICK_ON_SCENE = QEvent::registerEventType();
const int EventType::BLOCK_ORBIT = QEvent::registerEventType();
const int EventType::HOVER_ON_SCENE = QEvent::registerEventType();
const int EventType::SPAWN_CLONE_FROM_NAME = QEvent::registerEventType();
const int EventType::DROP_ON_SCENE = QEvent::registerEventType();
const int EventType::SCROLL_ON_SCENE = QEvent::registerEventType();
const int EventType::DRAG_ON_SCENE = QEvent::registerEventType();
const int EventType::MOUSE_PRESS_ON_SCENE = QEvent::registerEventType();

// Tool Path
const int EventType::TOOL_PATH_ADD = QEvent::registerEventType();
const int EventType::TOOL_PATH_REMOVE = QEvent::registerEventType();
const int EventType::TOOL_PATH_REMOVE_ALL = QEvent::registerEventType();
const int EventType::TOOL_PATH_REMOVE_SELECTED = QEvent::registerEventType();
const int EventType::TOOL_PATH_HIDE = QEvent::registerEventType();
const int EventType::TOOL_PATH_HIDE_ALL = QEvent::registerEventType();
const int EventType::TOOL_PATH_SHOW = QEvent::registerEventType();
const int EventType::TOOL_PATH_SHOW_ALL = QEvent::registerEventType();
const int EventType::TOOL_PATH_OPEN = QEvent::registerEventType();
const int EventType::TOOL_PATH_SAVE = QEvent::registerEventType();

// Scene Graph
const int EventType::SCENE_GRAPH_CLEAR = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_SET = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_ADD_LINK = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_ADD_JOINT = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_MOVE_LINK = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_MOVE_JOINT = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_REMOVE_LINK = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_REMOVE_JOINT = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_REPLACE_JOINT = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_MODIFY_LINK_VISIBILITY = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_MODIFY_LINK_VISIBILITY_ALL = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_STATE_CHANGED = QEvent::registerEventType();
const int EventType::SCENE_GRAPH_PLOT = QEvent::registerEventType();

// Contact Results
const int EventType::CONTACT_RESULTS_CLEAR = QEvent::registerEventType();
const int EventType::CONTACT_RESULTS_SET = QEvent::registerEventType();
const int EventType::CONTACT_RESULTS_REMOVE = QEvent::registerEventType();
const int EventType::CONTACT_RESULTS_VISIBILITY = QEvent::registerEventType();
const int EventType::CONTACT_RESULTS_VISIBILITY_ALL = QEvent::registerEventType();
const int EventType::CONTACT_RESULTS_COMPUTE = QEvent::registerEventType();

// Contact Results
const int EventType::ACM_CLEAR = QEvent::registerEventType();
const int EventType::ACM_SET = QEvent::registerEventType();
const int EventType::ACM_ADD = QEvent::registerEventType();
const int EventType::ACM_REMOVE = QEvent::registerEventType();
const int EventType::ACM_REMOVE_LINK = QEvent::registerEventType();
const int EventType::ACM_VISIBILITY = QEvent::registerEventType();
const int EventType::ACM_GENERATE = QEvent::registerEventType();

// Command Language
const int EventType::CL_COMPOSITE_INSTRUCTION_CLEAR = QEvent::registerEventType();
const int EventType::CL_COMPOSITE_INSTRUCTION_SET = QEvent::registerEventType();
const int EventType::CL_COMPOSITE_INSTRUCTION_REMOVE = QEvent::registerEventType();

// Group Joint States
const int EventType::GROUP_JOINT_STATES_CLEAR = QEvent::registerEventType();
const int EventType::GROUP_JOINT_STATES_SET = QEvent::registerEventType();
const int EventType::GROUP_JOINT_STATES_ADD = QEvent::registerEventType();
const int EventType::GROUP_JOINT_STATES_REMOVE = QEvent::registerEventType();
const int EventType::GROUP_JOINT_STATES_REMOVE_GROUP = QEvent::registerEventType();
const int EventType::GROUP_JOINT_STATES_SHOW = QEvent::registerEventType();
const int EventType::GROUP_JOINT_STATES_HIDE = QEvent::registerEventType();

// Group TCPS
const int EventType::GROUP_TCPS_CLEAR = QEvent::registerEventType();
const int EventType::GROUP_TCPS_SET = QEvent::registerEventType();
const int EventType::GROUP_TCPS_ADD = QEvent::registerEventType();
const int EventType::GROUP_TCPS_REMOVE = QEvent::registerEventType();
const int EventType::GROUP_TCPS_REMOVE_GROUP = QEvent::registerEventType();

// Kinematic Groups
const int EventType::KINEMATIC_GROUPS_CLEAR = QEvent::registerEventType();
const int EventType::KINEMATIC_GROUPS_SET = QEvent::registerEventType();
const int EventType::KINEMATIC_GROUPS_ADD_CHAIN = QEvent::registerEventType();
const int EventType::KINEMATIC_GROUPS_ADD_JOINT = QEvent::registerEventType();
const int EventType::KINEMATIC_GROUPS_ADD_LINK = QEvent::registerEventType();
const int EventType::KINEMATIC_GROUPS_REMOVE = QEvent::registerEventType();

// Environment Commands
const int EventType::ENVIRONMENT_COMMANDS_CLEAR = QEvent::registerEventType();
const int EventType::ENVIRONMENT_COMMANDS_SET = QEvent::registerEventType();
const int EventType::ENVIRONMENT_COMMANDS_APPEND = QEvent::registerEventType();

// Environment Commands
const int EventType::ENVIRONMENT_APPLY_COMMANDS = QEvent::registerEventType();
const int EventType::ENVIRONMENT_SET_STATE = QEvent::registerEventType();

// Joint Trajectory
const int EventType::JOINT_TRAJECTORY_ADD = QEvent::registerEventType();
const int EventType::JOINT_TRAJECTORY_REMOVE = QEvent::registerEventType();
const int EventType::JOINT_TRAJECTORY_REMOVE_NAMESPACE = QEvent::registerEventType();
const int EventType::JOINT_TRAJECTORY_REMOVE_SELECTED = QEvent::registerEventType();
const int EventType::JOINT_TRAJECTORY_REMOVE_ALL = QEvent::registerEventType();
const int EventType::JOINT_TRAJECTORY_OPEN = QEvent::registerEventType();
const int EventType::JOINT_TRAJECTORY_SAVE = QEvent::registerEventType();
const int EventType::JOINT_TRAJECTORY_PLOT = QEvent::registerEventType();
const int EventType::JOINT_TRAJECTORY_TOOLBAR_STATE = QEvent::registerEventType();

// Manipulation
const int EventType::MANIPULATION_CHANGED = QEvent::registerEventType();

// Task Composer
const int EventType::TASK_COMPOSER_LOAD_CONFIG = QEvent::registerEventType();
const int EventType::TASK_COMPOSER_LOAD_LOG = QEvent::registerEventType();
const int EventType::TASK_COMPOSER_ADD_LOG = QEvent::registerEventType();
const int EventType::TASK_COMPOSER_SAVE_LOG = QEvent::registerEventType();
const int EventType::TASK_COMPOSER_PLOT_DOTGRAPH = QEvent::registerEventType();
const int EventType::TASK_COMPOSER_SET_PROFILES = QEvent::registerEventType();

// Status Log
const int EventType::STATUS_LOG_INFO = QEvent::registerEventType();
const int EventType::STATUS_LOG_WARN = QEvent::registerEventType();
const int EventType::STATUS_LOG_ERROR = QEvent::registerEventType();
const int EventType::STATUS_LOG_CLEAR = QEvent::registerEventType();
const int EventType::STATUS_LOG_INFO_TOGGLE_ON = QEvent::registerEventType();
const int EventType::STATUS_LOG_WARN_TOGGLE_ON = QEvent::registerEventType();
const int EventType::STATUS_LOG_ERROR_TOGGLE_ON = QEvent::registerEventType();
const int EventType::STATUS_LOG_INFO_TOGGLE_OFF = QEvent::registerEventType();
const int EventType::STATUS_LOG_WARN_TOGGLE_OFF = QEvent::registerEventType();
const int EventType::STATUS_LOG_ERROR_TOGGLE_OFF = QEvent::registerEventType();

}  // namespace tesseract_gui::events
