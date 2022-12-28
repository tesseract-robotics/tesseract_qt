find_package(tesseract_common REQUIRED)
find_package(tesseract_scene_graph REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(SCENE_GRAPH_MODEL_headers_MOC include/tesseract_qt/scene_graph/scene_state_model.h
             include/tesseract_qt/scene_graph/scene_graph_model.h)

add_library(
  ${PROJECT_NAME}_scene_graph_models SHARED
  ${SCENE_GRAPH_MODEL_headers_MOC}
  src/calibration_standard_item.cpp
  src/collision_standard_item.cpp
  src/dynamics_standard_item.cpp
  src/inertial_standard_item.cpp
  src/joint_standard_item.cpp
  src/limits_standard_item.cpp
  src/link_standard_item.cpp
  src/material_standard_item.cpp
  src/mimic_standard_item.cpp
  src/safety_standard_item.cpp
  src/visual_standard_item.cpp
  src/box_standard_item.cpp
  src/capsule_standard_item.cpp
  src/cone_standard_item.cpp
  src/cylinder_standard_item.cpp
  src/plane_standard_item.cpp
  src/sphere_standard_item.cpp
  src/polygon_mesh_standard_item.cpp
  src/octree_standard_item.cpp
  src/scene_graph_events.cpp
  src/scene_graph_standard_item.cpp
  src/scene_state_standard_item.cpp
  src/scene_state_model.cpp
  src/scene_graph_model.cpp)

target_link_libraries(
  ${PROJECT_NAME}_scene_graph_models
  PUBLIC tesseract::tesseract_scene_graph
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_scene_graph_models PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                            "$<INSTALL_INTERFACE:include>")

qt5_wrap_cpp(
  SCENE_GRAPH_WIDGET_headers_MOC
  include/tesseract_qt/scene_graph/scene_state_widget.h
  include/tesseract_qt/scene_graph/scene_graph_widget.h
  include/tesseract_qt/scene_graph/scene_graph_tool_bar.h)

add_library(
  ${PROJECT_NAME}_scene_graph_widgets SHARED
  ${SCENE_GRAPH_WIDGET_headers_MOC}
  src/scene_graph_widget.cpp
  src/scene_graph_tool_bar.cpp
  src/scene_state_widget.cpp)

target_link_libraries(
  ${PROJECT_NAME}_scene_graph_widgets
  PUBLIC tesseract::tesseract_scene_graph
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${PROJECT_NAME}_scene_graph_models
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_scene_graph_widgets PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                             "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_scene_graph_models ${PROJECT_NAME}_scene_graph_widgets)
