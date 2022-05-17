find_package(tesseract_common REQUIRED)
find_package(tesseract_environment REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(
  ENVIRONMENT_WIDGET_headers_MOC
  include/tesseract_qt/environment/environment_widget.h
  include/tesseract_qt/environment/environment_widget_config.h
  include/tesseract_qt/environment/environment_commands_model.h)
qt5_wrap_ui(ENVIRONMENT_WIDGET_headers_UI include/tesseract_qt/environment/environment_widget.ui)

add_library(
  ${PROJECT_NAME}_environment SHARED
  ${ENVIRONMENT_WIDGET_headers_MOC}
  ${ENVIRONMENT_WIDGET_headers_UI}
  src/environment_widget.cpp
  src/environment_widget_config.cpp
  src/environment_commands_standard_item.cpp
  src/environment_commands_model.cpp
  src/commands/add_allowed_collision_command_standard_item.cpp
  src/commands/add_contact_managers_plugin_info_command_standard_item.cpp
  src/commands/add_kinematics_information_command_standard_item.cpp
  src/commands/add_link_command_standard_item.cpp
  src/commands/add_scene_graph_command_standard_item.cpp
  src/commands/change_collision_margins_command_standard_item.cpp
  src/commands/change_joint_acceleration_limits_command_standard_item.cpp
  src/commands/change_joint_origin_command_standard_item.cpp
  src/commands/change_joint_position_limits_command_standard_item.cpp
  src/commands/change_joint_velocity_limits_command_standard_item.cpp
  src/commands/change_link_collision_enabled_command_standard_item.cpp
  src/commands/change_link_origin_command_standard_item.cpp
  src/commands/change_link_visibility_command_standard_item.cpp
  src/commands/move_joint_command_standard_item.cpp
  src/commands/move_link_command_standard_item.cpp
  src/commands/remove_allowed_collision_command_standard_item.cpp
  src/commands/remove_allowed_collision_link_command_standard_item.cpp
  src/commands/remove_joint_command_standard_item.cpp
  src/commands/remove_link_command_standard_item.cpp
  src/commands/replace_joint_command_standard_item.cpp
  src/commands/set_active_continuous_contact_manager_command_standard_item.cpp
  src/commands/set_active_discrete_contact_manager_command_standard_item.cpp)
target_link_libraries(
  ${PROJECT_NAME}_environment
  PUBLIC tesseract::tesseract_environment
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_scene_graph
         ${PROJECT_NAME}_kinematic_groups
         ${PROJECT_NAME}_acm
         ${PROJECT_NAME}_collision)
target_include_directories(
  ${PROJECT_NAME}_environment
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/environment>"
         "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_environment)
