find_package(G3LOG REQUIRED)

include_directories(inc)
include_directories(${core_INCLUDE_DIR})
include_directories(${comm_core_INCLUDE_DIR})
include_directories(${comm_zeromq_INCLUDE_DIR})
include_directories(${comm_nanomsg_INCLUDE_DIR})
include_directories(${G3LOG_INCLUDE_DIRS})

add_library (ksync_client_core SHARED src/client_state.cxx)

install (TARGETS ksync_client_core DESTINATION lib)
install (DIRECTORY inc/ksync DESTINATION include FILES_MATCHING PATTERN "*.h")
