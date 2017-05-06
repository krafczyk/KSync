include_directories(${libksync_INCLUDE_DIR})
include_directories(${nanomsg_module_INCLUDE_DIR})
include_directories(${nanomsg_INCLUDEDIR})

add_library(ksync_nanomsg_module SHARED src/nanomsg_comm_system.cxx)
install (TARGETS ksync_nanomsg_module DESTINATION lib)
