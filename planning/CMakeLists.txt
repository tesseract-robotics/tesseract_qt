find_package(tesseract_common REQUIRED)
find_package(tesseract_task_composer REQUIRED COMPONENTS core planning)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

set(CMAKE_AUTOMOC ON)

set(PLANNING_headers_MOC include/tesseract_qt/planning/task_composer_data_storage_model.h)

add_library(
  ${PROJECT_NAME}_planning SHARED
  ${PLANNING_headers_MOC}
  src/continuous_contact_check_task_info_standard_item.cpp
  src/discrete_contact_check_task_info_standard_item.cpp
  src/fix_state_collision_task_info_standard_item.cpp
  src/motion_planner_task_info_standard_item.cpp
  src/planning_profile_remapping_standard_item.cpp
  src/register_poly_types.cpp
  src/task_composer_context_standard_item.cpp
  src/task_composer_data_storage_model.cpp
  src/task_composer_data_storage_standard_item.cpp
  src/task_composer_keys_standard_item.cpp
  src/task_composer_node_info_map_standard_item.cpp
  src/task_composer_node_info_standard_item_factory.cpp
  src/task_composer_node_info_standard_item.cpp
  src/time_optimal_parameterization_task_info_standard_item.cpp)

target_link_libraries(
  ${PROJECT_NAME}_planning
  PUBLIC tesseract::tesseract_motion_planners_core
         tesseract::tesseract_task_composer
         tesseract::tesseract_task_composer_nodes
         tesseract::tesseract_task_composer_planning_nodes
         Qt5::Core
         Qt5::Widgets
         Qt5::Gui
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_scene_graph_widgets
         ${PROJECT_NAME}_environment_widgets
         ${PROJECT_NAME}_command_language_widgets)
target_include_directories(
  ${PROJECT_NAME}_planning PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                  "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/planning>" "$<INSTALL_INTERFACE:include>")

# add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
list(APPEND PROJECT_TARGETS ${PROJECT_NAME}_planning)
set(PROJECT_TARGETS ${PROJECT_TARGETS} PARENT_SCOPE)
install_targets(TARGETS ${PROJECT_NAME}_planning)
