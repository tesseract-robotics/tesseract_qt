add_executable(${PROJECT_NAME}_plot_demo plot_demo.cpp)
target_link_libraries(${PROJECT_NAME}_plot_demo PRIVATE ${PROJECT_NAME}_plot)
target_include_directories(${PROJECT_NAME}_plot_demo PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                                            "$<INSTALL_INTERFACE:include>")

add_executable(${PROJECT_NAME}_plot_docker_demo plot_docker_demo.cpp)
target_link_libraries(${PROJECT_NAME}_plot_docker_demo PRIVATE ${PROJECT_NAME}_plot)
target_include_directories(
  ${PROJECT_NAME}_plot_docker_demo PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                          "$<INSTALL_INTERFACE:include>")

add_executable(${PROJECT_NAME}_plot_subplots_demo plot_subplots_demo.cpp)
target_link_libraries(${PROJECT_NAME}_plot_subplots_demo PRIVATE ${PROJECT_NAME}_plot)
target_include_directories(
  ${PROJECT_NAME}_plot_subplots_demo PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                            "$<INSTALL_INTERFACE:include>")

install_targets(
  TARGETS
  ${PROJECT_NAME}_plot_demo
  ${PROJECT_NAME}_plot_docker_demo
  ${PROJECT_NAME}_plot_subplots_demo)
