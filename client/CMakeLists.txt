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

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

add_executable(ksync_client src/client.cpp)
include_directories(inc ${ArgParse_INCLUDEDIR} ${libksync_INCLUDE_DIR} ${libnanomsg_INCLUDEDIR})
target_link_libraries(ksync_client ksync)
target_link_libraries(ksync_client ${ArgParse_LDFLAGS})
target_link_libraries(ksync_client ${libnanomsg_LDFLAGS})

install (TARGETS ksync_client DESTINATION bin)
