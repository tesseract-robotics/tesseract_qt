find_package(tesseract_common REQUIRED)
find_package(tesseract_visualization REQUIRED)
find_package(tesseract_command_language REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(TRAJ_MODEL_headers_MOC include/tesseract_qt/joint_trajectory/joint_trajectory_model.h)

add_library(
  ${PROJECT_NAME}_joint_trajectory_models SHARED
  ${TRAJ_MODEL_headers_MOC}
  src/joint_trajectory_info_item.cpp
  src/joint_trajectory_set_item.cpp
  src/joint_trajectory_state_item.cpp
  src/joint_trajectory_model.cpp)
target_link_libraries(
  ${PROJECT_NAME}_joint_trajectory_models
  PUBLIC tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_joint_trajectory_models PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                                 "$<INSTALL_INTERFACE:include>")

qt5_wrap_cpp(
  TRAJ_WIDGET_headers_MOC
  include/tesseract_qt/joint_trajectory/joint_trajectory_widget.h
  include/tesseract_qt/joint_trajectory/joint_trajectory_tool_bar.h
  include/tesseract_qt/joint_trajectory/joint_trajectory_plot_dialog.h)
qt5_wrap_ui(TRAJ_WIDGET_headers_UI include/tesseract_qt/joint_trajectory/joint_trajectory_widget.ui
            include/tesseract_qt/joint_trajectory/joint_trajectory_plot_dialog.ui)

add_library(
  ${PROJECT_NAME}_joint_trajectory_widgets SHARED
  ${TRAJ_WIDGET_headers_MOC}
  ${TRAJ_WIDGET_headers_UI}
  src/joint_trajectory_tool_bar.cpp
  src/joint_trajectory_widget.cpp
  src/joint_trajectory_plot_dialog.cpp)

target_link_libraries(
  ${PROJECT_NAME}_joint_trajectory_widgets
  PUBLIC tesseract::tesseract_common
         tesseract::tesseract_visualization
         tesseract::tesseract_command_language
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${PROJECT_NAME}_plot
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_joint_trajectory_models)
target_include_directories(
  ${PROJECT_NAME}_joint_trajectory_widgets
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/joint_trajectory>" "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_joint_trajectory_models ${PROJECT_NAME}_joint_trajectory_widgets)
