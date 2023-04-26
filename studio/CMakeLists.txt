find_package(tesseract_common REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(STUDIO_WIDGET_headers_MOC include/tesseract_qt/studio/studio.h)
qt5_wrap_ui(STUDIO_headers_UI include/tesseract_qt/studio/studio.ui)

add_executable(
  ${PROJECT_NAME}_studio
  ${STUDIO_WIDGET_headers_MOC}
  ${STUDIO_headers_UI}
  src/studio.cpp
  src/studio_app.cpp)

target_link_libraries(
  ${PROJECT_NAME}_studio
  PRIVATE tesseract::tesseract_common
          Qt5::Core
          Qt5::Gui
          Qt5::Widgets
          ${PROJECT_NAME}_common
          ${PROJECT_NAME}_render_widgets
          ${PROJECT_NAME}_environment_widgets
          ${PROJECT_NAME}_joint_trajectory_widgets
          ${PROJECT_NAME}_tool_path_widgets
          ${PROJECT_NAME}_manipulation_widgets)
target_include_directories(
  ${PROJECT_NAME}_studio PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                 "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/studio>" "$<INSTALL_INTERFACE:include>")

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_studio)
