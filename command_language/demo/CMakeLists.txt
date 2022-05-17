add_executable(${PROJECT_NAME}_command_language_demo command_language_demo.cpp)
target_link_libraries(${PROJECT_NAME}_command_language_demo PRIVATE tesseract::tesseract_command_language
                                                                    ${PROJECT_NAME}_command_language)

install_targets(TARGETS ${PROJECT_NAME}_command_language_demo)
