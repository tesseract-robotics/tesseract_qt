add_executable(${PROJECT_NAME}_yaml_model_demo yaml_model_demo.cpp)
target_link_libraries(${PROJECT_NAME}_yaml_model_demo PRIVATE ${PROJECT_NAME}_common)

install_targets(TARGETS ${PROJECT_NAME}_yaml_model_demo)
