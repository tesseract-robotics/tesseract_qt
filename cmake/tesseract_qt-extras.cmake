list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/")
include(CMakeFindDependencyMacro)
find_dependency(Qwt)
find_dependency(Graphviz)
