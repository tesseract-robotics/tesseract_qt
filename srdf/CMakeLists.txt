find_package(tesseract_common REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(SRDF_EDITOR_WIDGET_headers_MOC include/tesseract_qt/srdf/srdf_editor_widget.h)
qt5_wrap_ui(SRDF_EDITOR_WIDGET_headers_UI include/tesseract_qt/srdf/srdf_editor_widget.ui)

set(SRDF_TARGETS ${PROJECT_NAME}_srdf)

add_library(${PROJECT_NAME}_srdf SHARED ${SRDF_EDITOR_WIDGET_headers_MOC} ${SRDF_EDITOR_WIDGET_headers_UI}
                                        src/srdf_editor_widget.cpp)
target_link_libraries(
  ${PROJECT_NAME}_srdf
  PUBLIC tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         ${PROJECT_NAME}_acm_widgets
         ${PROJECT_NAME}_kinematic_groups_widgets)
target_include_directories(
  ${PROJECT_NAME}_srdf PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                              "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/srdf>" "$<INSTALL_INTERFACE:include>")

if(${BUILD_RENDERING})
  add_executable(${PROJECT_NAME}_srdf_app src/srdf_editor_app.cpp)
  target_link_libraries(
    ${PROJECT_NAME}_srdf_app
    PRIVATE ${PROJECT_NAME}_srdf
            ${PROJECT_NAME}_render_widgets
            ${PROJECT_NAME}_environment_widgets
            tesseract::tesseract_common)

  list(APPEND SRDF_TARGETS ${PROJECT_NAME}_srdf_app)
endif()

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${SRDF_TARGETS})
