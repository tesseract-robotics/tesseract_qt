find_package(tesseract_common REQUIRED)
find_package(tesseract_kinematics REQUIRED)
find_package(tesseract_srdf REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(
  KINEMATIC_GROUP_headers_MOC
  include/tesseract_qt/kinematic_groups/kinematic_groups_model.h
  include/tesseract_qt/kinematic_groups/group_joint_states_model.h
  include/tesseract_qt/kinematic_groups/group_tcps_model.h)

add_library(
  ${PROJECT_NAME}_kinematic_groups SHARED
  ${KINEMATIC_GROUP_headers_MOC}
  src/chain_group_standard_item.cpp
  src/link_group_standard_item.cpp
  src/opw_params_standard_item.cpp
  src/joint_group_standard_item.cpp
  src/kinematic_groups_model.cpp
  src/kinematic_groups_events.cpp
  src/kinematics_info_standard_item.cpp
  src/group_joint_state_standard_item.cpp
  src/group_joint_states_standard_item.cpp
  src/group_joint_states_events.cpp
  src/group_joint_states_model.cpp
  src/group_tcps_standard_item.cpp
  src/group_tcps_model.cpp
  src/group_tcps_events.cpp)
target_link_libraries(
  ${PROJECT_NAME}_kinematic_groups
  PUBLIC tesseract::tesseract_common
         tesseract::tesseract_kinematics_opw
         tesseract::tesseract_srdf
         Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_kinematic_groups PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                          "$<INSTALL_INTERFACE:include>")

qt5_wrap_cpp(KINEMATIC_GROUP_EDITOR_headers_MOC include/tesseract_qt/kinematic_groups/kinematic_groups_editor_widget.h)
qt5_wrap_ui(KINEMATIC_GROUP_EDITOR_headers_UI include/tesseract_qt/kinematic_groups/kinematic_groups_editor_widget.ui)

add_library(
  ${PROJECT_NAME}_kinematic_groups_editor_widget SHARED
  ${KINEMATIC_GROUP_EDITOR_headers_MOC} ${KINEMATIC_GROUP_EDITOR_headers_UI} src/kinematic_groups_editor_widget.cpp)
target_link_libraries(
  ${PROJECT_NAME}_kinematic_groups_editor_widget
  PUBLIC Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_kinematic_groups)
target_include_directories(
  ${PROJECT_NAME}_kinematic_groups_editor_widget
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/kinematic_groups>" "$<INSTALL_INTERFACE:include>")

qt5_wrap_cpp(GROUPS_JOINT_STATES_EDITOR_headers_MOC
             include/tesseract_qt/kinematic_groups/groups_joint_states_editor_widget.h)
qt5_wrap_ui(GROUPS_JOINT_STATES_EDITOR_headers_UI
            include/tesseract_qt/kinematic_groups/groups_joint_states_editor_widget.ui)

add_library(
  ${PROJECT_NAME}_groups_joint_states_editor_widget SHARED
  ${GROUPS_JOINT_STATES_EDITOR_headers_MOC} ${GROUPS_JOINT_STATES_EDITOR_headers_UI}
  src/groups_joint_states_editor_widget.cpp)
target_link_libraries(
  ${PROJECT_NAME}_groups_joint_states_editor_widget
  PUBLIC Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_kinematic_groups
         ${PROJECT_NAME}_joint_state_slider)
target_include_directories(
  ${PROJECT_NAME}_groups_joint_states_editor_widget
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/kinematic_groups>" "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(
  TARGETS
  ${PROJECT_NAME}_kinematic_groups
  ${PROJECT_NAME}_kinematic_groups_editor_widget
  ${PROJECT_NAME}_groups_joint_states_editor_widget)
