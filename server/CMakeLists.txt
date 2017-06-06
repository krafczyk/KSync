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

find_package(G3LOG REQUIRED)
find_package(PkgConfig)
pkg_search_module(ArgParse REQUIRED ArgParse)
pkg_search_module(nanomsg REQUIRED nanomsg)
pkg_search_module(libzmq REQUIRED libzmq)

include_directories(inc)
include_directories(${core_INCLUDE_DIR})
include_directories(${comm_core_INCLUDE_DIR})
include_directories(${comm_zeromq_INCLUDE_DIR})
include_directories(${comm_nanomsg_INCLUDE_DIR})
include_directories(${G3LOG_INCLUDE_DIRS})
include_directories(${ArgParse_INCLUDEDIR})

add_executable(ksync_server src/master_thread.cpp src/gateway_thread.cpp)
add_definitions(-pthread)

target_link_libraries(ksync_server ksync)
target_link_libraries(ksync_server ksync_comm_core)
target_link_libraries(ksync_server ksync_comm_zeromq)
target_link_libraries(ksync_server ksync_comm_nanomsg)
target_link_libraries(ksync_server ${G3LOG_LIBRARIES})
target_link_libraries(ksync_server ${ArgParse_LDFLAGS})
target_link_libraries(ksync_server ${libzmq_LDFLAGS})
target_link_libraries(ksync_server ${nanomsg_LDFLAGS})
target_link_libraries(ksync_server -lpthread)

install (TARGETS ksync_server DESTINATION bin)
