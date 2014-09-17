add_library (libksync SHARED src/messages.cxx)
set_target_properties(libksync PROPERTIES COMPILE_FLAGS " -std=c++11 -Wall -Wextra -Werror")
include_directories(inc)
target_include_directories (libksync PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

install (TARGETS libksync DESTINATION lib)
install (DIRECTORY inc/ksync DESTINATION include FILES_MATCHING PATTERN "*.h")
