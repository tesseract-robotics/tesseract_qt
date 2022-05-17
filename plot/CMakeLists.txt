find_package(tesseract_common REQUIRED)
find_package(
  Qt5
  COMPONENTS Core
             Gui
             Widgets
             OpenGL
             Xml
  REQUIRED)
find_package(Qwt REQUIRED)
find_package(qtadvanceddocking REQUIRED)

# Load variable for clang tidy args, compiler options and cxx version
tesseract_variables()

qt5_wrap_ui(
  PLOT_headers_UI
  include/tesseract_qt/plot/plot_widget_editor.ui
  include/tesseract_qt/plot/plot_widget_transforms.ui
  include/tesseract_qt/plot/plot_docker_toolbar.ui
  include/tesseract_qt/plot/transforms/first_derivative.ui
  include/tesseract_qt/plot/transforms/integral_transform.ui
  include/tesseract_qt/plot/transforms/moving_average_filter.ui
  include/tesseract_qt/plot/transforms/moving_rms.ui
  include/tesseract_qt/plot/transforms/outlier_removal.ui
  include/tesseract_qt/plot/transforms/scale_transform.ui
  include/tesseract_qt/plot/transforms/transform_selector.ui)

qt5_wrap_cpp(
  PLOT_headers_MOC
  include/tesseract_qt/plot/custom_tracker.h
  include/tesseract_qt/plot/plot_legend.h
  include/tesseract_qt/plot/plot_magnifier.h
  include/tesseract_qt/plot/plot_widget.h
  include/tesseract_qt/plot/plot_widget_base.h
  include/tesseract_qt/plot/plot_widget_editor.h
  include/tesseract_qt/plot/plot_widget_transforms.h
  include/tesseract_qt/plot/plot_zoomer.h
  include/tesseract_qt/plot/plot_docker.h
  include/tesseract_qt/plot/plot_subplots.h
  include/tesseract_qt/plot/transform_function.h
  include/tesseract_qt/plot/transforms/transform_selector.h)

add_library(
  ${PROJECT_NAME}_plot SHARED
  ${PLOT_headers_UI}
  ${PLOT_headers_MOC}
  src/custom_tracker.cpp
  src/plot_data.cpp
  src/plot_legend.cpp
  src/plot_magnifier.cpp
  src/plot_widget.cpp
  src/plot_widget_base.cpp
  src/plot_widget_editor.cpp
  src/plot_widget_transforms.cpp
  src/plot_zoomer.cpp
  src/plot_docker.cpp
  src/plot_subplots.cpp
  src/point_series_xy.cpp
  src/timeseries_qwt.cpp
  src/transform_function.cpp
  src/transforms/first_derivative.cpp
  src/transforms/integral_transform.cpp
  src/transforms/moving_average_filter.cpp
  src/transforms/moving_rms.cpp
  src/transforms/outlier_removal.cpp
  src/transforms/scale_transform.cpp
  src/transforms/transform_selector.cpp)
target_cxx_version(${PROJECT_NAME}_plot PUBLIC VERSION ${TESSERACT_CXX_VERSION})
target_link_libraries(
  ${PROJECT_NAME}_plot
  PUBLIC Qt5::Core
         Qt5::Gui
         Qt5::Widgets
         Qt5::OpenGL
         Qt5::Xml
         Qwt::Qwt
         tesseract::tesseract_common
         ads::qtadvanceddocking
         ${PROJECT_NAME}_common)
target_include_directories(
  ${PROJECT_NAME}_plot PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                              "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/plot>" "$<INSTALL_INTERFACE:include>")

add_subdirectory(demo)

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

install_targets(TARGETS ${PROJECT_NAME}_plot)
