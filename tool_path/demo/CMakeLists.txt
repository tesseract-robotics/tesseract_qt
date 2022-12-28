add_executable(${PROJECT_NAME}_tool_path_demo tool_path_demo.cpp)
target_link_libraries(${PROJECT_NAME}_tool_path_demo PRIVATE ${PROJECT_NAME}_tool_path_models
                                                             ${PROJECT_NAME}_tool_path_widgets)

install_targets(TARGETS ${PROJECT_NAME}_tool_path_demo)
