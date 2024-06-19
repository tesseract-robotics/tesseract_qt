find_package(tesseract_common REQUIRED)
find_package(tesseract_scene_graph REQUIRED)
find_package(tesseract_environment REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS include/tesseract_qt/acm/widgets)

set(ACM_MODEL_headers_MOC include/tesseract_qt/acm/models/allowed_collision_matrix_model.h)

add_library(${PROJECT_NAME}_acm_models SHARED ${ACM_MODEL_headers_MOC} src/models/allowed_collision_matrix_model.cpp)

target_link_libraries(
  ${PROJECT_NAME}_acm_models
  PUBLIC tesseract::tesseract_scene_graph
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_acm_models PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                    "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/acm>" "$<INSTALL_INTERFACE:include>")

set(
  ACM_WIDGET_headers_MOC
  include/tesseract_qt/acm/widgets/allowed_collision_matrix_widget.h
  include/tesseract_qt/acm/widgets/allowed_collision_matrix_editor_widget.h
  include/tesseract_qt/acm/widgets/add_allowed_collision_entry_dialog.h)

add_library(
  ${PROJECT_NAME}_acm_widgets SHARED
  ${ACM_WIDGET_headers_MOC}
  src/widgets/allowed_collision_matrix_widget.cpp
  src/widgets/allowed_collision_matrix_editor_widget.cpp
  src/widgets/add_allowed_collision_entry_dialog.cpp)

target_link_libraries(
  ${PROJECT_NAME}_acm_widgets
  PUBLIC tesseract::tesseract_scene_graph
         tesseract::tesseract_common
         tesseract::tesseract_environment
         Qt5::Core
         Qt5::Widgets
         Qt5::Gui
         ${PROJECT_NAME}_acm_models
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_acm_widgets PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                     "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/acm>" "$<INSTALL_INTERFACE:include>")
add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
list(
  APPEND
  PROJECT_TARGETS
  ${PROJECT_NAME}_acm_models
  ${PROJECT_NAME}_acm_widgets)
set(PROJECT_TARGETS ${PROJECT_TARGETS} PARENT_SCOPE)
install_targets(TARGETS ${PROJECT_NAME}_acm_models ${PROJECT_NAME}_acm_widgets)
