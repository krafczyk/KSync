find_package(G3LOG REQUIRED)
find_package(PkgConfig)
pkg_search_module(nanomsg REQUIRED nanomsg)

include_directories(${libksync_INCLUDE_DIR})
include_directories(${G3LOG_INCLUDE_DIRS})
include_directories(${nanomsg_module_INCLUDE_DIR})
include_directories(${nanomsg_INCLUDEDIR})

add_library(ksync_nanomsg_module SHARED src/nanomsg_comm_system.cxx)

install (TARGETS ksync_nanomsg_module DESTINATION lib)
