find_package(tesseract REQUIRED COMPONENTS urdf)

add_executable(${PROJECT_NAME}_scene_graph_demo scene_graph_demo.cpp)
target_link_libraries(${PROJECT_NAME}_scene_graph_demo PRIVATE tesseract::urdf ${PROJECT_NAME}_scene_graph_widgets
                                                               ${PROJECT_NAME}_common)

install_targets(TARGETS ${PROJECT_NAME}_scene_graph_demo)
