find_package(tesseract_urdf REQUIRED)
find_package(tesseract_srdf REQUIRED)

add_executable(${PROJECT_NAME}_planning_task_composer_demo task_composer_demo.cpp)
target_link_libraries(
  ${PROJECT_NAME}_planning_task_composer_demo
  PRIVATE tesseract::tesseract_urdf
          tesseract::tesseract_srdf
          tesseract::tesseract_environment
          tesseract::tesseract_common
          ${PROJECT_NAME}_planning_widgets
          ${PROJECT_NAME}_common)

install_targets(TARGETS ${PROJECT_NAME}_planning_task_composer_demo)
