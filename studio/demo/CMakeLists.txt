add_executable(${PROJECT_NAME}_plot_docker_demo plot_docker_demo.cpp)
target_link_libraries(${PROJECT_NAME}_plot_docker_demo PRIVATE ${PROJECT_NAME}_studio_plugin_factory)
install_targets(TARGETS ${PROJECT_NAME}_plot_docker_demo)
