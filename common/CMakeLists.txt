find_package(tesseract_common REQUIRED)
find_package(tesseract_environment REQUIRED)
find_package(tesseract_command_language REQUIRED)
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

qt5_wrap_cpp(
  COMMON_WIDGETS_headers_MOC
  include/tesseract_qt/common/cartesian_editor_widget.h
  include/tesseract_qt/common/combo_box.h
  include/tesseract_qt/common/image_viewer_widget.h
  include/tesseract_qt/common/json_model.h
  include/tesseract_qt/common/environment_manager.h
  include/tesseract_qt/common/environment_wrapper.h
  include/tesseract_qt/common/tree_view.h
  include/tesseract_qt/common/models/component_info_manager_model.h
  include/tesseract_qt/common/widgets/component_info_dialog.h
  include/tesseract_qt/common/widgets/component_info_manager_widget.h
  include/tesseract_qt/common/widgets/component_info_widget.h
  include/tesseract_qt/common/widgets/create_child_component_info_dialog.h
  include/tesseract_qt/common/widgets/create_component_info_dialog.h
  include/tesseract_qt/common/widgets/load_environment_widget.h
  include/tesseract_qt/common/widgets/load_environment_dialog.h)

qt5_wrap_ui(
  COMMON_WIDGETS_headers_UI
  include/tesseract_qt/common/cartesian_editor_widget.ui
  include/tesseract_qt/common/image_viewer_widget.ui
  include/tesseract_qt/common/widgets/component_info_dialog.ui
  include/tesseract_qt/common/widgets/component_info_manager_widget.ui
  include/tesseract_qt/common/widgets/component_info_widget.ui
  include/tesseract_qt/common/widgets/create_child_component_info_dialog.ui
  include/tesseract_qt/common/widgets/create_component_info_dialog.ui
  include/tesseract_qt/common/widgets/load_environment_widget.ui
  include/tesseract_qt/common/widgets/load_environment_dialog.ui)

qt5_add_resources(
  COMMON_WIDGETS_resources_RCC
  include/tesseract_qt/common/resources/tesseract_qt_resources.qrc
  include/tesseract_qt/common/resources/themes/dark/qdarkstyle_dark.qrc
  include/tesseract_qt/common/resources/themes/light/qdarkstyle_light.qrc)

add_library(
  ${PROJECT_NAME}_common SHARED
  ${COMMON_WIDGETS_headers_MOC}
  ${COMMON_WIDGETS_headers_UI}
  ${COMMON_WIDGETS_resources_RCC}
  src/events/component_events.cpp
  src/events/allowed_collision_matrix_events.cpp
  src/events/application_events.cpp
  src/events/command_language_events.cpp
  src/events/contact_results_events.cpp
  src/events/contact_results_render_manager.cpp
  src/events/environment_events.cpp
  src/events/group_joint_states_events.cpp
  src/events/group_tcps_events.cpp
  src/events/joint_trajectory_events.cpp
  src/events/kinematic_groups_events.cpp
  src/events/manipulation_events.cpp
  src/events/render_events.cpp
  src/events/scene_graph_events.cpp
  src/events/scene_graph_render_manager.cpp
  src/events/tool_path_events.cpp
  src/events/tool_path_render_manager.cpp
  src/models/component_info_manager_model.cpp
  src/widgets/component_info_dialog.cpp
  src/widgets/component_info_manager_widget.cpp
  src/widgets/component_info_widget.cpp
  src/widgets/create_child_component_info_dialog.cpp
  src/widgets/create_component_info_dialog.cpp
  src/widgets/load_environment_widget.cpp
  src/widgets/load_environment_dialog.cpp
  src/calibration_info_standard_item.cpp
  src/cartesian_editor_widget.cpp
  src/collision_margin_data_standard_item.cpp
  src/combo_box.cpp
  src/contact_managers_plugin_info_standard_item.cpp
  src/kinematics_plugin_info_standard_item.cpp
  src/namespace_standard_item.cpp
  src/transform_standard_item.cpp
  src/pairs_collision_margin_data_standard_item.cpp
  src/plugin_info_standard_item.cpp
  src/position_standard_item.cpp
  src/quaternion_standard_item.cpp
  src/rgba_standard_item.cpp
  src/component_info.cpp
  src/component_info_manager.cpp
  src/component_info_standard_item.cpp
  src/standard_item_utils.cpp
  src/plugin_info_container_standard_item.cpp
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
  src/tool_path_segment_standard_item.cpp
  src/tool_path_standard_item.cpp
  src/icon_utils.cpp
  src/image_viewer_widget.cpp
  src/tree_view.cpp
  src/type_standard_item.cpp
  src/vector_double_standard_item.cpp
  src/vector_string_standard_item.cpp
  src/joint_state_standard_item.cpp
  src/joint_trajectory_standard_item.cpp
  src/json_model.cpp
  src/manipulator_info_standard_item.cpp
  src/model_manager.cpp
  src/utils.cpp)
target_link_libraries(
  ${PROJECT_NAME}_common
  PUBLIC tesseract::tesseract_common
         tesseract::tesseract_environment
         tesseract::tesseract_command_language
         PkgConfig::graphviz_gvc
         Boost::boost
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         Qt5::Svg)
target_include_directories(
  ${PROJECT_NAME}_common PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/common>" "$<INSTALL_INTERFACE:include>")

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_common)
