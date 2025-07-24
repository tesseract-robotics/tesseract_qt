cmake_minimum_required(VERSION 3.15.0)

# Extract package name and version
find_package(ros_industrial_cmake_boilerplate REQUIRED)
extract_package_metadata(pkg)
project(${pkg_extracted_name} VERSION ${pkg_extracted_version} LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Determine Ubuntu release
  find_program(LSB_RELEASE_EXEC lsb_release)
  execute_process(COMMAND ${LSB_RELEASE_EXEC} -cs OUTPUT_VARIABLE LSB_RELEASE_CODE_SHORT
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

option(BUILD_RENDERING "Build the rendering module" ON)
option(BUILD_STUDIO "Build the studio module" ON)

set(PROJECT_TARGETS)

add_subdirectory(common)
add_subdirectory(acm)
add_subdirectory(collision)
add_subdirectory(command_language)
add_subdirectory(joint_state_slider)
add_subdirectory(kinematic_groups)
add_subdirectory(scene_graph)
add_subdirectory(environment)
add_subdirectory(plot)
add_subdirectory(joint_trajectory)
add_subdirectory(tool_path)
add_subdirectory(manipulation)
add_subdirectory(srdf)
add_subdirectory(planning)
add_subdirectory(workbench)

set(DEPS
    "Qt5 COMPONENTS Core Gui Widgets Svg OpenGL Xml"
    "Boost COMPONENTS program_options"
    "tesseract_collision REQUIRED COMPONENTS core"
    "tesseract_kinematics REQUIRED COMPONENTS core opw"
    "tesseract_task_composer REQUIRED COMPONENTS core planning"
    tesseract_environment
    tesseract_scene_graph
    tesseract_common
    tesseract_urdf
    tesseract_srdf
    tesseract_visualization
    tesseract_command_language)

set(LINUX_DEPENDS
    "libboost-program-options-dev"
    "libqt5gui5"
    "libqt5widgets5"
    "libqt5svg5"
    "libqt5opengl5"
    "libqt5xml5"
    "libgraphviz-dev"
    "libqwt-qt5-6"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-common"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-scene-graph"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-environment"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-urdf"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-srdf"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-visualization"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-command-language"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-collision-core"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-kinematics-core"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-kinematics-opw"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-task-composer-core"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-task-composer-planning")

set(WINDOWS_DEPENDS
    "libboost-program-options-dev"
    "libqt5gui5"
    "libqt5widgets5"
    "libqt5svg5"
    "libqt5opengl5"
    "libqt5xml5"
    "libgraphviz-dev"
    "libqwt-qt5-6"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-common"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-scene-graph"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-environment"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-urdf"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-srdf"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-visualization"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-command-language"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-collision-core"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-kinematics-core"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-kinematics-opw"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-task-composer-core"
    "${TESSERACT_PACKAGE_PREFIX}tesseract-task-composer-planning")

if(${BUILD_RENDERING})
  message("Building rendering module")
  add_subdirectory(rendering)
  if((EXISTS LSB_RELEASE_CODE_SHORT) AND (LSB_RELEASE_CODE_SHORT STREQUAL "noble"))
    set(GZ_RENDERING gz-rendering8)
  else()
    set(GZ_RENDERING gz-rendering7)
  endif()
  list(
    APPEND
    DEPS
    ${GZ_RENDERING}
    gz-math7-eigen3
    "gz-common5 COMPONENTS profiler events av")
  list(
    APPEND
    LINUX_DEPENDS
    lib${GZ_RENDERING}-dev
    libgz-common5-dev
    libgz-math7-dev)
  list(
    APPEND
    WINDOWS_DEPENDS
    lib${GZ_RENDERING}-dev
    libgz-common5-dev
    libgz-math7-dev)
endif()

if(${BUILD_STUDIO} AND ${BUILD_RENDERING})
  message("Building studio module")
  list(APPEND DEPS "qtadvanceddocking NAMES qtadvanceddocking qt5advanceddocking qt6advanceddocking")
  add_subdirectory(studio)

  if((EXISTS LSB_RELEASE_CODE_SHORT) AND (LSB_RELEASE_CODE_SHORT STREQUAL "noble"))
    set(QT_ADS libqt-advanced-docking-system-dev)
  else()
    set(QT_ADS qt-advanced-docking-system)
  endif()

  list(APPEND LINUX_DEPENDS ${QT_ADS})
  list(APPEND WINDOWS_DEPENDS ${QT_ADS})
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/cmake/FindQwt.cmake" "${CMAKE_CURRENT_LIST_DIR}/cmake/FindGraphviz.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME})

configure_package(
  SKIP_INSTALL_TARGETS
  NAMESPACE tesseract
  TARGETS ${PROJECT_TARGETS}
  DEPENDENCIES ${DEPS}
  CFG_EXTRAS cmake/tesseract_qt-extras.cmake)

if(TESSERACT_PACKAGE)
  cpack(
    VERSION ${pkg_extracted_version}
    MAINTAINER_NAME ${pkg_extracted_maintainer_name}
    MAINTAINER_EMAIL ${pkg_extracted_maintainer_email}
    DESCRIPTION ${pkg_extracted_description}
    LICENSE_FILE ${CMAKE_CURRENT_LIST_DIR}/LICENSE
    README_FILE ${CMAKE_CURRENT_LIST_DIR}/README.md
    LINUX_DEPENDS ${LINUX_DEPENDS}
    WINDOWS_DEPENDS ${WINDOWS_DEPENDS})

  if(UNIX AND TESSERACT_PACKAGE_SOURCE)
    cpack_debian_source_package(
      CHANGLELOG ${CMAKE_CURRENT_LIST_DIR}/CHANGELOG.rst
      UPLOAD ${TESSERACT_PACKAGE_SOURCE_UPLOAD}
      DPUT_HOST ${TESSERACT_PACKAGE_SOURCE_DPUT_HOST}
      DEBIAN_INCREMENT ${TESSERACT_PACKAGE_SOURCE_DEBIAN_INCREMENT}
      DISTRIBUTIONS ${TESSERACT_PACKAGE_SOURCE_DISTRIBUTIONS})
  endif()
endif()
