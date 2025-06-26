find_package(tesseract_common REQUIRED)
find_package(tesseract_environment REQUIRED)
find_package(tesseract_command_language REQUIRED)
find_package(tesseract_task_composer REQUIRED COMPONENTS core)
find_package(Graphviz REQUIRED)
find_package(Boost REQUIRED)
find_package(
  Qt5
  COMPONENTS Core
             Gui
             Widgets
             Svg
  REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS include/tesseract_qt/common/widgets)

set(COMMON_WIDGETS_headers_MOC
    include/tesseract_qt/common/environment_manager.h
    include/tesseract_qt/common/environment_wrapper.h
    include/tesseract_qt/common/models/json_model.h
    include/tesseract_qt/common/models/status_log_model.h
    include/tesseract_qt/common/models/yaml_model.h
    include/tesseract_qt/common/models/component_info_manager_model.h
    include/tesseract_qt/common/widgets/cartesian_editor_widget.h
    include/tesseract_qt/common/widgets/collapsible_group_box.h
    include/tesseract_qt/common/widgets/combo_box.h
    include/tesseract_qt/common/widgets/image_viewer_widget.h
    include/tesseract_qt/common/widgets/tree_view.h
    include/tesseract_qt/common/widgets/component_info_dialog.h
    include/tesseract_qt/common/widgets/component_info_manager_widget.h
    include/tesseract_qt/common/widgets/component_info_widget.h
    include/tesseract_qt/common/widgets/create_child_component_info_dialog.h
    include/tesseract_qt/common/widgets/create_component_info_dialog.h
    include/tesseract_qt/common/widgets/load_environment_widget.h
    include/tesseract_qt/common/widgets/load_environment_dialog.h
    include/tesseract_qt/common/widgets/status_log_tool_bar.h
    include/tesseract_qt/common/widgets/status_log_widget.h)

set(COMMON_WIDGETS_resources_RCC
    include/tesseract_qt/common/resources/tesseract_qt_resources.qrc
    include/tesseract_qt/common/resources/themes/dark/qdarkstyle_dark.qrc
    include/tesseract_qt/common/resources/themes/light/qdarkstyle_light.qrc)

add_library(
  ${PROJECT_NAME}_common SHARED
  ${COMMON_WIDGETS_headers_MOC}
  ${COMMON_WIDGETS_resources_RCC}
  src/events/component_events.cpp
  src/events/allowed_collision_matrix_events.cpp
  src/events/application_events.cpp
  src/events/command_language_events.cpp
  src/events/contact_results_events.cpp
  src/events/contact_results_render_manager.cpp
  src/events/environment_events.cpp
  src/events/event_type.cpp
  src/events/group_joint_states_events.cpp
  src/events/group_tcps_events.cpp
  src/events/joint_trajectory_events.cpp
  src/events/kinematic_groups_events.cpp
  src/events/manipulation_events.cpp
  src/events/render_events.cpp
  src/events/scene_graph_events.cpp
  src/events/scene_graph_render_manager.cpp
  src/events/status_log_events.cpp
  src/events/task_composer_events.cpp
  src/events/tool_path_events.cpp
  src/events/tool_path_render_manager.cpp
  src/factories/any_poly_standard_item_factory.cpp
  src/factories/instruction_poly_standard_item_factory.cpp
  src/models/any_standard_item.cpp
  src/models/collision_margin_data_standard_item.cpp
  src/models/component_info_manager_model.cpp
  src/models/component_info_standard_item.cpp
  src/models/contact_managers_plugin_info_standard_item.cpp
  src/models/kinematics_plugin_info_standard_item.cpp
  src/models/namespace_standard_item.cpp
  src/models/null_any_standard_item.cpp
  src/models/transform_standard_item.cpp
  src/models/pairs_collision_margin_data_standard_item.cpp
  src/models/plugin_info_standard_item.cpp
  src/models/plugin_info_container_standard_item.cpp
  src/models/position_standard_item.cpp
  src/models/quaternion_standard_item.cpp
  src/models/rgba_standard_item.cpp
  src/models/status_log_model.cpp
  src/models/tool_path_segment_standard_item.cpp
  src/models/tool_path_standard_item.cpp
  src/models/type_standard_item.cpp
  src/models/vector_double_standard_item.cpp
  src/models/vector_string_standard_item.cpp
  src/models/joint_state_standard_item.cpp
  src/models/joint_trajectory_standard_item.cpp
  src/models/json_model.cpp
  src/models/yaml_model.cpp
  src/models/manipulator_info_standard_item.cpp
  src/models/calibration_info_standard_item.cpp
  src/models/standard_item_utils.cpp
  src/widgets/component_info_dialog.cpp
  src/widgets/component_info_manager_widget.cpp
  src/widgets/component_info_widget.cpp
  src/widgets/create_child_component_info_dialog.cpp
  src/widgets/create_component_info_dialog.cpp
  src/widgets/load_environment_widget.cpp
  src/widgets/load_environment_dialog.cpp
  src/widgets/cartesian_editor_widget.cpp
  src/widgets/collapsible_group_box.cpp
  src/widgets/combo_box.cpp
  src/widgets/tree_view.cpp
  src/widgets/image_viewer_widget.cpp
  src/widgets/status_log_tool_bar.cpp
  src/widgets/status_log_widget.cpp
  src/component_info_manager.cpp
  src/component_info.cpp
  src/contact_result_types.cpp
  src/entity_manager.cpp
  src/entity_container.cpp
  src/environment_manager.cpp
  src/environment_wrapper.cpp
  src/joint_trajectory_set.cpp
  src/svg_util.cpp
  src/theme_utils.cpp
  src/tool_path.cpp
  src/tool_path_pose.cpp
  src/tool_path_segment.cpp
  src/icon_utils.cpp
  src/utils.cpp)
target_link_libraries(
  ${PROJECT_NAME}_common
  PUBLIC tesseract::tesseract_common
         tesseract::tesseract_environment
         tesseract::tesseract_command_language
         tesseract::tesseract_task_composer
         PkgConfig::graphviz_gvc
         Boost::boost
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         Qt5::Svg)
target_include_directories(
  ${PROJECT_NAME}_common PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/common>" "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
list(APPEND PROJECT_TARGETS ${PROJECT_NAME}_common)
set(PROJECT_TARGETS ${PROJECT_TARGETS} PARENT_SCOPE)
install_targets(TARGETS ${PROJECT_NAME}_common)
