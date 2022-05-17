find_package(tesseract_support REQUIRED)
find_package(tesseract_urdf REQUIRED)

add_executable(${PROJECT_NAME}_joint_state_slider_demo joint_state_slider_demo.cpp)
target_link_libraries(${PROJECT_NAME}_joint_state_slider_demo
                      PRIVATE ${PROJECT_NAME}_joint_state_slider tesseract::tesseract_support tesseract::tesseract_urdf)

install_targets(TARGETS ${PROJECT_NAME}_joint_state_slider_demo)
