set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

add_executable(ksync_client src/client.cpp)
set_target_properties(ksync_client PROPERTIES COMPILE_FLAGS " -std=c++11 -Wall -Wextra -Werror")
include_directories(inc)
target_link_libraries(ksync_client libksync)

install (TARGETS ksync_client DESTINATION bin)
