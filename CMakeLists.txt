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

find_package(PkgConfig)
pkg_search_module(libnanomsg REQUIRED libnanomsg)
pkg_search_module(ArgParse REQUIRED ArgParse)

include_directories(${libnanomsg_INCLUDEDIR} ${ArgParse_INCLUDEDIR})

add_definitions(-std=c++11 -Wall -Wextra -Werror)

add_subdirectory(libksync)
add_subdirectory(server)
add_subdirectory(client)
