find_package(tesseract_common REQUIRED)
find_package(tesseract_scene_graph REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(SCENE_GRAPH_MODEL_headers_MOC include/tesseract_qt/scene_graph/models/scene_state_model.h
             include/tesseract_qt/scene_graph/models/scene_graph_model.h)

add_library(
  ${PROJECT_NAME}_scene_graph_models SHARED
  ${SCENE_GRAPH_MODEL_headers_MOC}
  src/models/calibration_standard_item.cpp
  src/models/collision_standard_item.cpp
  src/models/dynamics_standard_item.cpp
  src/models/inertial_standard_item.cpp
  src/models/joint_standard_item.cpp
  src/models/limits_standard_item.cpp
  src/models/link_standard_item.cpp
  src/models/material_standard_item.cpp
  src/models/mimic_standard_item.cpp
  src/models/safety_standard_item.cpp
  src/models/visual_standard_item.cpp
  src/models/box_standard_item.cpp
  src/models/capsule_standard_item.cpp
  src/models/cone_standard_item.cpp
  src/models/cylinder_standard_item.cpp
  src/models/plane_standard_item.cpp
  src/models/sphere_standard_item.cpp
  src/models/polygon_mesh_standard_item.cpp
  src/models/octree_standard_item.cpp
  src/models/scene_graph_standard_item.cpp
  src/models/scene_state_standard_item.cpp
  src/models/scene_state_model.cpp
  src/models/scene_graph_model.cpp)

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
  include/tesseract_qt/scene_graph/widgets/scene_state_widget.h
  include/tesseract_qt/scene_graph/widgets/scene_graph_widget.h
  include/tesseract_qt/scene_graph/widgets/scene_graph_tool_bar.h)

add_library(
  ${PROJECT_NAME}_scene_graph_widgets SHARED
  ${SCENE_GRAPH_WIDGET_headers_MOC}
  src/widgets/scene_graph_widget.cpp
  src/widgets/scene_graph_tool_bar.cpp
  src/widgets/scene_state_widget.cpp)

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
list(
  APPEND
  PROJECT_TARGETS
  ${PROJECT_NAME}_scene_graph_models
  ${PROJECT_NAME}_scene_graph_widgets)
set(PROJECT_TARGETS ${PROJECT_TARGETS} PARENT_SCOPE)
install_targets(TARGETS ${PROJECT_NAME}_scene_graph_models ${PROJECT_NAME}_scene_graph_widgets)
