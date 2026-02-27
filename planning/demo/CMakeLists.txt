find_package(tesseract REQUIRED COMPONENTS urdf srdf)

add_executable(${PROJECT_NAME}_planning_task_composer_demo task_composer_demo.cpp)
target_link_libraries(
  ${PROJECT_NAME}_planning_task_composer_demo
  PRIVATE tesseract::urdf
          tesseract::srdf
          tesseract::environment
          tesseract::common
          ${PROJECT_NAME}_planning_widgets
          ${PROJECT_NAME}_common)

install_targets(TARGETS ${PROJECT_NAME}_planning_task_composer_demo)
