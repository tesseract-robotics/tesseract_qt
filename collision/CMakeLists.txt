find_package(tesseract_common REQUIRED)
find_package(tesseract_collision REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(COLLISION_MODEL_headers_MOC include/tesseract_qt/collision/models/contact_results_model.h)

add_library(
  ${PROJECT_NAME}_collision_models SHARED
  ${COLLISION_MODEL_headers_MOC}
  src/models/collision_margin_data_standard_item.cpp
  src/models/contact_request_standard_item.cpp
  src/models/contact_result_standard_item.cpp
  src/models/contact_result_vector_standard_item.cpp
  src/models/contact_results_model.cpp)

target_link_libraries(
  ${PROJECT_NAME}_collision_models
  PUBLIC tesseract::tesseract_collision_core
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_collision_models
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/collision>"
         "$<INSTALL_INTERFACE:include>")

qt5_wrap_cpp(COLLISION_WIDGET_headers_MOC include/tesseract_qt/collision/widgets/contact_results_compute_widget.h
             include/tesseract_qt/collision/widgets/contact_results_widget.h)
qt5_wrap_ui(COLLISION_WIDGET_headers_UI include/tesseract_qt/collision/widgets/contact_results_compute_widget.ui)

add_library(
  ${PROJECT_NAME}_collision_widgets SHARED
  ${COLLISION_WIDGET_headers_MOC}
  ${COLLISION_WIDGET_headers_UI}
  src/widgets/contact_results_widget.cpp
  src/widgets/contact_results_compute_widget.cpp)

target_link_libraries(
  ${PROJECT_NAME}_collision_widgets
  PUBLIC tesseract::tesseract_collision_core
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Widgets
         Qt5::Gui
         ${PROJECT_NAME}_collision_models
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_collision_widgets
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/collision>"
         "$<INSTALL_INTERFACE:include>")

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_collision_models ${PROJECT_NAME}_collision_widgets)
