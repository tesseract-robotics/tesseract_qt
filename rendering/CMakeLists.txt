find_package(tesseract_common REQUIRED)
find_package(
  Qt5
  COMPONENTS Core
             Gui
             Widgets
             OpenGL
  REQUIRED)

find_package(gz-rendering7 REQUIRED)
find_package(gz-common5 COMPONENTS profiler events av REQUIRED)
find_package(gz-math7-eigen3 REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(
  RENDERING_WIDGET_headers_MOC
  include/tesseract_qt/rendering/render_events.h
  include/tesseract_qt/rendering/interactive_view_control.h
  include/tesseract_qt/rendering/render_widget.h)

add_library(
  ${PROJECT_NAME}_render_widgets SHARED
  ${RENDERING_WIDGET_headers_MOC}
  src/conversions.cpp
  src/interactive_view_control.cpp
  src/ign_contact_results_render_manager.cpp
  src/ign_scene_graph_render_manager.cpp
  src/ign_tool_path_render_manager.cpp
  src/render_events.cpp
  src/render_widget.cpp
  src/utils.cpp)

target_link_libraries(
  ${PROJECT_NAME}_render_widgets
  PUBLIC tesseract::tesseract_common
         gz-common5::gz-common5-profiler
         gz-common5::gz-common5-events
         gz-common5::gz-common5-av
         gz-rendering7::gz-rendering7
         gz-math7::gz-math7-eigen3
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         Qt5::OpenGL
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_render_widgets PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                        "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_render_widgets)
