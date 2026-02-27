find_package(tesseract REQUIRED COMPONENTS urdf)

add_executable(${PROJECT_NAME}_joint_state_slider_demo joint_state_slider_demo.cpp)
target_link_libraries(${PROJECT_NAME}_joint_state_slider_demo PRIVATE ${PROJECT_NAME}_joint_state_slider
                                                                      tesseract::urdf)

install_targets(TARGETS ${PROJECT_NAME}_joint_state_slider_demo)
