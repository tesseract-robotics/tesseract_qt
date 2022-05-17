find_package(tesseract_common REQUIRED)
find_package(tesseract_motion_planners REQUIRED)
find_package(tesseract_process_managers REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

# qt5_wrap_cpp( COLLISION_headers_MOC include/tesseract_qt/collision/contact_results_model.h
# include/tesseract_qt/collision/contact_results_tree_view.h
# include/tesseract_qt/collision/contact_results_widget.h) qt5_wrap_ui(COLLISION_headers_UI
# include/tesseract_qt/collision/contact_results_widget.ui)

add_library(
  ${PROJECT_NAME}_planning SHARED
  # ${COLLISION_headers_MOC}
  # ${COLLISION_headers_UI}
  src/planning_profile_remapping_standard_item.cpp
  src/process_planning_request_standard_item.cpp
  src/process_planning_problem_standard_item.cpp
  src/process_planning_future_standard_item.cpp
  src/task_info_map_standard_item.cpp
  src/task_info_standard_item.cpp
  src/taskflow_interface_standard_item.cpp)

target_link_libraries(
  ${PROJECT_NAME}_planning
  PUBLIC tesseract::tesseract_motion_planners_core
         tesseract::tesseract_process_managers
         Qt5::Core
         Qt5::Widgets
         Qt5::Gui
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_scene_graph
         ${PROJECT_NAME}_environment
         ${PROJECT_NAME}_command_language)
target_include_directories(
  ${PROJECT_NAME}_planning PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                  "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/planning>" "$<INSTALL_INTERFACE:include>")

# add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_planning)
