find_package(tesseract_common REQUIRED)
find_package(tesseract_environment REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS include/tesseract_qt/environment/widgets)

set(ENVIRONMENT_MODEL_headers_MOC include/tesseract_qt/environment/models/environment_commands_model.h)

add_library(
  ${PROJECT_NAME}_environment_models SHARED
  ${ENVIRONMENT_MODEL_headers_MOC}
  src/models/environment_commands_standard_item.cpp
  src/models/environment_commands_model.cpp
  src/models/commands/add_contact_managers_plugin_info_command_standard_item.cpp
  src/models/commands/add_kinematics_information_command_standard_item.cpp
  src/models/commands/add_link_command_standard_item.cpp
  src/models/commands/add_scene_graph_command_standard_item.cpp
  src/models/commands/add_trajectory_link_command_standard_item.cpp
  src/models/commands/change_collision_margins_command_standard_item.cpp
  src/models/commands/change_joint_acceleration_limits_command_standard_item.cpp
  src/models/commands/change_joint_origin_command_standard_item.cpp
  src/models/commands/change_joint_position_limits_command_standard_item.cpp
  src/models/commands/change_joint_velocity_limits_command_standard_item.cpp
  src/models/commands/change_link_collision_enabled_command_standard_item.cpp
  src/models/commands/change_link_origin_command_standard_item.cpp
  src/models/commands/change_link_visibility_command_standard_item.cpp
  src/models/commands/modify_allowed_collisions_command_standard_item.cpp
  src/models/commands/move_joint_command_standard_item.cpp
  src/models/commands/move_link_command_standard_item.cpp
  src/models/commands/remove_allowed_collision_link_command_standard_item.cpp
  src/models/commands/remove_joint_command_standard_item.cpp
  src/models/commands/remove_link_command_standard_item.cpp
  src/models/commands/replace_joint_command_standard_item.cpp
  src/models/commands/set_active_continuous_contact_manager_command_standard_item.cpp
  src/models/commands/set_active_discrete_contact_manager_command_standard_item.cpp)
target_link_libraries(
  ${PROJECT_NAME}_environment_models
  PUBLIC tesseract::tesseract_environment
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_scene_graph_models
         ${PROJECT_NAME}_kinematic_groups_models
         ${PROJECT_NAME}_acm_models
         ${PROJECT_NAME}_collision_models)
target_include_directories(
  ${PROJECT_NAME}_environment_models
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/environment>"
         "$<INSTALL_INTERFACE:include>")

set(ENVIRONMENT_WIDGET_headers_MOC include/tesseract_qt/environment/widgets/environment_commands_widget.h
                                   include/tesseract_qt/environment/widgets/environment_widget.h)

add_library(
  ${PROJECT_NAME}_environment_widgets SHARED ${ENVIRONMENT_WIDGET_headers_MOC} src/widgets/environment_widget.cpp
                                             src/widgets/environment_commands_widget.cpp)
target_link_libraries(
  ${PROJECT_NAME}_environment_widgets
  PUBLIC tesseract::tesseract_environment
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${PROJECT_NAME}_environment_models
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_scene_graph_widgets
         ${PROJECT_NAME}_kinematic_groups_widgets
         ${PROJECT_NAME}_acm_widgets
         ${PROJECT_NAME}_collision_widgets)
target_include_directories(
  ${PROJECT_NAME}_environment_widgets
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/environment>"
         "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
list(
  APPEND
  PROJECT_TARGETS
  ${PROJECT_NAME}_environment_models
  ${PROJECT_NAME}_environment_widgets)
set(PROJECT_TARGETS ${PROJECT_TARGETS} PARENT_SCOPE)
install_targets(TARGETS ${PROJECT_NAME}_environment_models ${PROJECT_NAME}_environment_widgets)
