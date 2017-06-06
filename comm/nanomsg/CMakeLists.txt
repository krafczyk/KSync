find_package(G3LOG REQUIRED)
find_package(PkgConfig)
pkg_search_module(nanomsg REQUIRED nanomsg)

include_directories(${core_INCLUDE_DIR})
include_directories(${G3LOG_INCLUDE_DIRS})
include_directories(${comm_core_INCLUDE_DIR})
include_directories(${comm_nanomsg_INCLUDE_DIR})
include_directories(${nanomsg_INCLUDEDIR})

add_library(ksync_comm_nanomsg SHARED src/nanomsg_comm_system.cxx)
install (TARGETS ksync_comm_nanomsg DESTINATION lib)
