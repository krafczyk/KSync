find_package(G3LOG REQUIRED)
find_package(PkgConfig)
pkg_search_module(ncursesw REQUIRED ncursesw)
pkg_search_module(panelw REQUIRED panelw)
pkg_search_module(menuw REQUIRED menuw)
pkg_search_module(formw REQUIRED formw)

include_directories(${core_INCLUDE_DIR})
include_directories(${ui_ncurses_INCLUDE_DIR})
include_directories(${G3LOG_INCLUDE_DIRS})
include_directories(${panelw_INCLUDE_DIR})
include_directories(${menuw_INCLUDE_DIR})
include_directories(${formw_INCLUDE_DIR})
include_directories(${ncursesw_INCLUDE_DIR})

add_library (ksync_ui_ncurses SHARED src/window.cxx)

install (TARGETS ksync_ui_ncurses DESTINATION lib)
install (DIRECTORY inc/ksync DESTINATION include FILES_MATCHING PATTERN "*.h")
