find_package(tesseract_common REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(TOOL_PATH_MODEL_headers_MOC include/tesseract_qt/tool_path/tool_path_model.h)

add_library(${PROJECT_NAME}_tool_path_model SHARED ${TOOL_PATH_MODEL_headers_MOC} src/tool_path_events.cpp
                                                   src/tool_path_model.cpp)
target_link_libraries(
  ${PROJECT_NAME}_tool_path_model
  PUBLIC tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_tool_path_model
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/tool_path>"
         "$<INSTALL_INTERFACE:include>")
add_dependencies(${PROJECT_NAME}_tool_path_model ${PROJECT_NAME}_common)

qt5_wrap_cpp(TOOL_PATH_WIDGET_headers_MOC
    include/tesseract_qt/tool_path/tool_path_tool_bar.h
    include/tesseract_qt/tool_path/tool_path_tree_view.h)

add_library(${PROJECT_NAME}_tool_path_widget SHARED ${TOOL_PATH_WIDGET_headers_MOC} src/tool_path_tool_bar.cpp src/tool_path_tree_view.cpp)

target_link_libraries(
  ${PROJECT_NAME}_tool_path_widget
  PUBLIC tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${PROJECT_NAME}_tool_path_model)
target_include_directories(
  ${PROJECT_NAME}_joint_trajectory_widget
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/tool_path>"
         "$<INSTALL_INTERFACE:include>")
add_dependencies(${PROJECT_NAME}_tool_path_widget ${PROJECT_NAME}_plot ${PROJECT_NAME}_common)

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_tool_path_model ${PROJECT_NAME}_tool_path_widget)
