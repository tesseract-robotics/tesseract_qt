find_package(tesseract_common REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

# Define compile-time default variables
if(MSVC)
  set(TESSERACT_STUDIO_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/bin)
else()
  set(TESSERACT_STUDIO_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/lib)
endif()

# Variable for task composer plugins
set(TESSERACT_STUDIO_PLUGINS "")

qt5_wrap_cpp(STUDIO_PLUGIN_FACTORY_headers_MOC include/tesseract_qt/studio/studio_dock_widget.h
             include/tesseract_qt/studio/studio_plugin_config_widget.h)

add_library(
  ${PROJECT_NAME}_studio_plugin_factory
  ${STUDIO_PLUGIN_FACTORY_headers_MOC}
  src/studio_plugin_factory.cpp
  src/studio_dock_widget.cpp
  src/studio_plugin_config_widget.cpp)

target_link_libraries(
  ${PROJECT_NAME}_studio_plugin_factory
  PUBLIC tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_studio_plugin_factory
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/studio>"
         "$<INSTALL_INTERFACE:include>")
target_compile_definitions(${PROJECT_NAME}_studio_plugin_factory
                           PRIVATE TESSERACT_STUDIO_PLUGIN_PATH="${TESSERACT_STUDIO_PLUGIN_PATH}")

qt5_wrap_cpp(
  STUDIO_PLUGINS_headers_MOC
  include/tesseract_qt/studio/plugins/render/studio_render_config_dialog.h
  include/tesseract_qt/studio/plugins/render/studio_render_dock_widget.h
  include/tesseract_qt/studio/plugins/render/studio_render_plugin_config_widget.h)
qt5_wrap_ui(STUDIO_PLUGINS_headers_UI include/tesseract_qt/studio/plugins/render/studio_render_config_dialog.ui
            include/tesseract_qt/studio/plugins/render/studio_render_plugin_config_widget.ui)

add_library(
  ${PROJECT_NAME}_studio_plugins
  ${STUDIO_PLUGINS_headers_MOC}
  ${STUDIO_PLUGINS_headers_UI}
  src/plugins/render/studio_render_config_dialog.cpp
  src/plugins/render/studio_render_dock_widget.cpp
  src/plugins/render/studio_render_plugin_config_widget.cpp
  src/plugins/studio_plugin_factories.cpp)

target_link_libraries(
  ${PROJECT_NAME}_studio_plugins
  PUBLIC tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${PROJECT_NAME}_studio_plugin_factory
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_render_widgets)
target_include_directories(
  ${PROJECT_NAME}_studio_plugins PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/studio>" "$<INSTALL_INTERFACE:include>")

# Add factory library so studio plugins can find these factories by defauult
set(TESSERACT_STUDIO_PLUGINS ${TESSERACT_STUDIO_PLUGINS} "${PROJECT_NAME}_studio_plugins")

# Add compiler definition to core so it can find all plugins produced
string(
  REPLACE ";"
          ":"
          TESSERACT_STUDIO_PLUGINS_STRING
          "${TESSERACT_STUDIO_PLUGINS}")
target_compile_definitions(${PROJECT_NAME}_studio_plugin_factory
                           PRIVATE TESSERACT_STUDIO_PLUGINS="${TESSERACT_STUDIO_PLUGINS_STRING}")

qt5_wrap_cpp(STUDIO_WIDGET_headers_MOC include/tesseract_qt/studio/studio.h
             include/tesseract_qt/studio/studio_plugin_loader_dialog.h)
qt5_wrap_ui(STUDIO_WIDGET_headers_UI include/tesseract_qt/studio/studio.ui
            include/tesseract_qt/studio/studio_plugin_loader_dialog.ui)

add_executable(
  ${PROJECT_NAME}_studio
  ${STUDIO_WIDGET_headers_MOC}
  ${STUDIO_WIDGET_headers_UI}
  src/studio.cpp
  src/studio_app.cpp
  src/studio_plugin_loader_dialog.cpp)

target_link_libraries(
  ${PROJECT_NAME}_studio
  PRIVATE tesseract::tesseract_common
          Qt5::Core
          Qt5::Gui
          Qt5::Widgets
          ${PROJECT_NAME}_studio_plugin_factory
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
install_targets(
  TARGETS
  ${PROJECT_NAME}_studio
  ${PROJECT_NAME}_studio_plugin_factory
  ${PROJECT_NAME}_studio_plugins)
