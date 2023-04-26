cmake_minimum_required(VERSION 3.6.0)

# Extract package name and version
find_package(ros_industrial_cmake_boilerplate REQUIRED)
extract_package_metadata(pkg)
project(${pkg_extracted_name} VERSION ${pkg_extracted_version} LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

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
add_subdirectory(rendering)
add_subdirectory(scene_graph)
add_subdirectory(planning)
add_subdirectory(srdf)
add_subdirectory(tool_path)
add_subdirectory(workbench)
add_subdirectory(studio)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/cmake/FindQwt.cmake" "${CMAKE_CURRENT_LIST_DIR}/cmake/FindGraphviz.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME})

install(FILES "${CMAKE_CURRENT_LIST_DIR}/cmake/tesseract_qt-extras.cmake" DESTINATION lib/cmake/${PROJECT_NAME})

configure_package(
  NAMESPACE tesseract
            DEPENDENCIES
            "Qt5 COMPONENTS Core Gui Widgets Svg OpenGL Xml"
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
            tesseract_task_composer
            qtadvanceddocking
            ignition-rendering6
            ignition-math6-eigen3
            "ignition-common4 COMPONENTS profiler events av"
            CFG_EXTRAS
            tesseract_qt-extras.cmake)
