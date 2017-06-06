#KSync - Client-Server synchronization system using rsync.
#Copyright (C) 2014  Matthew Scott Krafczyk

#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)
#find_package(Qt5Core REQUIRED)
#find_package(Qt5Widgets REQUIRED)

find_package(G3LOG REQUIRED)
find_package(PkgConfig)
pkg_search_module(ArgParse REQUIRED ArgParse)
pkg_search_module(nanomsg REQUIRED nanomsg)
pkg_search_module(libzmq REQUIRED libzmq)
pkg_search_module(ncursesw REQUIRED ncursesw)
pkg_search_module(panelw REQUIRED panelw)
pkg_search_module(menuw REQUIRED menuw)
pkg_search_module(formw REQUIRED formw)
pkg_search_module(ncurses++w REQUIRED ncurses++w)

include_directories(inc)
include_directories(${core_INCLUDE_DIR})
include_directories(${comm_core_INCLUDE_DIR})
include_directories(${comm_zeromq_INCLUDE_DIR})
include_directories(${comm_nanomsg_INCLUDE_DIR})
include_directories(${G3LOG_INCLUDE_DIRS})
include_directories(${ArgParse_INCLUDEDIR})

add_executable(ksync_client src/client.cpp)
add_definitions(-pthread)

target_link_libraries(ksync_client ksync)
target_link_libraries(ksync_client ksync_comm_core)
target_link_libraries(ksync_client ksync_comm_zeromq)
target_link_libraries(ksync_client ksync_comm_nanomsg)
target_link_libraries(ksync_client ${G3LOG_LIBRARIES})
target_link_libraries(ksync_client ${ArgParse_LDFLAGS})
target_link_libraries(ksync_client ${libzmq_LDFLAGS})
target_link_libraries(ksync_client ${nanomsg_LDFLAGS})
target_link_libraries(ksync_client -lpthread)


include_directories(inc)
include_directories(${core_INCLUDE_DIR})
include_directories(${comm_core_INCLUDE_DIR})
include_directories(${comm_zeromq_INCLUDE_DIR})
include_directories(${comm_nanomsg_INCLUDE_DIR})
include_directories(${G3LOG_INCLUDE_DIRS})
include_directories(${ArgParse_INCLUDEDIR})
include_directories(${panelw_INCLUDE_DIR})
include_directories(${menuw_INCLUDE_DIR})
include_directories(${formw_INCLUDE_DIR})
include_directories(${ncursesw_INCLUDE_DIR})

add_executable(ksync_client_ncurses src/ncurses_client.cpp)
add_definitions(-pthread)

target_link_libraries(ksync_client_ncurses ksync)
target_link_libraries(ksync_client_ncurses ksync_comm_core)
target_link_libraries(ksync_client_ncurses ksync_comm_zeromq)
target_link_libraries(ksync_client_ncurses ksync_comm_nanomsg)
target_link_libraries(ksync_client_ncurses ${G3LOG_LIBRARIES})
target_link_libraries(ksync_client_ncurses ${ArgParse_LDFLAGS})
target_link_libraries(ksync_client_ncurses ${libzmq_LDFLAGS})
target_link_libraries(ksync_client_ncurses ${nanomsg_LDFLAGS})
target_link_libraries(ksync_client_ncurses ${panelw_LDFLAGS})
target_link_libraries(ksync_client_ncurses ${menuw_LDFLAGS})
target_link_libraries(ksync_client_ncurses ${formw_LDFLAGS})
target_link_libraries(ksync_client_ncurses ${ncursesw_LDFLAGS})
target_link_libraries(ksync_client_ncurses -lpthread)

install (TARGETS ksync_client ksync_client_ncurses DESTINATION bin)
