find_gtest()

add_executable(${PROJECT_NAME}_gazebo_utils_unit gazebo_utils_unit.cpp)
target_link_libraries(${PROJECT_NAME}_gazebo_utils_unit PRIVATE GTest::GTest GTest::Main ${PROJECT_NAME}_gazebo_utils)
target_compile_options(${PROJECT_NAME}_gazebo_utils_unit PRIVATE ${TESSERACT_COMPILE_OPTIONS_PRIVATE}
                                                                 ${TESSERACT_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(${PROJECT_NAME}_gazebo_utils_unit PRIVATE ${TESSERACT_COMPILE_DEFINITIONS_PUBLIC})
target_clang_tidy(${PROJECT_NAME}_gazebo_utils_unit ENABLE ${TESSERACT_ENABLE_CLANG_TIDY})
target_cxx_version(${PROJECT_NAME}_gazebo_utils_unit PRIVATE VERSION ${TESSERACT_CXX_VERSION})
target_code_coverage(
  ${PROJECT_NAME}_gazebo_utils_unit
  PRIVATE
  ALL
  EXCLUDE ${COVERAGE_EXCLUDE}
  ENABLE ${TESSERACT_ENABLE_CODE_COVERAGE})
add_gtest_discover_tests(${PROJECT_NAME}_gazebo_utils_unit DISCOVERY_MODE PRE_TEST)
