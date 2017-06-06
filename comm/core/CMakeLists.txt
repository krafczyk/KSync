add_library(ksync_comm_core SHARED src/interface.cxx src/object.cxx)

include_directories(${core_INCLUDE_DIR})
include_directories(${comm_core_INCLUDE_DIR})

install (TARGETS ksync_comm_core DESTINATION lib)
install (DIRECTORY inc/ksync DESTINATION include FILES_MATCHING PATTERN "*.h")
