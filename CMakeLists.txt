cmake_minimum_required(VERSION 3.6.0)

# Extract package name and version
find_package(ros_industrial_cmake_boilerplate REQUIRED)
extract_package_metadata(pkg)
project(${pkg_extracted_name} VERSION ${pkg_extracted_version} LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

option(BUILD_RENDERING "Build the rendering module" ON)
option(BUILD_STUDIO "Build the studio module" ON)

add_subdirectory(common)
add_subdirectory(acm)
add_subdirectory(collision)
add_subdirectory(command_language)
add_subdirectory(environment)
add_subdirectory(joint_state_slider)
add_subdirectory(joint_trajectory)
add_subdirectory(kinematic_groups)
add_subdirectory(manipulation)
add_subdirectory(plot)
add_subdirectory(scene_graph)
add_subdirectory(planning)
add_subdirectory(srdf)
add_subdirectory(tool_path)
add_subdirectory(workbench)

set(DEPS
    "Qt5 COMPONENTS Core Gui Widgets Svg OpenGL Xml"
    "Boost COMPONENTS program_options"
    tesseract_environment
    tesseract_kinematics
    tesseract_scene_graph
    tesseract_common
    tesseract_support
    tesseract_urdf
    tesseract_srdf
    tesseract_visualization
    tesseract_command_language
    tesseract_motion_planners
    tesseract_task_composer)

if(${BUILD_RENDERING})
  message("Building rendering module")
  add_subdirectory(rendering)
  list(
    APPEND
    DEPS
    gz-rendering7
    gz-math7-eigen3
    "gz-common5 COMPONENTS profiler events av")
endif()

if(${BUILD_STUDIO} AND ${BUILD_RENDERING})
  message("Building studio module")
  add_subdirectory(studio)
  list(APPEND DEPS qtadvanceddocking)
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/cmake/FindQwt.cmake" "${CMAKE_CURRENT_LIST_DIR}/cmake/FindGraphviz.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME})

install(FILES "${CMAKE_CURRENT_LIST_DIR}/cmake/tesseract_qt-extras.cmake" DESTINATION lib/cmake/${PROJECT_NAME})

configure_package(
  NAMESPACE tesseract
            DEPENDENCIES
            ${DEPS}
            CFG_EXTRAS
            tesseract_qt-extras.cmake)
