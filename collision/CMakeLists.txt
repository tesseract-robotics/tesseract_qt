find_package(tesseract_common REQUIRED)
find_package(tesseract_collision REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(COLLISION_headers_MOC include/tesseract_qt/collision/contact_results_model.h
             include/tesseract_qt/collision/contact_results_widget.h)
qt5_wrap_ui(COLLISION_headers_UI include/tesseract_qt/collision/contact_results_widget.ui)

add_library(
  ${PROJECT_NAME}_collision SHARED
  ${COLLISION_headers_MOC}
  ${COLLISION_headers_UI}
  src/collision_margin_data_standard_item.cpp
  src/contact_request_standard_item.cpp
  src/contact_result_standard_item.cpp
  src/contact_result_vector_standard_item.cpp
  src/contact_results_events.cpp
  src/contact_results_model.cpp
  src/contact_results_widget.cpp)

target_link_libraries(
  ${PROJECT_NAME}_collision
  PUBLIC tesseract::tesseract_collision_core
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Widgets
         Qt5::Gui
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_collision PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                   "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/collision>" "$<INSTALL_INTERFACE:include>")

# add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_collision)
