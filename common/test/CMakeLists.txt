find_gtest()

add_executable(${PROJECT_NAME}_property_tree_model_unit property_tree_model_unit.cpp)
target_link_libraries(${PROJECT_NAME}_property_tree_model_unit PRIVATE GTest::GTest ${PROJECT_NAME}_common)
target_compile_options(${PROJECT_NAME}_property_tree_model_unit PRIVATE ${TESSERACT_COMPILE_OPTIONS_PRIVATE}
                                                                        ${TESSERACT_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(${PROJECT_NAME}_property_tree_model_unit PRIVATE ${TESSERACT_COMPILE_DEFINITIONS_PUBLIC})
target_clang_tidy(${PROJECT_NAME}_property_tree_model_unit ENABLE ${TESSERACT_ENABLE_CLANG_TIDY})
target_cxx_version(${PROJECT_NAME}_property_tree_model_unit PRIVATE VERSION ${TESSERACT_CXX_VERSION})
target_code_coverage(
  ${PROJECT_NAME}_property_tree_model_unit
  PRIVATE
  ALL
  EXCLUDE ${COVERAGE_EXCLUDE}
  ENABLE ${TESSERACT_ENABLE_CODE_COVERAGE})
add_gtest_discover_tests(${PROJECT_NAME}_property_tree_model_unit DISCOVERY_MODE PRE_TEST)

add_executable(${PROJECT_NAME}_property_tree_editor_widget_unit property_tree_editor_widget_unit.cpp)
target_link_libraries(${PROJECT_NAME}_property_tree_editor_widget_unit PRIVATE GTest::GTest ${PROJECT_NAME}_common)
target_compile_options(${PROJECT_NAME}_property_tree_editor_widget_unit PRIVATE ${TESSERACT_COMPILE_OPTIONS_PRIVATE}
                                                                                ${TESSERACT_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(${PROJECT_NAME}_property_tree_editor_widget_unit
                           PRIVATE ${TESSERACT_COMPILE_DEFINITIONS_PUBLIC})
target_clang_tidy(${PROJECT_NAME}_property_tree_editor_widget_unit ENABLE ${TESSERACT_ENABLE_CLANG_TIDY})
target_cxx_version(${PROJECT_NAME}_property_tree_editor_widget_unit PRIVATE VERSION ${TESSERACT_CXX_VERSION})
target_code_coverage(
  ${PROJECT_NAME}_property_tree_editor_widget_unit
  PRIVATE
  ALL
  EXCLUDE ${COVERAGE_EXCLUDE}
  ENABLE ${TESSERACT_ENABLE_CODE_COVERAGE})
add_gtest_discover_tests(${PROJECT_NAME}_property_tree_editor_widget_unit DISCOVERY_MODE PRE_TEST)

add_executable(${PROJECT_NAME}_plugin_schema_resolver_unit plugin_schema_resolver_unit.cpp)
target_link_libraries(${PROJECT_NAME}_plugin_schema_resolver_unit PRIVATE GTest::GTest GTest::Main
                                                                          ${PROJECT_NAME}_common)
get_filename_component(TESSERACT_PLUGIN_LIBRARY_DIR "${tesseract_DIR}/../.." ABSOLUTE)
get_filename_component(TESSERACT_PLANNING_PLUGIN_LIBRARY_DIR "${tesseract_planning_DIR}/../.." ABSOLUTE)
target_compile_options(${PROJECT_NAME}_plugin_schema_resolver_unit PRIVATE ${TESSERACT_COMPILE_OPTIONS_PRIVATE}
                                                                           ${TESSERACT_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(
  ${PROJECT_NAME}_plugin_schema_resolver_unit
  PRIVATE ${TESSERACT_COMPILE_DEFINITIONS_PUBLIC}
          TESSERACT_PLUGIN_LIBRARY_DIR="${TESSERACT_PLUGIN_LIBRARY_DIR}"
          TESSERACT_PLANNING_PLUGIN_LIBRARY_DIR="${TESSERACT_PLANNING_PLUGIN_LIBRARY_DIR}"
          TESSERACT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../../../tesseract-1")
target_clang_tidy(${PROJECT_NAME}_plugin_schema_resolver_unit ENABLE ${TESSERACT_ENABLE_CLANG_TIDY})
target_cxx_version(${PROJECT_NAME}_plugin_schema_resolver_unit PRIVATE VERSION ${TESSERACT_CXX_VERSION})
target_code_coverage(
  ${PROJECT_NAME}_plugin_schema_resolver_unit
  PRIVATE
  ALL
  EXCLUDE ${COVERAGE_EXCLUDE}
  ENABLE ${TESSERACT_ENABLE_CODE_COVERAGE})
add_gtest_discover_tests(${PROJECT_NAME}_plugin_schema_resolver_unit)

add_executable(${PROJECT_NAME}_config_editor_widget_unit config_editor_widget_unit.cpp)
target_link_libraries(${PROJECT_NAME}_config_editor_widget_unit PRIVATE GTest::GTest ${PROJECT_NAME}_common)
target_compile_options(${PROJECT_NAME}_config_editor_widget_unit PRIVATE ${TESSERACT_COMPILE_OPTIONS_PRIVATE}
                                                                         ${TESSERACT_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(
  ${PROJECT_NAME}_config_editor_widget_unit
  PRIVATE ${TESSERACT_COMPILE_DEFINITIONS_PUBLIC}
          TESSERACT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../../../tesseract-1")
target_clang_tidy(${PROJECT_NAME}_config_editor_widget_unit ENABLE ${TESSERACT_ENABLE_CLANG_TIDY})
target_cxx_version(${PROJECT_NAME}_config_editor_widget_unit PRIVATE VERSION ${TESSERACT_CXX_VERSION})
target_code_coverage(
  ${PROJECT_NAME}_config_editor_widget_unit
  PRIVATE
  ALL
  EXCLUDE ${COVERAGE_EXCLUDE}
  ENABLE ${TESSERACT_ENABLE_CODE_COVERAGE})
add_gtest_discover_tests(${PROJECT_NAME}_config_editor_widget_unit DISCOVERY_MODE PRE_TEST)
