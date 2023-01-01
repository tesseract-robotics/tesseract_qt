find_package(tesseract_support REQUIRED)
add_executable(${PROJECT_NAME}_render_widget_demo render_widget_demo.cpp)
target_link_libraries(
  ${PROJECT_NAME}_render_widget_demo
  PRIVATE ${PROJECT_NAME}_rendering
          tesseract::tesseract_common
          Qt5::Core
          Qt5::Widgets)

add_executable(${PROJECT_NAME}_render_environment_widget_demo render_environment_widget_demo.cpp)
target_link_libraries(
  ${PROJECT_NAME}_render_environment_widget_demo
  PRIVATE ${PROJECT_NAME}_rendering
          ${PROJECT_NAME}_environment_widgets
          tesseract::tesseract_common
          tesseract::tesseract_support
          Qt5::Core
          Qt5::Widgets)

install_targets(TARGETS ${PROJECT_NAME}_render_widget_demo ${PROJECT_NAME}_render_environment_widget_demo)
