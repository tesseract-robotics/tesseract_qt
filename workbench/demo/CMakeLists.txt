find_package(tesseract_urdf REQUIRED)
find_package(tesseract_srdf REQUIRED)

add_executable(${PROJECT_NAME}_workbench_demo workbench_demo.cpp)
target_link_libraries(
  ${PROJECT_NAME}_workbench_demo
  PRIVATE tesseract::tesseract_urdf
          tesseract::tesseract_srdf
          ${PROJECT_NAME}_workbench_widgets
          ${PROJECT_NAME}_environment_widgets
          ${PROJECT_NAME}_common)

install_targets(TARGETS ${PROJECT_NAME}_workbench_demo)
