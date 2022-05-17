add_executable(${PROJECT_NAME}_acm_demo acm_demo.cpp)
target_link_libraries(${PROJECT_NAME}_acm_demo PRIVATE ${PROJECT_NAME}_acm)

install_targets(TARGETS ${PROJECT_NAME}_acm_demo)
