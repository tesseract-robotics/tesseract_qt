find_package(tesseract_common REQUIRED)
find_package(tesseract_scene_graph REQUIRED)
find_package(tesseract_environment REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS include/tesseract_qt/manipulation)

set(MANIPULATION_WIDGET_headers_MOC include/tesseract_qt/manipulation/manipulation_widget.h
                                    include/tesseract_qt/manipulation/manipulation_tool_bar.h)

add_library(${PROJECT_NAME}_manipulation_widgets SHARED ${MANIPULATION_WIDGET_headers_MOC} src/manipulation_widget.cpp
                                                        src/manipulation_tool_bar.cpp)

target_link_libraries(
  ${PROJECT_NAME}_manipulation_widgets
  PUBLIC tesseract::tesseract_environment
         tesseract::tesseract_scene_graph
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Widgets
         Qt5::Gui
         ${PROJECT_NAME}_common
         ${PROJECT_NAME}_scene_graph_widgets
         ${PROJECT_NAME}_joint_state_slider)
target_include_directories(
  ${PROJECT_NAME}_manipulation_widgets
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/manipulation>"
         "$<INSTALL_INTERFACE:include>")

# add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
list(APPEND PROJECT_TARGETS ${PROJECT_NAME}_manipulation_widgets)
set(PROJECT_TARGETS ${PROJECT_TARGETS} PARENT_SCOPE)
install_targets(TARGETS ${PROJECT_NAME}_manipulation_widgets)
