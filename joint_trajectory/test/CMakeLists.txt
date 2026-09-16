find_gtest()
find_package(tesseract REQUIRED COMPONENTS common environment scene_graph)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(${PROJECT_NAME}_joint_trajectory_model_unit joint_trajectory_model_unit.cpp)
target_link_libraries(
  ${PROJECT_NAME}_joint_trajectory_model_unit
  PRIVATE GTest::GTest
          Qt5::Widgets
          tesseract::common
          tesseract::environment
          tesseract::scene_graph
          ${PROJECT_NAME}_common
          ${PROJECT_NAME}_joint_trajectory_models)
target_compile_definitions(${PROJECT_NAME}_joint_trajectory_model_unit PRIVATE ${TESSERACT_COMPILE_DEFINITIONS_PUBLIC})
add_gtest_discover_tests(${PROJECT_NAME}_joint_trajectory_model_unit)
