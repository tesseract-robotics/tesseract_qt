find_package(tesseract_common REQUIRED)
find_package(tesseract_kinematics REQUIRED)
find_package(tesseract_srdf REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(
  KINEMATIC_MODEL_GROUP_headers_MOC
  include/tesseract_qt/kinematic_groups/models/kinematic_groups_model.h
  include/tesseract_qt/kinematic_groups/models/group_joint_states_model.h
  include/tesseract_qt/kinematic_groups/models/group_tcps_model.h)

add_library(
  ${PROJECT_NAME}_kinematic_groups_models SHARED
  ${KINEMATIC_MODEL_GROUP_headers_MOC}
  src/models/chain_group_standard_item.cpp
  src/models/link_group_standard_item.cpp
  src/models/opw_params_standard_item.cpp
  src/models/joint_group_standard_item.cpp
  src/models/kinematic_groups_model.cpp
  src/models/kinematics_info_standard_item.cpp
  src/models/group_joint_state_standard_item.cpp
  src/models/group_joint_states_standard_item.cpp
  src/models/group_joint_states_model.cpp
  src/models/group_tcps_standard_item.cpp
  src/models/group_tcps_model.cpp)
target_link_libraries(
  ${PROJECT_NAME}_kinematic_groups_models
  PUBLIC tesseract::tesseract_common
         tesseract::tesseract_kinematics_opw
         tesseract::tesseract_srdf
         Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_kinematic_groups_models PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                                 "$<INSTALL_INTERFACE:include>")

qt5_wrap_cpp(
  KINEMATIC_GROUP_WIDGET_headers_MOC
  include/tesseract_qt/kinematic_groups/widgets/group_joint_states_widget.h
  include/tesseract_qt/kinematic_groups/widgets/group_joint_states_editor_widget.h
  include/tesseract_qt/kinematic_groups/widgets/group_tcps_widget.h
  include/tesseract_qt/kinematic_groups/widgets/kinematic_groups_widget.h
  include/tesseract_qt/kinematic_groups/widgets/kinematic_groups_editor_widget.h)
qt5_wrap_ui(
  KINEMATIC_GROUP_WIDGET_headers_UI include/tesseract_qt/kinematic_groups/widgets/kinematic_groups_editor_widget.ui
  include/tesseract_qt/kinematic_groups/widgets/group_joint_states_editor_widget.ui)

add_library(
  ${PROJECT_NAME}_kinematic_groups_widgets SHARED
  ${KINEMATIC_GROUP_WIDGET_headers_MOC}
  ${KINEMATIC_GROUP_WIDGET_headers_UI}
  src/widgets/group_joint_states_widget.cpp
  src/widgets/group_joint_states_editor_widget.cpp
  src/widgets/group_tcps_widget.cpp
  src/widgets/kinematic_groups_widget.cpp
  src/widgets/kinematic_groups_editor_widget.cpp)
target_link_libraries(
  ${PROJECT_NAME}_kinematic_groups_widgets
  PUBLIC Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_kinematic_groups_models
         ${PROJECT_NAME}_joint_state_slider)
target_include_directories(
  ${PROJECT_NAME}_kinematic_groups_widgets
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/kinematic_groups>" "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_kinematic_groups_models ${PROJECT_NAME}_kinematic_groups_widgets)
