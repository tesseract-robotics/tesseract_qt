add_executable(${PROJECT_NAME}_joint_trajectory_demo joint_trajectory_demo.cpp)
target_link_libraries(${PROJECT_NAME}_joint_trajectory_demo PRIVATE ${PROJECT_NAME}_joint_trajectory_models
                                                                    ${PROJECT_NAME}_joint_trajectory_widgets)

install_targets(TARGETS ${PROJECT_NAME}_joint_trajectory_demo)
