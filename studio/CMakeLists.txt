find_package(tesseract_common REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(
  qtadvanceddocking
  NAMES
  qtadvanceddocking
  qt5advanceddocking
  qt6advanceddocking
  REQUIRED)
if(TARGET ads::qtadvanceddocking)
  set(ADS_TARGET ads::qtadvanceddocking)
elseif(TARGET ads::qt5advanceddocking)
  set(ADS_TARGET ads::qt5advanceddocking)
elseif(TARGET ads::qt6advanceddocking)
  set(ADS_TARGET ads::qt6advanceddocking)
endif()

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

qt5_wrap_ui(STUDIO_PLUGIN_FACTORY_headers_UI include/tesseract_qt/studio/plot_docker_toolbar.ui)
qt5_wrap_cpp(STUDIO_PLUGIN_FACTORY_headers_MOC include/tesseract_qt/studio/studio_dock_widget.h
             include/tesseract_qt/studio/plot_docker.h)

add_library(
  ${PROJECT_NAME}_studio_plugin_factory
  ${STUDIO_PLUGIN_FACTORY_headers_UI}
  ${STUDIO_PLUGIN_FACTORY_headers_MOC}
  src/studio_dock_widget_factory.cpp
  src/studio_dock_widget.cpp
  src/plot_docker.cpp)

target_link_libraries(
  ${PROJECT_NAME}_studio_plugin_factory
  PUBLIC tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${ADS_TARGET}
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_plot)
target_include_directories(
  ${PROJECT_NAME}_studio_plugin_factory
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/studio>"
         "$<INSTALL_INTERFACE:include>")
target_compile_definitions(${PROJECT_NAME}_studio_plugin_factory
                           PRIVATE TESSERACT_STUDIO_PLUGIN_PATH="${TESSERACT_STUDIO_PLUGIN_PATH}")

qt5_wrap_cpp(
  STUDIO_PLUGINS_headers_MOC
  include/tesseract_qt/studio/plugins/manipulation/studio_manipulation_config_dialog.h
  include/tesseract_qt/studio/plugins/manipulation/studio_manipulation_dock_widget.h
  include/tesseract_qt/studio/plugins/render/studio_render_config_dialog.h
  include/tesseract_qt/studio/plugins/render/studio_render_dock_widget.h
  include/tesseract_qt/studio/plugins/studio_environment_dock_widget.h
  include/tesseract_qt/studio/plugins/studio_joint_trajectory_dock_widget.h
  include/tesseract_qt/studio/plugins/studio_tool_path_dock_widget.h)
qt5_wrap_ui(
  STUDIO_PLUGINS_headers_UI include/tesseract_qt/studio/plugins/manipulation/studio_manipulation_config_dialog.ui
  include/tesseract_qt/studio/plugins/render/studio_render_config_dialog.ui)

add_library(
  ${PROJECT_NAME}_studio_plugins
  ${STUDIO_PLUGINS_headers_MOC}
  ${STUDIO_PLUGINS_headers_UI}
  src/plugins/manipulation/studio_manipulation_config_dialog.cpp
  src/plugins/manipulation/studio_manipulation_dock_widget.cpp
  src/plugins/render/studio_render_config_dialog.cpp
  src/plugins/render/studio_render_dock_widget.cpp
  src/plugins/studio_environment_dock_widget.cpp
  src/plugins/studio_joint_trajectory_dock_widget.cpp
  src/plugins/studio_tool_path_dock_widget.cpp
  src/plugins/studio_plugin_factories.cpp)

target_link_libraries(
  ${PROJECT_NAME}_studio_plugins
  PUBLIC tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${PROJECT_NAME}_studio_plugin_factory
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_render_widgets
         ${PROJECT_NAME}_scene_graph_widgets
         ${PROJECT_NAME}_environment_widgets
         ${PROJECT_NAME}_joint_trajectory_widgets
         ${PROJECT_NAME}_manipulation_widgets
         ${PROJECT_NAME}_tool_path_widgets)
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

qt5_wrap_cpp(STUDIO_WIDGET_headers_MOC include/tesseract_qt/studio/studio.h
             include/tesseract_qt/studio/studio_plugin_loader_dialog.h)
qt5_wrap_ui(STUDIO_WIDGET_headers_UI include/tesseract_qt/studio/studio.ui
            include/tesseract_qt/studio/studio_plugin_loader_dialog.ui)

add_library(
  ${PROJECT_NAME}_studio_widgets
  ${STUDIO_WIDGET_headers_MOC}
  ${STUDIO_WIDGET_headers_UI}
  src/studio.cpp
  src/studio_plugin_loader_dialog.cpp)

target_link_libraries(
  ${PROJECT_NAME}_studio_widgets
  PUBLIC tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${ADS_TARGET}
         Boost::program_options
         ${PROJECT_NAME}_studio_plugin_factory
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_studio_widgets PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/studio>" "$<INSTALL_INTERFACE:include>")
target_compile_definitions(${PROJECT_NAME}_studio_widgets
                           PRIVATE TESSERACT_STUDIO_PLUGIN_PATH="${TESSERACT_STUDIO_PLUGIN_PATH}")
target_compile_definitions(${PROJECT_NAME}_studio_widgets
                           PRIVATE TESSERACT_STUDIO_PLUGINS="${TESSERACT_STUDIO_PLUGINS_STRING}")

add_executable(${PROJECT_NAME}_studio src/studio_app.cpp)
target_link_libraries(
  ${PROJECT_NAME}_studio
  PRIVATE Qt5::Core
          Qt5::Gui
          Qt5::Widgets
          ${PROJECT_NAME}_studio_widgets
          ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_studio PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                 "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/studio>" "$<INSTALL_INTERFACE:include>")

# Add the demo subdirectory
add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

foreach(dir config)
  install(DIRECTORY ${dir}/ DESTINATION share/${PROJECT_NAME}/studio/${dir})
endforeach()

# Plugin should not be export only installed
install(
  TARGETS ${PROJECT_NAME}_studio_plugins
  COMPONENT taskflow
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

# Install
list(
  APPEND
  PROJECT_TARGETS
  ${PROJECT_NAME}_studio
  ${PROJECT_NAME}_studio_widgets
  ${PROJECT_NAME}_studio_plugin_factory)
set(PROJECT_TARGETS ${PROJECT_TARGETS} PARENT_SCOPE)
install_targets(
  TARGETS
  ${PROJECT_NAME}_studio
  ${PROJECT_NAME}_studio_widgets
  ${PROJECT_NAME}_studio_plugin_factory)
