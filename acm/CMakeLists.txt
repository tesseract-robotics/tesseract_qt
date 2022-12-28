find_package(tesseract_common REQUIRED)
find_package(tesseract_scene_graph REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(ACM_MODEL_headers_MOC include/tesseract_qt/acm/allowed_collision_matrix_model.h)

add_library(${PROJECT_NAME}_acm_models SHARED ${ACM_MODEL_headers_MOC} src/allowed_collision_matrix_events.cpp
                                              src/allowed_collision_matrix_model.cpp)

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

qt5_wrap_cpp(
  ACM_WIDGET_headers_MOC
  include/tesseract_qt/acm/allowed_collision_matrix_widget.h
  include/tesseract_qt/acm/allowed_collision_matrix_editor_widget.h
  include/tesseract_qt/acm/add_allowed_collision_entry_dialog.h)

qt5_wrap_ui(ACM_WIDGET_headers_UI include/tesseract_qt/acm/allowed_collision_matrix_editor_widget.ui
            include/tesseract_qt/acm/add_allowed_collision_entry_dialog.ui)

add_library(
  ${PROJECT_NAME}_acm_widgets SHARED
  ${ACM_WIDGET_headers_MOC}
  ${ACM_WIDGET_headers_UI}
  src/allowed_collision_matrix_widget.cpp
  src/allowed_collision_matrix_editor_widget.cpp
  src/add_allowed_collision_entry_dialog.cpp)

target_link_libraries(
  ${PROJECT_NAME}_acm_widgets
  PUBLIC tesseract::tesseract_scene_graph
         tesseract::tesseract_common
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
install_targets(TARGETS ${PROJECT_NAME}_acm_models ${PROJECT_NAME}_acm_widgets)
