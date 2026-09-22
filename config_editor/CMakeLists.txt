find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Load variable for compiler options and the C++ version.
tesseract_variables()

add_executable(tesseract_config_editor src/main.cpp src/config_editor_main_window.cpp)
target_link_libraries(
  tesseract_config_editor
  PRIVATE Qt5::Core
          Qt5::Gui
          Qt5::Widgets
          ${PROJECT_NAME}_common)
target_compile_options(tesseract_config_editor PRIVATE ${TESSERACT_COMPILE_OPTIONS_PRIVATE}
                                                       ${TESSERACT_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(tesseract_config_editor PRIVATE ${TESSERACT_COMPILE_DEFINITIONS_PRIVATE}
                                                           ${TESSERACT_COMPILE_DEFINITIONS_PUBLIC})
target_cxx_version(tesseract_config_editor PRIVATE VERSION ${TESSERACT_CXX_VERSION})

list(APPEND PROJECT_TARGETS tesseract_config_editor)
set(PROJECT_TARGETS ${PROJECT_TARGETS} PARENT_SCOPE)
install_targets(TARGETS tesseract_config_editor)
