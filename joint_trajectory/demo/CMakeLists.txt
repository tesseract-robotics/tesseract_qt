add_executable(${PROJECT_NAME}_joint_trajectory_demo joint_trajectory_demo.cpp)
target_link_libraries(${PROJECT_NAME}_joint_trajectory_demo PRIVATE ${PROJECT_NAME}_joint_trajectory_model
                                                                    ${PROJECT_NAME}_joint_trajectory_widget)

install_targets(TARGETS ${PROJECT_NAME}_joint_trajectory_demo)
