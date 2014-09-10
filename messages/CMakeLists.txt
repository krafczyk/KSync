add_library (ksync_messages SHARED src/messages.cxx)
set_target_properties(ksync_messages PROPERTIES COMPILE_FLAGS " -std=c++11 -Wall -Wextra -Werror")
include_directories(inc)
target_include_directories (ksync_messages PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

install (TARGETS ksync_messages DESTINATION lib)
install (DIRECTORY inc/ksync DESTINATION include FILES_MATCHING PATTERN "*.h")
