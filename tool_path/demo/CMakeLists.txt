add_executable(${PROJECT_NAME}_tool_path_demo tool_path_demo.cpp)
target_link_libraries(${PROJECT_NAME}_tool_path_demo PRIVATE ${PROJECT_NAME}_tool_path_model
                                                             ${PROJECT_NAME}_tool_path_widget)

install_targets(TARGETS ${PROJECT_NAME}_tool_path_demo)
