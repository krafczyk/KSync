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

cmake_minimum_required (VERSION 2.8.11)
project (ksync)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(core_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/core/inc" CACHE INTERNAL "core include dir")
set(comm_core_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/comm/core/inc" CACHE INTERNAL "comm core include dir")
set(comm_zeromq_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/comm/zeromq/inc" CACHE INTERNAL "comm zeromq include dir")
set(comm_nanomsg_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/comm/nanomsg/inc" CACHE INTERNAL "comm nanomsg include dir")
set(ui_ncurses_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ui/ncurses/inc" CACHE INTERNAL "ui ncurses include dir")
set(client_core_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/client/core/inc" CACHE INTERNAL "client core include dir")

add_definitions(-std=c++11 -Wall -Wextra -Werror)

add_subdirectory(core)
add_subdirectory(comm)
add_subdirectory(ui)
add_subdirectory(server)
add_subdirectory(client)
