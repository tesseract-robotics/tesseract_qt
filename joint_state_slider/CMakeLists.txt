find_package(tesseract_common REQUIRED)
find_package(tesseract_scene_graph REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(JOINT_STATE_SLIDER_WIDGET_headers_MOC include/tesseract_qt/joint_state_slider/joint_state_slider_widget.h)
qt5_wrap_ui(JOINT_STATE_SLIDER_WIDGET_headers_UI include/tesseract_qt/joint_state_slider/joint_state_slider_widget.ui)

add_library(
  ${PROJECT_NAME}_joint_state_slider SHARED ${JOINT_STATE_SLIDER_WIDGET_headers_MOC}
                                            ${JOINT_STATE_SLIDER_WIDGET_headers_UI} src/joint_state_slider_widget.cpp)
target_link_libraries(
  ${PROJECT_NAME}_joint_state_slider
  PUBLIC Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         tesseract::tesseract_scene_graph
         tesseract::tesseract_common)
target_include_directories(
  ${PROJECT_NAME}_joint_state_slider
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/joint_state_slider>" "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_joint_state_slider)
