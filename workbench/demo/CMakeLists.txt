find_package(tesseract REQUIRED COMPONENTS urdf srdf)

add_executable(${PROJECT_NAME}_workbench_demo workbench_demo.cpp)
target_link_libraries(
  ${PROJECT_NAME}_workbench_demo
  PRIVATE tesseract::urdf
          tesseract::srdf
          ${PROJECT_NAME}_workbench_widgets
          ${PROJECT_NAME}_environment_widgets
          ${PROJECT_NAME}_common)

install_targets(TARGETS ${PROJECT_NAME}_workbench_demo)
