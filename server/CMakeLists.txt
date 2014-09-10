add_executable(ksync_server src/server.cpp)
set_target_properties(ksync_server PROPERTIES COMPILE_FLAGS " -std=c++11 -Wall -Wextra -Werror")
include_directories(inc)
target_link_libraries(ksync_server ksync_messages)

install (TARGETS ksync_server DESTINATION bin)
