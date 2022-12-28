find_package(tesseract_common REQUIRED)
find_package(tesseract_command_language REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_cpp(COMMAND_LANGUAGE_MODEL_headers_MOC include/tesseract_qt/command_language/composite_instruction_model.h)

add_library(
  ${PROJECT_NAME}_command_language_models SHARED
  ${COMMAND_LANGUAGE_MODEL_headers_MOC}
  src/cartesian_waypoint_standard_item.cpp
  src/command_language_events.cpp
  src/joint_waypoint_standard_item.cpp
  src/null_waypoint_standard_item.cpp
  src/state_waypoint_standard_item.cpp
  src/null_instruction_standard_item.cpp
  src/move_instruction_standard_item.cpp
  src/set_analog_instruction_standard_item.cpp
  src/set_tool_instruction_standard_item.cpp
  src/timer_instruction_standard_item.cpp
  src/wait_instruction_standard_item.cpp
  src/instruction_standard_item.cpp
  src/vector_instruction_standard_item.cpp
  src/composite_instruction_standard_item.cpp
  src/composite_instruction_model.cpp)

target_link_libraries(
  ${PROJECT_NAME}_command_language_models
  PUBLIC tesseract::tesseract_command_language
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Gui
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_command_language_models
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/command_language>" "$<INSTALL_INTERFACE:include>")

qt5_wrap_cpp(COMMAND_LANGUAGE_WIDGET_headers_MOC include/tesseract_qt/command_language/composite_instruction_widget.h)

add_library(${PROJECT_NAME}_command_language_widgets SHARED ${COMMAND_LANGUAGE_WIDGET_headers_MOC}
                                                            src/composite_instruction_widget.cpp)

target_link_libraries(
  ${PROJECT_NAME}_command_language_widgets
  PUBLIC tesseract::tesseract_command_language
         tesseract::tesseract_common
         Qt5::Core
         Qt5::Widgets
         Qt5::Gui
         ${PROJECT_NAME}_command_language_models
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_command_language_widgets
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
         "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/command_language>" "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

# Install
install_targets(TARGETS ${PROJECT_NAME}_command_language_models ${PROJECT_NAME}_command_language_widgets)
