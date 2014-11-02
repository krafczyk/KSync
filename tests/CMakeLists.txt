add_executable(string_test string-test.cpp)

include_directories(${ARGPARSE_INCLUDE_DIRECTORY} ${libksync_INCLUDE_DIR})
set_target_properties(string_test PROPERTIES COMPILE_FLAGS " -std=c++11 -Wall -Wextra -Werror")

target_link_libraries(string_test ksync ${ARGPARSE_LIBRARY})

install(TARGETS string_test DESTINATION bin)
