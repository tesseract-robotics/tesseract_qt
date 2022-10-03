# This creates the following target - PkgConfig::graphviz_cdt - PkgConfig::graphviz_cgraph - PkgConfig::graphviz_gvc -
# PkgConfig::graphviz_gvpr - PkgConfig::graphviz_lab_gamut - PkgConfig::graphviz_pathplan - PkgConfig::graphviz_xdot

find_package(PkgConfig REQUIRED)
pkg_search_module(
  graphviz_cdt
  ${REQUIRED}
  IMPORTED_TARGET
  GLOBAL
  libcdt)
pkg_search_module(
  graphviz_cgraph
  ${REQUIRED}
  IMPORTED_TARGET
  GLOBAL
  libcgraph)
pkg_search_module(
  graphviz_gvc
  ${REQUIRED}
  IMPORTED_TARGET
  GLOBAL
  libgvc)
pkg_search_module(
  graphviz_gvpr
  ${REQUIRED}
  IMPORTED_TARGET
  GLOBAL
  libgvpr)
pkg_search_module(
  graphviz_lab_gamut
  ${REQUIRED}
  IMPORTED_TARGET
  GLOBAL
  liblab_gamut)
pkg_search_module(
  graphviz_pathplan
  ${REQUIRED}
  IMPORTED_TARGET
  GLOBAL
  libpathplan)
pkg_search_module(
  graphviz_xdot
  ${REQUIRED}
  IMPORTED_TARGET
  GLOBAL
  libxdot)

# Output variables generation
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Graphviz
  DEFAULT_MSG
  graphviz_cdt_FOUND
  graphviz_cgraph_FOUND
  graphviz_gvc_FOUND
  graphviz_gvpr_FOUND
  graphviz_lab_gamut_FOUND
  graphviz_pathplan_FOUND
  graphviz_xdot_FOUND)
